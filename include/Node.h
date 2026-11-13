#pragma once
#include <string>

class Node{
    private:
        std::string name_;

    protected:
        bool inited_{false};//用来判断子类是否初始化日志，main函数里不可以改，子类里需要把布尔型变量inited_变成true,代表成功初始化了，写法还是ATT：列表初始化！！！

    public:
        Node(const std::string& name);

        const std::string& getName()const;


        virtual void init()=0;
        virtual void shutdown()=0;
        virtual ~Node()=default;

};
