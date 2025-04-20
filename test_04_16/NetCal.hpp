#pragma once

#include "Protocol.hpp"
#include <iostream>

class Cal
{
public:
    Response Execute(Request& req)
    {
        Response resp;

        std::cout << "Excute: " << req.X() << ' ' << req.Oper() << ' ' << req.Y() << std::endl;
        switch(req.Oper())
        {
        case '+':
            resp.SetResult(req.X() + req.Y());
            break;
        case '-':
            resp.SetResult(req.X() - req.Y());
            break;
        case '*':
            resp.SetResult(req.X() * req.Y());
            break;
        case '/':
            if (req.Y() == 0)
            {
                resp.SetCode(1); // 1 除零错误
            }
            else
            {
                resp.SetResult(req.X() / req.Y());
            }
            break;
        case '%':
            if (req.Y() == 0)
            {
                resp.SetCode(2); 
            }
            else
            {
                resp.SetResult(req.X() % req.Y());
            }
            break;
        default:
            resp.SetCode(3);
            break;
        }
        return resp;
    }
};