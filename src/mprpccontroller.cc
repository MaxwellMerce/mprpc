#include "mprpccontroller.h"
Mprpccontroller::Mprpccontroller()
{
    m_failed = false;
    m_errText = "";
}
void Mprpccontroller::Reset()
{
    m_failed = false;
    m_errText = "";
}
bool Mprpccontroller::Failed() const
{
    return m_failed;
}
std::string Mprpccontroller::ErrorText() const
{
    return m_errText;
}
void Mprpccontroller::SetFailed(const std::string &reason)
{
    m_failed = true;
    m_errText = reason;
}
// 目前未实现的功能
void Mprpccontroller::StartCancel(){};
bool Mprpccontroller::IsCanceled() const { return false; }
void Mprpccontroller::NotifyOnCancel(google::protobuf::Closure *callback) {}
