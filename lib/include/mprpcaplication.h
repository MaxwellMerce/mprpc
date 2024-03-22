#pragma once
#include"mprpcconfig.h"
//mprpc框架的基础类,初始化框架
class MprpcApplication{
public:
    //定义成静态成员，是他们变成所有成员共享的东西
    static void Init(int argc,char** argv);
    static MprpcApplication& GetInstance();//获取实例
    static MprpcConfig& GetConfig();
private:  
    static MprpcConfig m_config;
    MprpcApplication(){};
    //禁止拷贝构造
    MprpcApplication(const MprpcApplication&)=delete;
    MprpcApplication(MprpcApplication&&)=delete;
};