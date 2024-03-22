#include"mprpcchannel.h"
#include<string>
#include"rpcheader.pb.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<error.h>
#include"mprpcaplication.h"
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include"mprpccontroller.h"
#include"zookeeperutil.h"
void Mprpcchannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done)
{
   
   
   const google::protobuf::ServiceDescriptor* sd= method->service();
   std::string service_name=sd->name();
   std::string method_name=method->name();
   //获取参数的序列化字符串 args_str
   uint32_t arg_size=0;
   std::string arg_str;
   if(!request->SerializeToString(&arg_str)){
       
       controller->SetFailed("请求序列化失败");
       return;
   }else{
       arg_size=arg_str.size();

   }
   //定义rpc的请求header
   mprpc::RpcHeader rpcHeader;
   rpcHeader.set_service_name(service_name);
   rpcHeader.set_args_size(arg_size);
   rpcHeader.set_method_name(method_name);

   uint32_t header_size=0;
   std::string rpc_header_str;
   if(rpcHeader.SerializeToString(&rpc_header_str)){
         header_size=rpc_header_str.size();
   }else{
       controller->SetFailed("rpcheader序列化失败");
       return;
   }
   //组织待发送的rpc请求字符串
   std::string send_rpc_str;
   send_rpc_str.insert(0,std::string((char *)&header_size,4));
   send_rpc_str+=rpc_header_str;
   send_rpc_str+=arg_str;

   //使用Socket编程，完成rpc方法的远程调用
   int client=socket(AF_INET,SOCK_STREAM,0);
   if(client==-1){
      char errtext[200]={0};
      sprintf(errtext,"scocket失败,错误号%d",errno);
      controller->SetFailed(errtext);
      return;
   }


   //std::string ip=MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
   //uint32_t port=atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
   //在znode节点上查询服务的信息
   ZkClient zkCil;
   zkCil.Start();
   std::string method_path="/"+service_name+"/"+method_name;
   std::string host_data=zkCil.GetData(method_path.c_str());
   if(host_data==""){
      controller->SetFailed(method_path+"不存在");
      return;
   }
   int idx=host_data.find(":");
   if(idx==-1){
      controller->SetFailed(method_path+"地址不合法");
      return;
   }
   std::string ip=host_data.substr(0,idx);
   uint16_t port=atoi(host_data.substr(idx+1,host_data.size()-idx).c_str());

   struct sockaddr_in server_addr;
   server_addr.sin_family=AF_INET;
   server_addr.sin_port=htons(port);
   server_addr.sin_addr.s_addr=inet_addr(ip.c_str());
   //连接
   if(connect(client,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1){
     
      close(client);
      char errtext[200]={0};
      sprintf(errtext,"scocket连接失败,错误号%d",errno);
      controller->SetFailed(errtext);
      return;
   }

   if(send(client,send_rpc_str.c_str(),send_rpc_str.size(),0)==-1){
      char errtext[200]={0};
      sprintf(errtext,"socket发送失败,错误号%d",errno);
      controller->SetFailed(errtext);
      close(client);
      return;
   }
   //接收rpc请求的响应
   char recvbuf[1024]={0};
   int recvsize=recv(client,recvbuf,1024,0);

   if(recvsize==-1){
      char errtext[200]={0};
      sprintf(errtext,"数据接收失败,错误号%d",errno);
      controller->SetFailed(errtext);
      close(client);
      return;
   }
   
   if(response->ParseFromArray(recvbuf,recvsize)){
    
   }else{
     controller->SetFailed("接收数据反序列化失败");
     close(client);
     return;
   }


}