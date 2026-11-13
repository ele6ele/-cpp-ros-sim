#include <string>
#include <ctime>
#include "Exception.h"
#include "Message.h"

Message::Message(const std::string& content){
    if(content.empty()) throw MessageException("消息内容不可为空");//空消息报错  用invalid也可
    content_ = content;//ATT:不可以变成成员函数，不然创造、调用对象会可能出现半成品，还需要考虑非空的情况
    timestamp_=now();
}

std::string Message::now(){
    time_t T = time(nullptr);//获取秒数值放在T里
    tm timeform{}; //列表初始化timeform
    localtime_s(&timeform,& T);//转换成年月日时分秒，获得指向它的指针timeform

    char buf[64];//限定范围
    strftime (buf,sizeof buf,"%Y-%m-%d %H:%M:%S",&timeform);//把timeform里的时间变成规定格式
    return buf;
}//用静态保证不隶属于对象

const std::string& Message::getContent() const{
    return content_;
}//这是一个getter函数，不然不可见
//返回常量引用避免拷贝构造！const*2:保证封装性、表示这是常函数

const std::string& Message::getTimestamp() const{
    return timestamp_;
}



