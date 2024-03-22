#pragma once
#include"safequeue.h"
#include<string>
#include<iostream>
//Mprpc框架提供的日志系统
//日志级别
enum loglever{
  INFO,//正常信息
  ERROR,//错误信息
};
class Logger{
public:
    //设置日志级别
    void SetLogeLerver(loglever lever);
    //写日志,把日志写入缓冲队列中
    void Log(std::string msg);
    //获取日志单例
    static Logger& GetInstance();
private:
    int m_loglever;//记录日志级别
    SafeQueue<std::string> m_safequeue;//日志缓冲队列
    Logger();
    Logger(const Logger*)=delete;
    Logger(Logger&&)=delete;
};

//定义宏
#define LOG_INFO(logmesgformat,...)\
      do\
      {\
        Logger &logger=Logger::GetInstance();\
        logger.SetLogeLerver(INFO);\
        char c[1024]={0};\
        snprintf(c,1024,logmesgformat,##__VA_ARGS__);\
        logger.Log(c);\
      } while (0);

#define LOG_ERR(logmesgformat,...)\
      do\
      {\
        Logger &logger=Logger::GetInstance();\
        logger.SetLogeLerver(ERROR);\
        char c[1024]={0};\
        snprintf(c,1024,logmesgformat,##__VA_ARGS__);\
        logger.Log(c);\
      } while (0);