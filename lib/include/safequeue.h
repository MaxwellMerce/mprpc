#pragma once
#include<queue>
#include<mutex>
#include<thread>
#include<condition_variable>
//异步写日志日志队列
template<typename T>
class SafeQueue{
public:
   //多个worker线程会写日志
   void push(const T&data){
      std::lock_guard<std::mutex> lock(m_mutex);
      m_queue.push(data);
      m_condition_variable.notify_one();
   }
   //一个线程读日志，写日志文件
   T pop(){
      std::unique_lock<std::mutex> lock(m_mutex);
      while(m_queue.empty()){
         //日志队列为空，线程进入wait状态,b并且释放掉持有的锁
         m_condition_variable.wait(lock);
      }
      T data=m_queue.front();
      m_queue.pop();
      return data;
   }

private:
   std::queue<T> m_queue;
   std::mutex m_mutex;
   std::condition_variable m_condition_variable;
};
