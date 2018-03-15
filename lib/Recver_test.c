#include "udpchat.c"
#define MSG_SIZE 6400
int main(){

     char *host="192.168.0.101";
    sockInfo info;
    joinInGroup(host,8888,&info);
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
