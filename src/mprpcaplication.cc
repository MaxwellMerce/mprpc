#include"mprpcaplication.h"
#include<iostream>
#include <unistd.h>
#include<string>
MprpcConfig MprpcApplication::m_config;
void ShowArgsHelp(){
    std::cout<<"format: command -i <configue>"<<std::endl;
}
void MprpcApplication::Init(int argc,char** argv){
    if(argc<2){
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }
    
    int c=0;
    std::string config_file;
    while((c=getopt(argc,argv,"i:"))!=-1){
        switch (c)
        {
        case 'i':
            config_file=optarg;
            break;
        case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }
    //开始加载配置文件 
    m_config.LoadConfig(config_file.c_str());
    //std::cout<<"rpcserverip"<<" "<<m_config.Load("rpcserverip")<<std::endl;

}
MprpcApplication& MprpcApplication::GetInstance(){
    static MprpcApplication app;
    return app;
}

 MprpcConfig& MprpcApplication::GetConfig(){
    return m_config;
 }