#pragma once

#include "TcpServer.hpp"
#include "Util.hpp"
#include "Log.hpp"
#include <memory>
#include <string>
#include <sstream>

using namespace LogModule;

const std::string gspace = " ";
const std::string gline = "\r\n";
const std::string glinesep = ": ";

const std::string webroot = "./wwwroot";
const std::string homepage = "index.html";
const std::string page_404 = "/404.html";

class HttpRequest
{
public:
    HttpRequest()
    {
    }

    std::string Uri()
    {
        return _uri;
    }

    std::string Serialize()
    {
        return "";
    }

    void ParseReqLine(std::string &reqline)
    {
        std::stringstream ss(reqline);
        ss >> _method >> _uri >> _version;
    }

    bool Deserialize(std::string &reqstr)
    {
        // 1. 提取请求行
        std::string reqline;
        bool res = Util::ReadOneLine(reqstr, &reqline, gline);
        LOG(LogLevel::DEBUG) << reqline;

        // 2.对请求行进行反序列化
        // GET / HTTP/1.1
        ParseReqLine(reqline);
        LOG(LogLevel::DEBUG) << "Method  " << _method;
        LOG(LogLevel::DEBUG) << "version  " << _version;
        LOG(LogLevel::DEBUG) << "_uri  " << _uri;

        if (_uri == "/")
        {
            _uri = webroot + _uri + homepage;
        }
        else
        {
            _uri = webroot + _uri;
        }

        return true;
    }

    ~HttpRequest()
    {
    }

private:
    std::string _method;
    std::string _uri;
    std::string _version;

    std::unordered_map<std::string, std::string> _headers;
    std::string _blanksize;
    std::string _text;
};

class HttpResponse
{
public:
    HttpResponse() : _blankline(gline), _version("HTTP/1.0")
    {
    }

    // 序列化.
    // 成熟的http,应答做序列化不要依赖任何第三方库
    std::string Serialize()
    {
        std::string status_line = _version + gspace + std::to_string(_code) + gspace + _desc + gline;
        std::string resp_header;
        for (auto &header : _headers)
        {
            std::string line = header.first + glinesep + header.second + gline;
            resp_header += line;
        }

        return status_line + resp_header + _blankline + _text;
    }

    bool Deserialize(std::string &reqstr)
    {
        return true;
    }

    void SetTargetFile(const std::string &target)
    {
        _targetfile = target;
    }

    void SetCode(int code)
    {
        _code = code;
        switch(code)
        {
            case 200:
                _desc = "ok";
                break;
            case 404:
                _desc = "Not Found";
                break;
            default:
                break;
        }
    }

    void SetHeader(const std::string key, const std::string value)
    {
        auto iter = _headers.find(key);
        if (iter != _headers.end())
        {
            return;
        }
        _headers[key] = value;
    }

    // 提取后缀
    // 映射
    std::string Uri2Suffix(std::string& targetfile)
    {
        auto pos = targetfile.rfind(".");
        if (pos == std::string::npos)
        {
            return "text/html";
        }
        std::string suffix = targetfile.substr(pos);
        if (suffix == ".html" || suffix == ".htm")
        {
            return "text/html";
        }
        else if (suffix == ".jpg")
        {
            return "image/jpeg";
        }
        else if (suffix == ".png")
        {
            return "image/png";
        }
        else
        {
            return "";
        }
    }

    bool MakeResponse()
    {
        if (_targetfile == "./wwwroot/favicon.ico")
        {
            LOG(LogLevel::INFO) << "用户请求" << _targetfile << "忽略它";
            return false;
        }
        int filesize = 0;
        bool res = Util::ReadFileContent(_targetfile, &_text);
        if (!res)
        {
            _text = "";
            LOG(LogLevel::WARNING) << "client want to get " << _targetfile << " but not found";
            SetCode(404);
            _targetfile = webroot + page_404;
            filesize = Util::FileSize(_targetfile);
            Util::ReadFileContent(_targetfile, &_text);
            std::string suffix = Uri2Suffix(_targetfile);
            SetHeader("Content-Type", suffix); 
            // _headers["Content-Type"] += "; charset=utf-8";
            SetHeader("Content-Length", std::to_string(filesize));
        }
        else
        {
            SetCode(200);
            filesize = Util::FileSize(_targetfile);
            std::string suffix = Uri2Suffix(_targetfile);
            SetHeader("Content-Type", suffix); 
            // _headers["Content-Type"] += "; charset=utf-8\r\n";
            SetHeader("Content-Length", std::to_string(filesize));
        }
        return true;
    }

    ~HttpResponse()
    {
    }

    // private:
public:
    std::string _version;
    int _code;
    std::string _desc;

    std::unordered_map<std::string, std::string> _headers;
    std::string _blankline;
    std::string _text;

    // 其他属性
    std::string _targetfile;
};

class Http
{
public:
    Http(uint16_t port)
        : tsvrp(std::make_unique<TcpServer>(port))
    {
    }

    void HandlerHttpRequest(std::shared_ptr<Socket> &sock, InetAddr &client)
    {
        // http面向字节流，如何保证请求完整？
        std::string httpreqstr;
        // 1.寻找空行，提取报头
        // 2. 从报头提出一个属性： Content: 有效载荷的长度
        // 3. 从剩余部分提出相应长度的正文内容即可
        // to do

        int n = sock->Recv(&httpreqstr);
        if (n > 0)
        {
            // 对报文完整性进行审核
            HttpRequest req;
            req.Deserialize(httpreqstr);
            HttpResponse resp;
            resp._version = "HTTP/1.1";
            resp._code = 200; // success
            resp._desc = "ok";

            resp.SetTargetFile(req.Uri());
            resp.MakeResponse();

            // LOG(LogLevel::DEBUG) << "用户请求：" << filename;
            // bool res = Util::ReadFileContent(filename, &resp._text);
            // (void)res;
            std::string response_str = resp.Serialize();
            sock->Send(response_str);
        }

#define DEBUG
#ifndef DEBUG
#define DEBUG
        std::string httpreqstr;
        sock->Recv(&httpreqstr);
        std::cout << httpreqstr << std::endl;

        // 测试，直接构建http应答
        HttpResponse resp;
        resp._version = "HTTP/1.1";
        resp._code = 200; // success
        resp._desc = "ok";

        std::string filename = webroot + homepage;

        bool res = Util::ReadFileContent(filename, &resp._text);

        std::string response_str = resp.Serialize();
        sock->Send(response_str);
#endif
    }

    void Start()
    {
        tsvrp->Start([this](std::shared_ptr<Socket> &sock, InetAddr &client)
                     { this->HandlerHttpRequest(sock, client); });
    }

    ~Http()
    {
    }

private:
    std::unique_ptr<TcpServer> tsvrp;
};