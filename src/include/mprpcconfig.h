#pragma once
#include<unordered_map>
#include<string>
//框架读取配置文件类
//配置文件里面包含了rpcserverip，rpcserverport，zookeeperip,zookeeperport
class MprpcConfig
{
public:
    //负责解析配置文件
    void LoadConfig(const char*config_file);
    //查询配置项的信息
    std::string Load(const std::string& Key);
private:
   std::unordered_map<std::string,std::string> m_configMap;
   //去掉字符串前后的空格
   void RemoveSpace(std::string &nbuf);

};


