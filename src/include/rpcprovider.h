#pragma once
#include"google/protobuf/service.h"
#include<memory>
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>
#include<muduo/net/TcpConnection.h>
#include<google/protobuf/descriptor.h>
#include<string>
#include<unordered_map>
#include"logger.h"
//框架提供的专门服务发布rpc服务的网络对象类
class RpcProvider{
public:
   //这里是框架提供给外部使用的，可以发布rpc服务的接口
   void NotifyService(google::protobuf::Service* service);
   //启动rpc服务节点开始提供rpc远程调用服务
   void Run();
private:
   muduo::net::EventLoop m_eventLoop;
   //服务相关信息
   struct ServiceInfo{
       google::protobuf::Service *m_service;
       std::unordered_map<std::string,const google::protobuf::MethodDescriptor*>m_methodmap;//保存服务方法
   };
   //存放注册成功的方法
   std::unordered_map<std::string,ServiceInfo>m_servicemap;
   //新的socket连接回调
   void OnConnection(const muduo::net::TcpConnectionPtr& conn);
   //已建立连接的读写事件的回调当有一个rpc服务请求到达，这个方法便会响应
   void OnMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buffer,muduo::Timestamp);
   //Closure回调操作,用于序列化rpc的响应和网络发送
   void SendRpcResponse(const muduo::net::TcpConnectionPtr&,google::protobuf::Message*);


};