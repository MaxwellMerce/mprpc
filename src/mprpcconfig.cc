#include "mprpcconfig.h"
#include <iostream>
#include <string>
void MprpcConfig::LoadConfig(const char *config_file)
{
    FILE *pf = fopen(config_file, "r");
    if (pf == nullptr)
    {
        std::cout << "配置文件不存在" << std::endl;
        exit(EXIT_FAILURE);
    }
    // 文件里面会含有：注释，有用的信息，空格
    while (!feof(pf))
    {
        char buf[512] = {0};
        fgets(buf, 512, pf);
        // 去掉前面多余的空格
        std::string nbuf(buf);
        RemoveSpace(nbuf);
        // 去掉注释
        if (nbuf[0] == '#' || nbuf.empty())
            continue;

        // 解析配置项
        int idx = nbuf.find('=');
        if (idx == -1)
        {
            // 配置不合法
            continue;
        }
        std::string key;
        std::string val;
        key = nbuf.substr(0, idx);
        RemoveSpace(key);
        int endidx=nbuf.find('\n',idx);
        val = nbuf.substr(idx + 1, endidx - idx-1);
        RemoveSpace(val);
        m_configMap.insert({key, val});
    }
}
std::string MprpcConfig::Load(const std::string &Key)
{
    auto fd = m_configMap.find(Key);
    if (fd == m_configMap.end())
    {
        return "";
    }
    return fd->second;
}

void MprpcConfig::RemoveSpace(std::string &nbuf)
{
    int idx = nbuf.find_first_not_of(' ');
    if (idx != -1)
    {
        nbuf = nbuf.substr(idx, nbuf.size() - idx);
    }
    // 去掉后面的空格
    idx = nbuf.find_last_not_of(' ');
    if (idx != -1)
    {
        nbuf = nbuf.substr(0, idx + 1);
    }
}