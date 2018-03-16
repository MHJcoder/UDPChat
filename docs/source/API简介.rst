API使用说明
=========

socket信息结构体:
::

    typedef struct sockInfo{
        int sender_fd;
        int recver_fd;
        int port;
    }sockInfo


作用:
    sender_fd和recver_fd存放发送和接收的socket描述符，port存放自定义的局域网统一通信端口

函数
======

（1）加入聊天
::

    void  joinInGroup(char *host,int port,sockInfo * Info)

参数:host，本地主机的局域网IP地址  port，局域网统一通信端口  Info,socket信息结构体

（2）发送消息
::

    ssize_t sendMessage(int fd,int port,char *message)

参数:fd,发送数据的socket描述符  port，局域网统一通信端口  message，发送的消息

（3）接收消息
::

    ssize_t recvMessage(int fd,char*message)

参数:fd,接收数据的socket描述符  port，局域网统一通信端口  message，接收消息的缓存区

（4）退出聊天,关闭socket
::

    void quitGroup(int sender_fd,int recver_fd,char*host)

参数:sender_fd,发送数据的socket描述符  recver_fd,接收数据的socket描述符  host，本地主机的局域网IP地址 