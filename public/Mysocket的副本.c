//
//  Mysocket.c
//  SocDisplayer
//
//  Created by MHJ on 2017/8/1.
//  Copyright © 2017年 MHJ. All rights reserved.
//

#include "Mysocket.h"
void TCPServerAcceptCallBack(CFSocketRef socket,
                                       CFSocketCallBackType type,
                                       CFDataRef address,
                                       const void *data,
                                       void *info)
{
    //    SocketViewController * vc =(__bridge SocketViewController*)(info);
    //判断回调事件类型为接入回调
    if(kCFSocketAcceptCallBack ==type){
        //此时data是CFSocketNativeHandle类型，获取本地Socket的Handle
        CFSocketNativeHandle nativeHanldle = *(CFSocketNativeHandle*)data;//*去引用
        //定义一个数组接受data转成的Socket地址，（可能有多个），SOCK_MAXADDRLEN表示最多的可能的地址
        uint8_t name[SOCK_MAXADDRLEN];
        socklen_t namelen = sizeof(name);
        if(getpeername(nativeHanldle, (struct sockaddr * )name , &namelen)!=0){
            perror("getpeername:");
            exit(1);
        }
        struct sockaddr_in * addr_in = (struct sockaddr_in*)name;
        //inet_ntoa将网络地址转换成点分十进制格式
        
        printf("%s:%d连接进来了",inet_ntoa(addr_in->sin_addr),addr_in->sin_port);
       
        CFStreamCreatePairWithSocket(kCFAllocatorDefault,//内存分配器
                                     nativeHanldle,
                                     &s_readStreamRef,
                                     &s_writeStreamRef);
        // ----CFStreamCreatePairWithSocket(）操作成功后，readStreamRef和writeStreamRef都指向有效的地址，因此判断是不是还是之前设置的NULL就可以了
        if(s_readStreamRef&&s_writeStreamRef){
            //使用NSStream
            printf("%s","输入输出流建立成功");
        }
        else{
            close(nativeHanldle);
        }
        
        
    }
    
}
//服务端建立socket
CFSocketRef createServerSocket(UInt16 port){
    CFSocketRef _socket;
    bool success = false;
    //创建socket
    _socket = CFSocketCreate(kCFAllocatorDefault,PF_INET, SOCK_STREAM, IPPROTO_TCP, kCFSocketAcceptCallBack,TCPServerAcceptCallBack,NULL);
    success=(_socket==NULL);
    if(!success){
        printf("%s","创建失败");
    }
    else{
        printf("%s","创建成功");
        bool reused = true;
        setsockopt(CFSocketGetNative(_socket), SOL_SOCKET, SO_REUSEADDR, (const void *)&reused, sizeof(reused));
        struct sockaddr_in Socaddr;
        memset(&Socaddr, 0, sizeof(Socaddr));
        Socaddr.sin_len=sizeof(Socaddr);
        Socaddr.sin_family=AF_INET;
        Socaddr.sin_addr.s_addr=INADDR_ANY;
        Socaddr.sin_port=CFSwapInt16(port);
        CFDataRef dataaddr=CFDataCreate(kCFAllocatorDefault, (UInt8*)&Socaddr, sizeof(Socaddr));
        CFSocketError err=CFSocketSetAddress(_socket, dataaddr);
        CFRelease(dataaddr);
        success=(err==kCFSocketSuccess);
    }
    if(!success){
        printf("%s","绑定失败");
        CFRelease(_socket);
        _socket=NULL;
    }
    else{
        //        bool isvalid=CFSocketIsValid(_socket);
        printf("%s","绑定成功");
        
        printf("%s","----启动循环监听客户端连接---");
        //获取当前线程的runloop
        CFRunLoopRef runLoopRef = CFRunLoopGetCurrent();
        CFRunLoopSourceRef sourceRef = CFSocketCreateRunLoopSource(kCFAllocatorDefault, _socket, 0);
        CFRunLoopAddSource(runLoopRef, sourceRef, kCFRunLoopCommonModes);
        CFRelease(sourceRef);
        CFRunLoopRun();
    }
    if(!success){
        return NULL;
    }
    else{
        return _socket;
    }  
}
//客户端连接socket
void connectToHost(CFStringRef IP,UInt16 port){
    
    CFStreamCreatePairWithSocketToHost(NULL,IP, 12435, &c_readStreamRef, &c_writeStreamRef);
}
//UDP
void createUDPSocket(){
    
}

CFWriteStreamRef getServerInputStream(){
    return s_writeStreamRef;
}
CFReadStreamRef getServerOutputStream(){
    return s_readStreamRef;
}
CFWriteStreamRef getClientInputStream(){
    return c_writeStreamRef;
}
CFReadStreamRef getClientOutputStream(){
    return c_readStreamRef;
}
