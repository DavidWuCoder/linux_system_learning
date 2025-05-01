#include "Http.hpp"

void Login(HttpRequest &req, HttpResponse &resp)
{
    LOG(LogLevel::DEBUG) << req.Args() << ", 成功进入类处理数据的模块";
    std::string text = "hello: " + req.Args();
    resp.SetCode(200);
    resp.SetHeader("Content-Type", "text/plain");
    resp.SetHeader("Content-Length", std::to_string(text.size()));
    resp.SetText(text);
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage " << argv[0] << " port" << std::endl;
        exit(USAGE_ERR);
    }
    uint16_t port = std::stoi(argv[1]);

    std::unique_ptr<Http> httpsvr = std::make_unique<Http>(port);
    httpsvr->RegisterService("/login", Login);

    httpsvr->Start();
    return 0;
}