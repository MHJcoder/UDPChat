#include "cn_edu_scnu_libhttpserver_httpserver_UDPChat.h"

int main(){

    sockInfo info=JoinInGroup("192.168.8.4",6666);
    char message[MSG_SIZE];

    //开辟线程接收数据
    int sender_fd=info.sender_fd;
    int buflen=0;
    printf("%d\n",sender_fd);
    printf("Input:\n");
   
    while(1){
        scanf("%s",message);
        if(strcmp(message,"end")==0)break;
        // printf("%d\n",errno);
        // buflen=sendto(sender_fd,message,MSG_SIZE,0,(struct sockaddr* )&multicast_addr,sizeof(multicast_addr));
        buflen=sendmessage(sender_fd,multicast_addr,message);
        printf("sendlength:\t%d\n",buflen);
        if(buflen<0){
            printf("result:\t fail\n");
            printf("error number:\t%d\n",errno);
            printf("reason:\t%s\n", strerror(errno));
        }
    }

    QuitGroup(sender_fd);
    close(sender_fd);
    return 0;
}