#pragma once

#include <jsoncpp/json/json.h>

#include <functional>
#include <sstream>
#include <string>

#include "Socket.hpp"

using namespace SocketModule;

const std::string sep = "\r\n";

// len\r\n{json string}\r\n
// 50
// {"x": 10, "y" : 20, "oper": +}
class Request
{
public:
    Request(int x, int y, char oper) : _x(x), _y(y), _oper(oper) {}

    std::string Serialize()
    {
        Json::Value root;
        root["x"] = _x;
        root["y"] = _y;
        root["oper"] = _oper;

        Json::StreamWriterBuilder sBuilder;
        sBuilder.settings_["emitUTF8"] = true;

        std::unique_ptr<Json::StreamWriter> writer(sBuilder.newStreamWriter());
        std::stringstream ss;
        writer->write(root, &ss);
        std::string s = ss.str();
        return s;
    }

    bool Deserialize(std::string& in)
    {
        Json::Value root;
        Json::Reader reader;
        bool ok = reader.parse(in, root);
        if (ok)
        {
            _x = root["x"].asInt();
            _y = root["y"].asInt();
            _oper = root["oper"].asInt();
        }
        return ok;
    }

    char Oper() { return _oper; }

    int X() { return _x; }

    int Y() { return _y; }

    Request() {}

private:
    int _x;
    int _y;
    char _oper;  // + - * % / -> _x _oper _y
};

// 使用现成的方案，json-> jsoncpp

// server to client
class Response
{
public:
    Response(int result = 0, int code = 0) : _result(result), _code(code) {}

    std::string Serialize()
    {
        Json::Value root;
        root["result"] = _result;
        root["code"] = _code;

        Json::StreamWriterBuilder sBuilder;
        sBuilder.settings_["emitUTF8"] = true;

        std::unique_ptr<Json::StreamWriter> writer(sBuilder.newStreamWriter());
        std::stringstream ss;
        writer->write(root, &ss);
        std::string s = ss.str();
        return s;
    }

    bool Deserialize(std::string& in)
    {
        Json::Value root;
        Json::Reader reader;
        bool ok = reader.parse(in, root);
        if (ok)
        {
            _result = root["result"].asInt();
            _code = root["code"].asInt();
        }
        return ok;
    }

    void ShowResult()
    {
        std::cout << "结果是：" << _result << "[" << _code << "]" << std::endl;
    }

    ~Response() {}

    void SetResult(int res) { _result = res; }

    void SetCode(int code) { _code = code; }

private:
    int _result;  // 运算结果
    int _code;    // 不同的运算异常的情况
};

using func_t = std::function<Response(Request& req)>;

class Protocol
{
public:
    Protocol() {}

    Protocol(func_t func) : _func(func) {}

    std::string Encode(const std::string jsonstr)
    {
        std::string len = std::to_string(jsonstr.size());
        std::string package = len + sep + jsonstr + sep;  // 封装报头
        return package;
    }

    // 1.判断报文完整性
    // 如果包含至少一个完整请求，提取它，并从缓冲区移除
    bool Decode(std::string& buffer, std::string* package)
    {
        ssize_t pos = buffer.find(sep);
        if (pos == std::string::npos)
        {
            return false;  // 让调用方继续读取数据
        }
        std::string package_len_str = buffer.substr(0, pos);
        int package_len_int = std::stoi(package_len_str);
        // 此时buffer一定有长度，但不一定有完整报文
        int target_len =
            package_len_str.size() + package_len_int + 2 * sep.size();

        if (buffer.size() < target_len)
        {
            // 不存在完整报文
            return false;
        }
        // buffer 一定至少有一个完整的报文
        *package = buffer.substr(pos + sep.size(), package_len_int);
        buffer.erase(0, target_len);
        return true;
    }

    std::string Excute(std::string& package)
    {
        Request req;
        bool ok = req.Deserialize(package);
        if (!ok)
        {
            return std::string();
        }

        Response resp = _func(req);

        std::string json_str = resp.Serialize();

        std::string send_str = Encode(json_str);

        return send_str;
    }

    std::string BuildRequestString(int x, int y, char oper)
    {
        Request req(x, y, oper);

        // std::cout << req.X() << ' ' << req.Y() << req.Oper() << std::endl;

        std::string json_req = req.Serialize();

        // 2.1 debug
        // std::cout << "========json_req string =========" << std::endl;
        // std::cout << json_req << std::endl;
        // std::cout << "=================================" << std::endl;

        return Encode(json_req);
    }

    bool GetResponse(std::shared_ptr<Socket>& client, std::string& resp_buffer,
                     Response* resp)
    {
        int n = client->Recv(&resp_buffer);

        while (true)
        {
            if (n > 0)
            {
                // std::cout << "------resp_bufer--------" << std::endl;
                // std::cout << resp_buffer << std::endl;
                // std::cout << "------------------------" << std::endl;
                // 成功
                std::string json_package;

                bool ret = Decode(resp_buffer, &json_package);
                if (!ret)
                {
                    continue;
                }

                // std::cout << "------json_package--------" << std::endl;
                // std::cout << json_package << std::endl;
                // std::cout << "------------------------" << std::endl;

                resp->Deserialize(json_package);
                return true;
            }
            else if (n == 0)
            {
                std::cout << "server quit" << std::endl;
                return true;
            }
            else
            {
                std::cerr << "server quit" << std::endl;
                return false;
            }
        }
    }

    ~Protocol() {}

private:
    Request _req;
    Response _resq;
    func_t _func;
};
