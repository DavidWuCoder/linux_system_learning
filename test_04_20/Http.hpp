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
    HttpResponse() :_blankline(gline)
    {}

    // 序列化.
    // 成熟的http,应答做序列化不要依赖任何第三方库
    std::string Serialize()
    {
        std::string status_line = _version + gspace + std::to_string(_code) + gspace + _desc + gline;
        std::string resp_header;
        for (auto& header : _headers)
        {
            std::string line = header.first + glinesep + header.second;
            resp_header += line;
        }

        return status_line + resp_header + _blankline + _text;
    }

    bool Deserialize(std::string& reqstr)
    {
        return true;
    }

    ~HttpResponse()
    {}

// private:
public:
    std::string _version;
    int _code;
    std::string _desc;

    std::unordered_map<std::string, std::string> _headers;
    std::string _blankline;
    std::string _text;
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

            std::string filename = req.Uri();

            LOG(LogLevel::DEBUG) << "用户请求：" << filename;
            bool res = Util::ReadFileContent(filename, &resp._text);
            (void)res;
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