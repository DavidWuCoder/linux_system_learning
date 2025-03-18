#ifndef CHAIN_OF_RESPONSIBILITY_HPP
#define CHAIN_OF_RESPONSIBILITY_HPP

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <filesystem>
#include <stdexcept>
#include <fstream>
#include <sys/wait.h>

class HandlerText
{
public:
    virtual void Excute(const std::string& text) = 0;
    void SetNext(std::shared_ptr<HandlerText> next)
    {
        _next = next;
    }

    void Disable()
    {
        _enable = false;                   
    }

    void Enable()
    {
        _enable = true;
    }

    virtual ~HandlerText()
    {

    }                                                                                                
protected:
    std::shared_ptr<HandlerText> _next;
    bool _enable = true;
};                                       

// 对文本格式化处理
class HandlerTextFormat : public HandlerText
{
public:
    void Excute(const std::string& text) override
    {
        std::string format_result = text + '\n';
        if(_enable)
        {
            // 若被开启
            std::stringstream ss;
            ss << time(nullptr) << "-" << getpid() << "-" << text << "\n";
            format_result = ss.str();
            std::cout << "step 1 : 格式化消息, 结果：" << format_result << std::endl;
        }
        if (_next)
        {
            _next->Excute(format_result);
        }
        else
        {
            std::cout << "到达责任链处理结尾，完成责任链处理" << std::endl;
        }
    }
};

// 文件的基本信息
std::string defaultfilepath = "./tmp/";
std::string defaultfilename = "test.log";

// 对文本进行文件保存
class HandlerTextSaveFile : public HandlerText
{
public:
    HandlerTextSaveFile(const std::string &filepath = defaultfilepath,
         const std::string& filename = defaultfilename)
         :_filepath(filepath)
         , _filename(filename)
         {
            // 构造时，检测目录是否存在
            if (std::filesystem::exists(_filepath))
            {
                return;
            }
            try
            {
                std::filesystem::create_directories(_filepath);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            
         }
    void Excute(const std::string& text) override
    {
        if(_enable)
        {
            // 若被开启,保存到文件里
            std::string file = _filepath + _filename;
            std::ofstream ofs(file, std::ios::app);
            if (!ofs.is_open())
            {
                std::cerr << "open file is error" << std::endl;
                return;
            }
            ofs << text;
            ofs.close();
            std::cout << "step2 : 保存消息： " << text << "到文件" << std::endl;
        }
        if (_next)
        {
            _next->Excute(text);
        }
        else
        {
            std::cout << "到达责任链处理结尾，完成责任链处理" << std::endl;
        }
    }
private:
    std::string _filepath;
    std::string _filename;
};

const int defaultmaxline = 5;

// 检查长度，过长则打包备份
class HandlerTextBackup : public HandlerText
{
public:
    HandlerTextBackup(const std::string &filepath = defaultfilepath,
        const std::string& filename = defaultfilename
    , const int& maxline = defaultmaxline)
        :_filepath(filepath)
        , _filename(filename)
        , _maxline(maxline)
        {}
    void Excute(const std::string& text) override
    {
        if(_enable)
        {
            // 若被开启
            std::string file = _filepath + _filename;
            std::cout << "step 3 检查文件:" << file <<" 大小是否超范围" << std::endl;
            if (IsOutOfRange(file) )
            {
                // 如果超过范围则备份
                std::cout << "目标文件超范围，进行备份" << std::endl;
                Backup(file);
            }
        }
        if (_next)
        {
            _next->Excute(text);
        }
        else
        {
            std::cout << "到达责任链处理结尾，完成责任链处理" << std::endl;
        }
    }
private:
    bool IsOutOfRange(const std::string& file)
    {
        std::cout <<std::endl << file << std::endl << std::endl;
        std::ifstream ifs(file);
        if (!ifs.is_open())
        {
            std::cerr << "open file error: " << file << std::endl;
            exit(4);
            return false;
        }
        int lines = 0;
        std::string line;
        while (std::getline(ifs, line))
        {
            lines++;
        }

        ifs.close();
        return lines > _maxline;
    }

    void Backup(const std::string& file)
    {
        std::string suffix = std::to_string(time(nullptr));
        std::string backup_file = file + "." + suffix; // 备份文件
        // 只要文件名，不要路径
        std::string src_file = _filename + "." + suffix;
        std::string tar_file = src_file + ".tgz";
        // 1. 先对文件重命名， Linux上对文件重命名是原子的
        // 2. 让子进程进行备份
        // 3. 做打包操作，tgz， exec系统调用
        pid_t id = fork();

        // 3.打包 ，删除
        if (id == 0)
        {
            std::filesystem::rename(file, backup_file);
            std::cout << "step 4: 备份文件" << backup_file << std::endl;
            // 3.1更改工作路径
            std::filesystem::current_path(_filepath);
            // // 3.1.2调用tar命令打包
            execlp("tar", "tar", "-czf", tar_file.c_str(), src_file.c_str(), nullptr);
            exit(1);
        }

        int status;
        pid_t rid = waitpid(id, &status, 0);
        std::cout << rid << std::endl;
        if (rid > 0)
        {
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
            {
                // 打包成功，删除原文件
                std::filesystem::remove(backup_file);
                std::cout << "step5, 删除备份文件！" << std::endl;
            }
        }
    }

private:
    std::string _filepath;
    std::string _filename;
    int _maxline;
};

// 责任链的入口类
class HandlerEntry
{
public:
    HandlerEntry()
    {
        // 构造责任链节点
        _format = std::make_shared<HandlerTextFormat>();
        _save = std::make_shared<HandlerTextSaveFile>();
        _backup = std::make_shared<HandlerTextBackup>();

        // 设置责任链处理顺序
        _format->SetNext(_save);
        _save->SetNext(_backup);
    }

    void EnableHandler(bool isformat, bool issave, bool isbackup)
    {
        isformat ? _format->Enable() : _format->Disable();
        issave ? _save->Enable() : _save->Disable();
        isbackup ? _backup->Enable() : _backup->Disable();
    }

    void Run(const std::string& text)
    {
        _format->Excute(text);
    }

    ~HandlerEntry()
    {}

private:
    std::shared_ptr<HandlerText> _format;
    std::shared_ptr<HandlerText> _save;
    std::shared_ptr<HandlerText> _backup;
};

#endif
