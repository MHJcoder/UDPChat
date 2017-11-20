//
//  socket.c
//  SOC1
//
//  Created by MHJ on 2017/8/24.
//  Copyright © 2017年 MHJ. All rights reserved.
//

#include <stdlib.h>
#include "socket.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <map>

#define FILEPORT 6201
#define MCAST_ADDR "224.1.1.88"

//文件名和包大小主客户端一致
#define FileNameLen 1024
#define BufferSize 1024
int server_fd;

//struct in_addr addr = {0};
typedef struct {
    long id;
    long buf_size;
    
}PackInfo;
typedef struct{
    PackInfo head;
    char buffer[BufferSize];//1K
}Pack;
long  sender(uint8_t* data,long length){
    long nbytes=0;
    return nbytes;
}

/*
 *初始化map
 */
void initMap(map<char* ,long> Map){
    FILE*nfp=fopen("./peername.txt","r");
    FILE*idfp=fopen("./packid.txt", "r");
	while(!feof(nfp)||!feof(idfp)){
		char peer[1024];
		fgets(peer, 1024, nfp);
		long id=-1;
		fscanf(idfp,"%ld",&id);
		Map.insert(pair<char*,long>(peer,id));
	}
    fclose(nfp);
	fclose(idfp);
	
}


/*
 *开辟新的线程给接入的客户端
 */
void accept_request(int clientfd){
	 char filename[FileNameLen];
     bzero(filename, sizeof(filename));
        
     //读取客户端请求的文件名
	 long  flag =read(clientfd, filename, sizeof(filename));
	 if (flag==-1) {
            perror("recv name fail");
            exit(4);
        }
        else if (flag==0){
            printf("未收到请求的文件名");
            continue;
        }
		else{
			            //打印请求的文件名
            printf("%s",filename);
            Pack data;
            long pack_id=0;
            FILE*fp=fopen(filename, "rb");
            long hlen=0;
            
            
            
            //到达文件尾部停止读取
            while(!feof(fp)){
                
               
                //对应包ID,从1开始计数
                pack_id++;
                data.head.id=pack_id;
                //fread返回实际读取到的字节数
                long  len=fread(data.buffer, sizeof(char), BufferSize, fp);
                data.head.buf_size=len;
                hlen=sizeof(data.head);
                if(len>BufferSize){
                    printf("读取文件出错\n");
                    break;
                }
                else if(len==0){
                    printf("到达文件尾部\n");
                    //因为文件传输成功，已到达末尾，删除记录的文件传输信息
                    break;
                }
                else{
                    
                    //发出包的字节数
                    long wlen=write(clientfd, &data, sizeof(data));
                    //比较包长度是否正确
                    if(wlen!=hlen+len){
                        perror("发送出错");
                        //重新发，待定
                    }
                    else{
                        printf("第%ld个包发送成功！\n",pack_id);
                    }
                    
                }

            }
		}
}
/*
 *创建服务端Socket，等待客户端连接
 */
int createServer(int Server_port){
    printf("start\n");
    server_fd=socket(AF_INET, SOCK_STREAM, 0);
    
    if(server_fd ==-1){
        perror("socket\n");
        exit(1);
    }
    int reused = 1;    //C中没有bool
    setsockopt(server_fd , SOL_SOCKET, SO_REUSEPORT ,(const void *)&reused , sizeof(reused));
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(Server_port);
    
    if(-1==bind(server_fd,( const struct sockaddr *)&addr, sizeof(addr))){
        perror("bind\n");
        exit(1);
    }
    /*listen*/
    if(-1==listen(server_fd, 10)){
        perror("listen\n");
        exit(2);
    }
    printf("Create Socket Successfully\n");
    
    int clientfd;
    struct sockaddr_in clientaddr;
    long addrlen=sizeof(clientaddr);
    pthread_t newthread;
    
    
    //保存每个客户端接收的最后一个包的id
    std::map<std::string,long> peerInfo;
    while (1) {
        //这里当有新客户请求时，创建新线程处理
        clientfd=accept(server_fd, (struct sockaddr *)&clientaddr, (socklen_t*)&addrlen);
        if(clientfd<0){
            perror("accept\n");
            exit(3);
        }
        

//        peerInfo.insert(<#const value_type &__v#>)
        
         if (pthread_create(&newthread , NULL, accept_request, client_sock) != 0)
            perror("pthread_create");
        
        
        //获取主机信息,根据主机信息存储文件断点
        struct hostent *peerInfo;
        peerInfo=gethostbyaddr(&(clientaddr.sin_addr.s_addr), 4, AF_INET);
        
        
        char filename[FileNameLen];
        bzero(filename, sizeof(filename));
        
        //读取客户端请求的文件名
        long  flag =read(clientfd, filename, sizeof(filename));
        if (flag==-1) {
            perror("recv name fail");
            exit(4);
        }
        else if (flag==0){
            printf("未收到请求的文件名");
            continue;
        }
        else {
            //打印请求的文件名
            printf("%s",filename);
            Pack data;
            long pack_id=0;
            FILE*fp=fopen(filename, "rb");
            long hlen=0;
            
            
            
            //到达文件尾部停止读取
            while(!feof(fp)){
                
               
                //对应包ID,从1开始计数
                pack_id++;
                data.head.id=pack_id;
                //fread返回实际读取到的字节数
                long  len=fread(data.buffer, sizeof(char), BufferSize, fp);
                data.head.buf_size=len;
                hlen=sizeof(data.head);
                if(len>BufferSize){
                    printf("读取文件出错\n");
                    break;
                }
                else if(len==0){
                    printf("到达文件尾部\n");
                    //因为文件传输成功，已到达末尾，删除记录的文件传输信息
                    break;
                }
                else{
                    
                    //发出包的字节数
                    long wlen=write(clientfd, &data, sizeof(data));
                    //比较包长度是否正确
                    if(wlen!=hlen+len){
                        perror("发送出错");
                        //重新发，待定
                    }
                    else{
                        printf("第%ld个包发送成功！\n",pack_id);
                    }
                    
                }

            }
            
        }
        
        
    }
    
    return server_fd;
}













































int connectServer(char* Server_IP,int Server_port){
    int client_fd=socket(AF_INET, SOCK_STREAM, 0);
    if(-1==client_fd){
        printf("socket\n");
        exit(1);
    }
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(Server_IP);
    addr.sin_port=htons(Server_port);
    if(-1==bind(client_fd , (struct sockaddr*)(&addr), sizeof(addr))){
        perror("bind");
        exit(1);
    }
    if(-1==connect(client_fd, (struct sockaddr*)&addr, sizeof(addr))){
        perror("connect");
        exit(1);
    }
    printf("connnect successfully\n");
    return client_fd;
}

