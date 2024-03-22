#include"zookeeperutil.h"
#include"mprpcaplication.h"
#include<semaphore.h>
#include<iostream>
//全局的watcher观察器 服务端给客户端的通知
void global_watcher(zhandle_t *zh,int type,int state,const char* path,void* watchcherctx){
    if(type==ZOO_SESSION_EVENT){//回调的消息类型，这些是和会话相关的消息类型

        if(state==ZOO_CONNECTED_STATE){//客户端和服务端连接成功
            sem_t *sem =(sem_t*) zoo_get_context(zh);
            sem_post(sem);
        }
    }
}
ZkClient::ZkClient():m_zhandle(nullptr){

}
ZkClient::~ZkClient(){
    if(m_zhandle!=nullptr){
      zookeeper_close(m_zhandle);//关闭句柄，释放资源
    }
}
void ZkClient::Start(){
    std::string host=MprpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port=MprpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string connstr=host+":"+port;
    /*
      启动了三个线程
      api调用线程
      网络I/O线程（用的是poll）
      watcher回调线程
    */
    /*
       zookeeper_init是用来初始化句柄的函数
    参数：
       host：ZooKeeper服务器的地址，格式为"hostname:port"，多个服务器之间用逗号分隔。
       watcher：一个回调函数指针，用于处理ZooKeeper事件。
       recv_timeout：客户端接收超时时间，单位为毫秒。
       clientid：客户端ID，用于恢复会话。如果为NULL，则表示创建一个新会话。
       
    这个方法的执行是异步的，它会创建两个线程，一直到网络I/O线程收到了ZOO_CONNECTED_STATE事件，才算执行成功
    */
    m_zhandle=zookeeper_init(connstr.c_str(),global_watcher,30000,nullptr,nullptr,0);
    if(m_zhandle==nullptr){
        std::cout<<"zookeeper 初始化错误"<<std::endl;
        exit(EXIT_FAILURE);
    }
    sem_t sem;
    sem_init(&sem,0,0);
    /*
       设置上下文对象，这个上下文对象可以是任何用户自定义的数据结构，用于在回调函数中传递额外的信息或状态。
    */
    zoo_set_context(m_zhandle,&sem);
    //信号量v操作成功才会往下走
    sem_wait(&sem);
    std::cout<<"zookeeper初始化成功"<<std::endl;

    
}
//这些znode节点并不是存在于文件系统中的，而是存在于内存中，因此它们可以存放的数据很小，一个znode只可以存放10m的数据
void ZkClient::Create(const char* path,const char * data,int datalen,int state){
    char path_buffer[128];
    int bufferlen=sizeof(path_buffer);
    int flag;
    flag=zoo_exists(m_zhandle,path,0,nullptr);
    if(flag==ZNONODE){
        //ACL简单来说是znode节点的读写权限
        flag=zoo_create(m_zhandle,path,data,datalen,&ZOO_OPEN_ACL_UNSAFE
        ,state,path_buffer,bufferlen);
        if(flag==ZOK){
            std::cout<<"znode 创建成功 路径："<<path<<std::endl;
        }else{
            std::cout<<"flag:"<<flag<<std::endl;
            std::cout<<"znode 创建失败 路径："<<path<<std::endl;
            exit(-1);
        }
    }
}
std::string ZkClient::GetData(const char *path){
    char buffer[64];
    int bufferlen=sizeof(buffer);
    int flag=zoo_get(m_zhandle,path,0,buffer,&bufferlen,nullptr);
    if(flag!=ZOK){
        std::cout<<"获取znode节点失败 path:"<<path<<std::endl;
        return "";
    }
    else{
        return buffer;
    }
}