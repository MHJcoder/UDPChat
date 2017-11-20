#include "udpchat.h"

int main(){

    sockInfo info=JoinInGroup();
    char message[MSG_SIZE];

    //开辟线程接收数据
    int recver_fd=info.recver_fd;
    int buflen=0;
    struct sockaddr_in multicast_addr=info.addr;
    socklen_t addrlen=sizeof(multicast_addr);
    printf("%d\n",recver_fd);
    // printf("%s",inet_ntoa(multicast_addr.sin_addr));
    printf("receive message:\n");

    while(1){
        buflen=recvfrom(recver_fd,message,MSG_SIZE-1,0,(struct sockaddr*)&multicast_addr,&addrlen);
        buflen=recvmessage(recver_fd,message);
                if(buflen<0){
                    break;
                }
                printf("content：\n");
                printf("%s\n",message);
    }
    QuitGroup(recver_fd);
    close(recver_fd);
    return 0;
   
}
