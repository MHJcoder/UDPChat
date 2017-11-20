//
//  Mysocket.h
//  SocDisplayer
//
//  Created by MHJ on 2017/8/1.
//  Copyright © 2017年 MHJ. All rights reserved.
//

#ifndef Mysocket_h
#define Mysocket_h
#include <CoreFoundation/CoreFoundation.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

CFWriteStreamRef s_writeStreamRef=NULL;
CFReadStreamRef s_readStreamRef=NULL;
CFWriteStreamRef c_writeStreamRef=NULL;
CFReadStreamRef c_readStreamRef=NULL;
CFSocketRef createServerSocket(UInt16 port);
void TCPServerAcceptCallBack(CFSocketRef socket,CFSocketCallBackType type, CFDataRef address, const void *data, void *info);
void connectToHost(CFStringRef IP,UInt16 port);
CFWriteStreamRef getServerInputStream();
CFReadStreamRef getServerOutputStream();
CFWriteStreamRef getClientInputStream();
CFReadStreamRef getClientOutputStream();

#endif /* Mysocket_h */
