// #include "udpchat.h"


#include "cn_edu_scnu_libhttpserver_httpserver_UDPChat.h"


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


JNIEXPORT jobject JNICALL Java_cn_edu_scnu_libhttpserver_httpserver_UDPChat_joinInGroup
(JNIEnv * env, jclass cls, jstring IP, jint port, jobject info){
    //获取类对象句柄
    jclass JInfoClass= env->GetObjectClass(info);
    //获取类对象属性
    jfieldID sfd = env->GetFieldID(JInfoClass,"sender_fd","Lcn/edu/scnu/libhttpserver/httpserver/UDPChat/sockInfo");
    jfieldID rfd = env->GetFieldID(JInfoClass,"recver_fd","Lcn/edu/scnu/libhttpserver/httpserver/UDPChat/sockInfo");
    jfieldID pid = env->GetFieldID(JInfoClass,"port","Lcn/edu/scnu/libhttpserver/httpserver/UDPChat/sockInfo");
    // (const char *) host = env->GetStringUTFChars(IP,0);
    const char *host =env->GetStringUTFChars(IP,0);
    printf("joinInGroup\n");
    int sender_fd=socket(AF_INET, SOCK_DGRAM, 0);
    int recver_fd=socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in multicast_addr,local_addr;
    struct ip_mreq mreq; 
    int buflen=0;
    if(sender_fd ==-1){
            perror("socket\n");
            exit(1);
    }
    if(recver_fd ==-1){
        perror("socket\n");
        exit(1);
    }
    //设置主机端口可重用
    int reused = 1;    //C中没有bool
    setsockopt(recver_fd , SOL_SOCKET, SO_REUSEPORT ,(const void *)&reused , sizeof(reused));
    //設置超時時間
    unsigned char ttl=255;
    setsockopt(recver_fd , IPPROTO_IP, IP_MULTICAST_TTL , &ttl , sizeof(ttl));
    //设置本地环回
    int loop=1;
	if(setsockopt(recver_fd,IPPROTO_IP,IP_MULTICAST_LOOP,&loop,sizeof(loop))){
		perror("set:IP_MULTICAST_LOOP");
		exit(errno);
	}

     //指定组播数据的出口网卡
    struct in_addr addr;

    addr.s_addr=inet_addr(host);
    setsockopt(recver_fd , IPPROTO_IP, IP_MULTICAST_IF,&addr,sizeof(addr));

    //指定目的多播地址
    memset(&multicast_addr,0,sizeof(multicast_addr));
    multicast_addr.sin_family=AF_INET;
    multicast_addr.sin_addr.s_addr=inet_addr(MCAST_ADDR);//@&
    multicast_addr.sin_port=htons(port);
    // printf("%d",multicast_addr.sin_port);

    //bind绑定socket到网络接口,指定数据接收地址
    local_addr.sin_family=AF_INET;
    local_addr.sin_addr.s_addr=inet_addr(host);
    local_addr.sin_port=htons(port);
    // if(bind(sender_fd,(struct sockaddr*)&local_addr,sizeof(local_addr))<0){
    //     perror("bind");
    //     exit(1);
    // }
    if(bind(recver_fd,(struct sockaddr*)&multicast_addr,sizeof(multicast_addr))<0){
        perror("bind");
        exit(1);
    }

    //设置加入的组播地址和加入组播的本地网络接口
    memset(&mreq,0,sizeof(mreq));
    mreq.imr_multiaddr.s_addr=inet_addr(MCAST_ADDR);//组播地址
    mreq.imr_interface.s_addr=inet_addr(host);//网络接口默认
    //加入组播
    if(setsockopt(recver_fd , IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq))<0)
    {
     perror("set:IP_ADD_MEMBERSHIP");
     exit(errno);
    }
    env->ReleaseStringUTFChars( IP, host);
    env->SetIntField(info,sfd,sender_fd);
    env->SetIntField(info,rfd,recver_fd);
    env->SetIntField(info,pid,port);
    return info;
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
    // printf("%d\n",buflen);
    // if(buflen<0){
    //     printf("fail\n");
    //     printf("%d",errno);
    //     printf("%s", strerror(errno));
    // }
    env->ReleaseStringUTFChars( msg, message);
    return buflen;
}

JNIEXPORT jstring JNICALL Java_cn_edu_scnu_libhttpserver_httpserver_UDPChat_recvMessage
(JNIEnv * env, jclass cls, jint socket_fd){
     // char buffer[MSG_SIZE];
     int buflen=0;
     char message[MSG_SIZE];
    //  const char * message = env->GetStringUTFChars(msg, 0);
     struct sockaddr_in addr;
     socklen_t addrlen=sizeof(addr);
     buflen=recvfrom(socket_fd,message,MSG_SIZE-1,0,(struct sockaddr*)&addr,&addrlen);
    //  msg=stoJstring(env,message);
     jstring msg=jstring(message);
    //  env->ReleaseStringUTFChars( msg, message);
     return msg;
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


