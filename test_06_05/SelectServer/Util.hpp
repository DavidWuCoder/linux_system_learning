#pragma once

#include <iostream>
#include <fstream>
#include <string>

class Util
{
public:
    static bool ReadFileContent(const std::string &filename, std::string* out)
    {
        // version1: 默认以文本形式读取
        // std::ifstream in(filename);
        // if (!in.is_open())
        // {
        //     return false;
        // }
        // std::string line;
        // while (std::getline(in, line))
        // {
        //     *out += line;
        // }
        // in.close();
        // return true;

        // version2, 采用二进制方式读取
        int filesize = FileSize(filename);
        if (filesize > 0)
        {
            std::ifstream in(filename, std::ios::binary);
            if (!in.is_open())
            {
                return false;
            }
            out->resize(filesize);
            in.read((char *)out->c_str(), filesize);
            in.close();
        }
        else
        {
            return false;
        }
        return true;
    }

    // static bool ReadFileContent(const std::string &filename, std::string *out)
    // {
    //     std::ifstream in(filename, std::ios::binary | std::ios::ate); // 直接定位到文件末尾

    //     if (!in.is_open())
    //     {
    //         return false;
    //     }

    //     const std::streamsize filesize = in.tellg(); // 直接通过流获取文件大小
    //     in.seekg(0, std::ios::beg);

    //     if (filesize <= 0)
    //     {
    //         return false;
    //     }

    //     out->clear();
    //     out->resize(filesize);
    //     in.read(reinterpret_cast<char *>(out->data()), filesize); // C++17正确访问

    //     // 处理可能的读入不完整
    //     return in.good() && (filesize == in.gcount());
    // }

    static bool ReadOneLine(std::string &bigstr, std::string *out, const std::string &sep)
    {
        auto pos = bigstr.find(sep);
        if (pos == std::string::npos)
        {
            return false;
        }
        *out = bigstr.substr(0, pos);
        bigstr.erase(0, pos + sep.size());
        return true;
    }
    static int FileSize(const std::string &filename)
    {
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open())
        {
            return -1;
        }

        in.seekg(0, in.end);
        int filesize = in.tellg();

        in.seekg(0, in.beg);
        in.close();

        return filesize;
    }
};