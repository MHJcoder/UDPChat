// #include "udpchat.h"
#include "cn_edu_scnu_libhttpserver_httpserver_UDPChat.h"
#include "cn_edu_scnu_libhttpserver_httpserver_UDPChat_sockInfo.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
// #include "ifaddrs.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <iostream>
using namespace std;


#define MSG_SIZE 6400

#define MCAST_ADDR "224.1.1.85"
typedef struct sockInfo{
    int sender_fd;
    int recver_fd;
    int port;
}sockInfo;


char * JstringTocharArr(jcharArray arr){
    
}
//转char*为string
jstring stoJstring(JNIEnv* env, const char* pat)  
{  
jclass strClass = env->FindClass("Ljava/lang/String;");  
jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");  
jbyteArray bytes = env->NewByteArray(strlen(pat));  
env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);  
jstring encoding = env->NewStringUTF("utf-8");  
return (jstring)env->NewObject(strClass, ctorID, bytes, encoding);  //局部引用
}   


JNIEXPORT void JNICALL Java_cn_edu_scnu_libhttpserver_httpserver_UDPChat_joinInGroup
(JNIEnv * env, jclass cls, jstring IP, jint port, jobject info){
    //获取类对象句柄
    printf("joinInGroup\n");
    jclass JInfoClass= env->GetObjectClass(info);
    //获取类对象属性的ID
    //GetFieldID需要3个参数。第一个是上一步获取的Jclass
    //第二个参数是Java中的变量名，最后一个参数是变量签名（int 的变量签名是”I“)
    jfieldID sfd = env->GetFieldID(JInfoClass,"sender_fd","I");
    jfieldID rfd = env->GetFieldID(JInfoClass,"recver_fd","I");
    jfieldID pid = env->GetFieldID(JInfoClass,"port","I");
    // (const char *) host = env->GetStringUTFChars(IP,0);
    const char * host = env->GetStringUTFChars(IP,0);
    int sender_fd = socket(AF_INET, SOCK_DGRAM, 0);
    int recver_fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in multicast_addr,local_addr;
    struct ip_mreq mreq; 
    int buflen = 0;
    if(sender_fd == -1){
            printf("socket\n");
            exit(1);
    }
    if(recver_fd == -1){
        printf("socket\n");
        exit(1);
    }
    //设置主机端口可重用
    int reused = 1;    //C中没有bool
    setsockopt(recver_fd , SOL_SOCKET, SO_REUSEPORT ,(const void *)&reused , sizeof(reused));
    //設置超時時間
    unsigned char ttl=255;
    setsockopt(recver_fd , IPPROTO_IP, IP_MULTICAST_TTL , &ttl , sizeof(ttl));
    //设置本地环回
    int loop = 1;
	if(setsockopt(recver_fd,IPPROTO_IP,IP_MULTICAST_LOOP,&loop,sizeof(loop))){
		printf("set:IP_MULTICAST_LOOP");
		exit(errno);
	}
     //指定组播数据的出口网卡
    struct in_addr addr;
    addr.s_addr=inet_addr(host);
    setsockopt(recver_fd , IPPROTO_IP, IP_MULTICAST_IF,&addr,sizeof(addr));
    //指定目的多播地址
    memset(&multicast_addr,0,sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_addr.s_addr = inet_addr(MCAST_ADDR);//@&
    multicast_addr.sin_port = htons(port);
    // printf("%d",multicast_addr.sin_port);

    //bind绑定socket到网络接口,指定数据接收地址
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = inet_addr(host);
    local_addr.sin_port = htons(port);
    // if(bind(sender_fd,(struct sockaddr*)&local_addr,sizeof(local_addr))<0){
    //     printf("bind");
    //     exit(1);
    // }
    if(bind(recver_fd,(struct sockaddr*)&multicast_addr,sizeof(multicast_addr))<0){
        printf("bind");
        exit(1);
    }

    //设置加入的组播地址和加入组播的本地网络接口
    memset(&mreq,0,sizeof(mreq));
    mreq.imr_multiaddr.s_addr=inet_addr(MCAST_ADDR);//组播地址
    mreq.imr_interface.s_addr=inet_addr(host);//网络接口默认
    //加入组播
    if(setsockopt(recver_fd , IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq))<0)
    {
     printf("set:IP_ADD_MEMBERSHIP");
     exit(errno);
    } 
    env->SetIntField(info,sfd,sender_fd);
    env->SetIntField(info,rfd,recver_fd);
    env->SetIntField(info,pid,port);
    env->ReleaseStringUTFChars( IP, host);
    // return info;
}

JNIEXPORT jint JNICALL Java_cn_edu_scnu_libhttpserver_httpserver_UDPChat_sendMessage
(JNIEnv * env, jclass cls, jint socket_fd, jint port, jstring msg){
    int buflen=0;
    const char *message = env->GetStringUTFChars(msg, 0);
    struct sockaddr_in multicast_addr;
    memset(&multicast_addr,0,sizeof(multicast_addr));
    multicast_addr.sin_family=AF_INET;
    multicast_addr.sin_addr.s_addr=inet_addr(MCAST_ADDR);
    multicast_addr.sin_port=htons(port);
    buflen=sendto(socket_fd,message,MSG_SIZE,0,(struct sockaddr* )&multicast_addr,sizeof(multicast_addr));
    env->ReleaseStringUTFChars( msg, message);
    return buflen;
}

JNIEXPORT jint JNICALL Java_cn_edu_scnu_libhttpserver_httpserver_UDPChat_recvMessage
(JNIEnv * env, jclass cls, jint socket_fd,jstring msg){
     // char buffer[MSG_SIZE];
     int buflen=0;
     char message[MSG_SIZE];
    //  const char * message = env->GetStringUTFChars(msg, 0);
     struct sockaddr_in addr;
     socklen_t addrlen=sizeof(addr);
     buflen=recvfrom(socket_fd,message,MSG_SIZE-1,0,(struct sockaddr*)&addr,&addrlen);
    //  msg=stoJstring(env,message);
     msg=env->NewStringUTF(message);
    //  env->ReleaseStringUTFChars( msg, message);
    return buflen;
}
JNIEXPORT void JNICALL Java_cn_edu_scnu_libhttpserver_httpserver_UDPChat_quitGroup
(JNIEnv * env, jclass cls, jint sender_fd,jint recver_fd, jstring IP){
    struct ip_mreq mreq;
    // char host[NI_MAXHOST];
    // findip(host);
    const char *host = env->GetStringUTFChars(IP, 0);
    memset(&mreq,0,sizeof(mreq));
    mreq.imr_multiaddr.s_addr=inet_addr(MCAST_ADDR);//组播地址
    mreq.imr_interface.s_addr=inet_addr(host);//网络接口默认
    setsockopt(sender_fd,IPPROTO_IP, IP_DROP_MEMBERSHIP,&mreq, sizeof(mreq));
    close(sender_fd);
    close(recver_fd);
    env->ReleaseStringUTFChars( IP, host);
}


