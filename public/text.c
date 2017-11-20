#include"sender.h"
#include<stdio.h>
#define MSG_SIZE 64000
int main(){

    printf("start\n");
    int sender_fd=socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in multicast_addr,local_addr;
    struct ip_mreq mreq;
    int strlen=0;
    char message[MSG_SIZE];
    if(sender_fd ==-1){
            perror("socket\n");
            exit(1);
    }
    //设置主机端口可重用
    int reused = 1;    //C中没有bool
    setsockopt(sender_fd , SOL_SOCKET, SO_REUSEPORT ,(const void *)&reused , sizeof(reused));
    unsigned char ttl=255;
    //设置多播数据包超时时间
    setsockopt(sender_fd , IPPROTO_IP, IP_MULTICAST_TTL , &ttl , sizeof(ttl));
    //设置多播收发数据的网络接口
    // struct in_addr addr;
    // addr.sin_addr.s_addr=inet_addr("192.168.8.247");
    // setsockopt(sender_fd , IPPROTO_IP, IP_MULTICAST_IF,&addr,sizeof(addr));
    
    //设置发送消息的目的多播地址
    memset(&multicast_addr,0,sizeof(multicast_addr));
    multicast_addr.sin_family=AF_INET;
    multicast_addr.sin_addr.s_addr=inet_addr(MCAST_ADDR);
    multicast_addr.sin_port=htons(MCAST_PORT);
    
    //bind绑定socket到网络接口
    local_addr.sin_family=AF_INET;
    local_addr.sin_addr.s_addr=inet_addr("192.168.8.247");
    local_addr.sin_port=htons(MCAST_PORT);
    if(bind(sender_fd,(struct sockaddr*)&local_addr,sizeof(local_addr))<0){
        perror("bind");
        exit(1);
    }
    //设置加入的组播地址和加入组播的本地网络接口
    
    memset(&mreq,0,sizeof(mreq));
    mreq.imr_multiaddr.s_addr=inet_addr(MCAST_ADDR);//组播地址
    mreq.imr_interface.s_addr=htonl(INADDR_ANY);//网络接口默认
    
    if(setsockopt(sender_fd , IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq))<0)
    {
     perror("set:IP_ADD_MEMBERSHIP");
     exit(errno);
    }
    
    printf("Input:\n");
    while(1){
        scanf("%s",message);
        if(strcmp(message,"end")==0)break;
        // printf("%d",errno);
        strlen=sendto(sender_fd,message,MSG_SIZE,0,(struct sockaddr* )&multicast_addr,sizeof(multicast_addr));
        if(strlen<0){
            printf("%d",errno);
            printf("fail\n");
            break;
        }
    }

    close(sender_fd);
    return 0;
}

int recvmessage(int fd){
    int strlen=0;

    return strlen;
}
