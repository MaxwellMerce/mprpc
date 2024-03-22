#include "logger.h"
#include<time.h>
void Logger::SetLogeLerver(loglever lever){
     m_loglever=lever;
}
void Logger::Log(std::string msg){
    m_safequeue.push(msg);
}

Logger& Logger::GetInstance(){
   /**
     * 局部静态特性的方式实现单例模式。
     * 静态局部变量只在当前函数内有效，其他函数无法访问。
     * 静态局部变量只在第一次被调用的时候初始化，也存储在静态存储区，生命周期从第一次被初始化起至程序结束止。
     * 这种写法是线程安全的
   */
   static Logger logger;
   return logger;
}
Logger::Logger(){
    //启动专门的写日志线程
    std::thread writeLogTask([&](){
        while(true){
            //获取时间，写入相应的日志文件当中
            time_t now=time(nullptr);
            tm* nowtm=localtime(&now);
            char filename[128];
            sprintf(filename,"%d-%d-%d-log.txt",nowtm->tm_year+1900,nowtm->tm_mon+1,nowtm->tm_mday);
            FILE *pf=fopen(filename,"a+");
            if(pf==nullptr){
                std::cout<<"日志文件打开失败"<<std::endl;
                exit(-1);
            }
            std::string msg=m_safequeue.pop();
            char time_buf[128]={0};
            sprintf(time_buf,"%d:%d:%d =>[%s]",nowtm->tm_hour,nowtm->tm_min,nowtm->tm_sec,(m_loglever==INFO?"info":"error"));
            msg.insert(0,time_buf);
            msg.append("\n");
            fputs(msg.c_str(),pf);
            fclose(pf);
        }
        
    });
    //设置分离线程
    writeLogTask.detach();
}
