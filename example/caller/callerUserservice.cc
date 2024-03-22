#include<iostream>
#include"mprpcaplication.h"
#include"user.pb.h"
#include"mprpcchannel.h"
#include"mprpccontroller.h"
int main(int argc,char **argv){
    
    //整个程序启动以后，想要享受rpc服务调用，一定要先调用框架的初始化函数(只初始化一次)
    MprpcApplication::Init(argc,argv);
    //演示调用远程发布的rpc方法Login
    //这个类只能用RpcChannel对象初始化
    fixbug::UserServiceRpc_Stub stub(new Mprpcchannel()) ;
    //rpc的请求参数
    fixbug::LoginRequest request;
    request.set_name("张三");
    request.set_pwd("12345");
    //rpc方法的响应
    fixbug::LoginReponse response;
    Mprpccontroller controller;

    //发起rpc的调用，同步rpc调用的过程
    stub.Login(&controller, &request, &response, nullptr); // 这个实际上是调用了Mprpcchannel()的callMethod
    // 一次rpc调用完成，读调用完成的结果
    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    else
    {
        if (response.result().errcode() == 0)
        {
            std::cout << "调用正确" << std::endl;
        }
        else
        {
            std::cout << "调用错误" << std::endl;
        }
    }
    return 0;

}