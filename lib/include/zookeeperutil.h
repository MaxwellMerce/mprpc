#pragma once

#include<semaphore.h>
#include<zookeeper/zookeeper.h>
#include<string>
//封装的zk客户端类
class ZkClient{
public:
    ZkClient();
    ~ZkClient();
    //启动连接zkserver
    void Start();
    //在zkserver上根据指定的path创建znode节点
    void Create(const char* path,const char * data,int datalen,int state=0);
    //根据参数指定的znode节点路径，获取znode的值
    std::string GetData(const char *path);
private:
    //zk的客户端句柄,用来描述和管理与zookeeper服务器的连接
    zhandle_t *m_zhandle;
};