#include <iostream>
#include <sstream>
#include <string>
#include <jsoncpp/json/json.h>
#include <memory>

int main()
{
    // JSON 字符串
    std::string json_string = "{\"name\":\"张三\",\"age\":30, \"city\":\"北京\"}";

    Json::Value root;
    Json::Reader reader;

    bool ok = reader.parse(json_string, root);
    (void)ok;

    // 把序列化的字符串反序列化到了root中
    std::string name = root["name"].asString();
    int age = root["age"].asInt();
    std::string city = root["city"].asString();

    std::cout << name << std::endl;
    std::cout << age << std::endl;
    std::cout << city << std::endl;

    return 0;
}

// int main()
// {
//     Json::StreamWriterBuilder sBuilder;
//     sBuilder.settings_["emitUTF8"] = true;
//     Json::Value root;
//     root["name"] = "小明";
//     root["sex"] = "男";
//     root["age"] = 8;

//     std::unique_ptr<Json::StreamWriter> writer(sBuilder.newStreamWriter());
//     std::stringstream ss;
//      writer->write(root, &ss);
//      std::string s = ss.str();
//     std::cout << s << std::endl;




//     // Json::FastWriter writer;
//     // std::string s = writer.write(root);
//     // std::cout << s << std::endl;

//     // std::string s = root.toStyledString();
//     // std::cout << s << std::endl;

//     return 0;
// }