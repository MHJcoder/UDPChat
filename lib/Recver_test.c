#include "udpchat.h"

int main(){

     char *host="192.168.8.4";
    sockInfo info=joinInGroup("192.168.8.4",6666);
    char message[MSG_SIZE];

    //开辟线程接收数据
     int sender_fd=info.sender_fd;
    int recver_fd=info.recver_fd;
    int buflen=0;
    
    // printf("%d\n",recver_fd);
    // printf("%s",inet_ntoa(multicast_addr.sin_addr));
    // printf("receive message:\n");

    while(1){
        // buflen=recvfrom(recver_fd,message,MSG_SIZE-1,0,(struct sockaddr*)&multicast_addr,&addrlen);
        buflen=recvMessage(recver_fd,message);
                if(buflen<0){
                    break;
                }
                printf("content：\n");
                printf("%s\n",message);
    }
    quitGroup(sender_fd,recver_fd,host);
    close(recver_fd);
    return 0;
   
}
