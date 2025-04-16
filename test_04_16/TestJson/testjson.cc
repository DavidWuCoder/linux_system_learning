#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>

int main()
{
    Json::StreamWriterBuilder wBuilder;
    Json::Value def;
    def["emitUTF8"] = true;
    Json::Value root;
    wBuilder.setDefaults(&def);
    root["name"] = "小明";
    root["sex"] = "男";
    root["age"] = "18";

    Json::FastWriter writer;
    std::string s = writer.write(root);
    std::cout << s << std::endl;

    // std::string s = root.toStyledString();
    // std::cout << s << std::endl;

    return 0;
}