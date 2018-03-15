#ifndef udpchat_h
#define udpchat_h
#include <sys/types.h>


typedef struct sockInfo{
    int sender_fd;
    int recver_fd;
    int port;
}sockInfo;

// void findip(char*host);
void  joinInGroup(char *host,int port,sockInfo * Info);
ssize_t sendMessage(int fd,int port,char *message);
ssize_t recvMessage(int fd,char*message);
void quitGroup(int sender_fd,int recver_fd,char*host);

#endif
