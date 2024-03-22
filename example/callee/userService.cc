#include<iostream>
#include<string>
#include"../user.pb.h"
#include"mprpcaplication.h"
#include"rpcprovider.h"
#include"logger.h"
/*
本地方法提供了login和getfriendlist
*/

class UserService:public fixbug::UserServiceRpc{
public:
    bool Login(std::string name,std::string pwd){
        std::cout<<"name  "<<name<<std::endl;
        std::cout<<"pwd  "<<pwd<<std::endl;
        return true;
    }

    //重写基类UserServiceRpc的虚函数 下面这些方法是框架直接调用的
    /*
       caller是一个请求发起者
       caller ===> login(LoginRequest) => muduo => callee
       callee ===> login(LoginRequest) =>交到下面重写的login方法上来
    */
    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginReponse* response,
                       ::google::protobuf::Closure* done)
    {
        //框架给业务上报了请求参数LoginRequest，引用获取相应数据做本地业务
        std::string name=request->name();
        std::string pwd=request->pwd();

        //做本地业务
        bool Login_result=Login(name,pwd);

        //本地业务做完了，把响应写入,包括错误码返回消息以及返回值
        response->set_success(Login_result);
        fixbug::ResultCode* code=response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");

        //执行回调操作 执行响应对象序列化和网络发送，这当然是由框架来完成的
        done->Run();
        
    }
};

int main(int argc, char **argv){
    LOG_INFO("first log message");
    LOG_INFO("%s:%s:%d",__FILE__,__FUNCTION__,__LINE__);
    //框架的初始化操作
    MprpcApplication::Init(argc,argv);
    //初始化一个rpc提供节点
    RpcProvider provider;
    provider.NotifyService(new UserService());
    //运行等待调用者调用
    provider.Run();

    return 0;
}