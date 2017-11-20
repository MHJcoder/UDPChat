#ifndef udpchat_h
#define udpchat_h

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
#define MSG_SIZE 6400
// #define MCAST_PORT 3485
#define MCAST_ADDR "224.1.1.85"


typedef struct sockInfo{
    int sender_fd;
    int recver_fd;
    int port;
}sockInfo;

// void findip(char*host);
sockInfo  joinInGroup(char *host,int port);
int sendMessage(int fd,int port,char *message);
int recvMessage(int fd,char*message);
void quitGroup(int sender_fd,int recver_fd,char*host);

#endif
