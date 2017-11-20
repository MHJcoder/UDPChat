#include "udpchat.h"


sockInfo  joinInGroup(char *host,int port){
    printf("JoinInGroup\n");
    int sender_fd=socket(AF_INET, SOCK_DGRAM, 0);
    int recver_fd=socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in multicast_addr,local_addr;
    struct ip_mreq mreq;
    int buflen=0;
    sockInfo Info;
    pthread_t pid;
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
    
    Info.recver_fd=recver_fd;
    Info.sender_fd=sender_fd;
    Info.port=port;
    return Info;
}



int sendMessage(int socket_fd,int port,char *message){
    int buflen=0;
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
    return buflen;
}


//接收消息
int recvMessage(int socket_fd,char*message){
    // char buffer[MSG_SIZE];
    int buflen=0;
    struct sockaddr_in addr;
    socklen_t addrlen=sizeof(addr);
    buflen=recvfrom(socket_fd,message,MSG_SIZE-1,0,(struct sockaddr*)&addr,&addrlen);

    return buflen;
}


void quitGroup(int sender_fd,int recver_fd,char*host){
    struct ip_mreq mreq;
    // char host[NI_MAXHOST];
    // findip(host);
    //退出组播
    memset(&mreq,0,sizeof(mreq));
    mreq.imr_multiaddr.s_addr=inet_addr(MCAST_ADDR);//组播地址
    mreq.imr_interface.s_addr=inet_addr(host);//网络接口默认
    setsockopt(sender_fd,IPPROTO_IP, IP_DROP_MEMBERSHIP,&mreq, sizeof(mreq));
    close(sender_fd);
    close(recver_fd);
}

