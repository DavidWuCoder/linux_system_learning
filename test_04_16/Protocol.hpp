#pragma once

#include <string>
#include <sstream>
#include <functional>
#include <jsoncpp/json/json.h>
#include "Socket.hpp"

using namespace SocketModule;


const std::string sep = "\r\n";

// len\r\n{json string}\r\n
// 50
// {"x": 10, "y" : 20, "oper": +}
class Request
{
public:

    Request(int x, int y , char oper):_x(x), _y(y), _oper(oper)
    {}

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
            _x = root["y"].asInt();
            _oper = root["oper"].asInt();
        }
        return ok;
    }

    char Oper()
    {
        return _oper;
    }

    int X()
    {
        return _x;
    }

    int Y()
    {
        return _y;
    }

    Request()
    {}
private:
    int _x;
    int _y;
    char _oper; // + - * % / -> _x _oper _y
};

// 使用现成的方案，json-> jsoncpp

// server to client
class Response
{
public:
    Response(int result = 0, int code = 0)
        :_result(result)
        ,_code(code)
    {}

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

    ~Response()
    {}

    void SetResult(int res)
    {
        _result = res;
    }

    void SetCode(int code)
    {
        _code = code;
    }
private:
    int _result; // 运算结果
    int _code; // 不同的运算异常的情况
};

using func_t = std::function<Response (Request &req)>;

class Protocol
{
public:
    Protocol(func_t func)
        : _func(func)
    {}

    std::string Encode(const std::string jsonstr)
    {
        std::string len = std::to_string(jsonstr.size());
        std::string package = len + sep + jsonstr + sep;  // 封装报头
        return package;
    }

    // 1.判断报文完整性
    // 如果包含至少一个完整请求，提取它，并从缓冲区移除
    bool Decode(std::string& buffer, std::string *package)
    {
        ssize_t pos = buffer.find(sep);
        if (pos == std::string::npos)
        {
            return false; // 让调用方继续读取数据 
        }
        std::string package_len_str = buffer.substr(0, pos);
        int package_len_int = std::stoi(package_len_str);
        // 此时buffer一定有长度，但不一定有完整报文
        int target_len = package_len_str.size() + package_len_int + 2 * sep.size();

        if (buffer.size() < target_len)
        {
            // 不存在完整报文
            return false;
        }
        // buffer 一定至少有一个完整的报文
        *package = buffer.substr(pos+sep.size(), package_len_int);
        buffer.erase(0, target_len);
        return true;
    }

    void GetRequest(std::shared_ptr<Socket> &sock, InetAddr& client)
    {
        // 读取
        std::string inbuffer;
        while (true)
        {
            int n = sock->Recv(&inbuffer);
            if (n > 0)
            {
                // 1.解析报文，提取完整的json
                std::string json_package;
                bool ret = Decode(inbuffer, &json_package);
                if (!ret)
                    continue;
                // 一定拿到一个完整报文
                // 2. 反序列化
                Request req;
                bool ok = req.Deserialize(json_package);
                if (!ok)
                {
                    continue;
                }
                // 3.一定得到了req了
                // req->resp, 进行计算
                Response resp = _func(req);

                // 4.序列化
                std::string json_str = resp.Serialize();

                // 5.添加报头
                std::string send_str = Encode(json_str);

                // 6. 直接发送
                sock->Send(send_str);
            }
            else if (n == 0)
            {
                LOG(LogLevel::INFO) << "client:" << client.StringAddr() << "Quit";
                break;
            }
            else
            {
                LOG(LogLevel::WARNING) << "client:" << client.StringAddr() << ", recv error";
                break;
            }
        }
        // sock->Close();
    }

    ~Protocol()
    {}


private:
    Request _req;
    Response _resq;
    func_t _func;
};