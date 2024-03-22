#include"rpcprovider.h"
#include<string>
#include"mprpcaplication.h"
#include<functional>
#include<google/protobuf/descriptor.h>
#include"rpcheader.pb.h"
#include"zookeeperutil.h"
void RpcProvider::NotifyService(google::protobuf::Service* service){
    ServiceInfo service_info;
    
      //获取了服务对象的描述信息
     const google::protobuf::ServiceDescriptor *pservicedesc=service->GetDescriptor();
     //获取服务的名字
     std::string service_name=pservicedesc->name();
     //获取服务对象service的方法的数量
     int methodscount=pservicedesc->method_count();
     for(int i=0;i<methodscount;i++){
       //获取服务对象下的服务方法的描述
       const google::protobuf::MethodDescriptor* pmethodecrip= pservicedesc->method(i);
       std::string mthodname=pmethodecrip->name();
       service_info.m_methodmap.insert({mthodname,pmethodecrip});

     }
     service_info.m_service=service;

     m_servicemap.insert({service_name,service_info});

}
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn){
     if(!conn->connected()){
        //和rpc 客户端的连接断掉了
        conn->shutdown();
     }
}
/*
在框架内部，客户和服务端必须先协调好protobuf的数据类型
header_size(4个字节)+head_str+args_str

*/
 void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buffer,muduo::Timestamp){
     //网上接受的远程rpc调用请求的字符流
     std::string recvbuf=buffer->retrieveAllAsString();
     
     //从字节流中读取前四个字节的内容，前四个字节记录了参数的长度，用以分割方法名，调用者等避免混淆
     uint32_t header_size=0;
     recvbuf.copy((char*)&header_size,4,0);
     std::string header=recvbuf.substr(4,header_size);
     mprpc::RpcHeader rpcHeader;
     std::string service_name;
     std::string method_name;
     uint32_t args_size;

     if(rpcHeader.ParseFromString(header)){
        //反序列化成功
        service_name=rpcHeader.service_name();
        method_name=rpcHeader.method_name();
        args_size=rpcHeader.args_size();

     }
     else{
       std::cout<<"error occur"<<std::endl;
       return;
     }
     //获取rpc方法参数
     std::string args=recvbuf.substr(4+header_size,args_size);

     //获取service对象和method对象
     auto it=m_servicemap.find(service_name);
     if(it==m_servicemap.end()){
        std::cout<<"服务不存在"<<std::endl;
        return;
     }
    
     auto methodit=it->second.m_methodmap.find(method_name);
     if(methodit==it->second.m_methodmap.end()){
        std::cout<<"方法不存在"<<std::endl;
        return;
     }   

     google::protobuf::Service* service=it->second.m_service; //获取service对象
     const google::protobuf::MethodDescriptor *method=methodit->second;//获取method方法
     
     //生成rpc方法的请求和响应参数
     google::protobuf::Message * request=service->GetRequestPrototype(method).New();
     if(!request->ParseFromString(args)){
       std::cout<<"序列化错误"<<std::endl;
       return;
     }
     google::protobuf::Message * response=service->GetResponsePrototype(method).New();
     //给下面的method方法的调用，绑定一个closure的回调函数
     
     google::protobuf::Closure* done= google::protobuf::NewCallback<RpcProvider,const muduo::net::TcpConnectionPtr&,google::protobuf::Message*>
     (this,&RpcProvider::SendRpcResponse,conn,response);

     //在框架上根据远端rpc请求，调用当前rpc节点上发布的方法
     // new UserService().Login
     //done是一个回调函数，当服务端方法调用完成后，会调用这个done回调函数，并将服务端方法的结果作为参数传递给它。
     service->CallMethod(method,nullptr,request,response,done);
     


}
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn,google::protobuf::Message* response){
     std::string response_str;
     if(response->SerializeToString(&response_str))//将response进行序列化
     {
        //序列化成功后，通过网络把rpc方法执行的结果发送给rpc的调用方
        conn->send(response_str);
     }
     else{
       std::cout<<"结果序列化失败"<<std::endl;

     }
     conn->shutdown();//发出结果后就断开连接



}
void RpcProvider::Run(){
    std::string ip=MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint32_t port=atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip,port);

    //创建Tcpserver对象
    muduo::net::TcpServer server(&m_eventLoop,address,"RpcProvider");
    //绑定连接回调和消息回调方法,分离了网络代码与业务代码
    //有连接建立时调用
    //将对象指针于函数绑定意味着以后可以在这个函数中访问这个对象的成员和函数;
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection,this,std::placeholders::_1));
    //有消息到达时调用
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));

    
    //设置muduo库的线程数量
    server.setThreadNum(4);
    
    //把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
    ZkClient zkChil;
    zkChil.Start();
    //service_name是永久性节点，method_path为临时性节点
    for(auto &sp:m_servicemap){
      // /service_name
       std::string service_path="/"+sp.first;
       zkChil.Create(service_path.c_str(),nullptr,0);
       for(auto &mp:sp.second.m_methodmap){
          // /service_name/method_name
          std::string method_path=service_path+'/'+mp.first;
          char method_path_data[128]={0};
          sprintf(method_path_data,"%s:%d",ip.c_str(),port);
          zkChil.Create(method_path.c_str(),method_path_data,strlen(method_path_data),ZOO_EPHEMERAL);
       }
    }

    std::cout<<"rpc 服务端准备就绪 ip:"<<ip<<"端口: "<<port<<std::endl;




    //启动网络服务
    server.start();
    m_eventLoop.loop();

}

