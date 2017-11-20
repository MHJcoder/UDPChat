
Webbench源码解析

/* $Id: socket.c 1.1 1995/01/01 07:11:14 cthuang Exp $
 *
 * This module has been modified by Radim Kolar for OS/2 emx
 */

/***********************************************************************
 module: socket.c
 program: popclient
 SCCS ID: @(#)socket.c 1.5 4/1/94
 programmer: Virginia Tech Computing Center
 compiler: DEC RISC C compiler (Ultrix 4.1)
 environment: DEC Ultrix 4.3
 description: UNIX sockets code.
 ***********************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/*******
 socket，一般被称为套接字，一个socket由一个IP地址和端口号组成
 通过socket可以建立长连接，实现网络中不同主机间的数据传输。
 这个函数主要使用Linux底层的socket接口，
 即BSDsocket实现对网络地址host和端口clientPort建立socket连接
 成功返回创建的socket的文件描述符，失败返回-1
 ********/

int Socket(const char *host, int clientPort)
{
    int sock; //socket文件描述符
    unsigned long inaddr;
    struct sockaddr_in ad;//地址结构体
    struct hostent *hp;
    
    memset(&ad, 0, sizeof(ad));//内存清零，防止原始数据的影响
    ad.sin_family = AF_INET; //设置协议族
    
    inaddr = inet_addr(host);//将hostIP点分十进制转为无符号长整形
    if (inaddr != INADDR_NONE)//地址无效
        memcpy(&ad.sin_addr, &inaddr, sizeof(inaddr));
    else//host是个域名
    {
        hp = gethostbyname(host); //根据域名获取IP
        if (hp == NULL)
            return -1;
        memcpy(&ad.sin_addr, hp->h_addr, hp->h_length);
    }
    ad.sin_port = htons(clientPort);
    
    sock = socket(AF_INET, SOCK_STREAM, 0); //根据host的IP和clientPort创建socket
    if (sock < 0)
        return sock; //返回文件描述符
    if (connect(sock, (struct sockaddr *)&ad, sizeof(ad)) < 0)
        return -1; //失败
    return sock;
}































/*
 * (C) Radim Kolar 1997-2004
 * This is free software, see GNU Public License version 2 for
 * details.
 *
 * Simple forking WWW Server benchmark:
 *
 * Usage:
 * webbench --help
 *
 * Return codes:
 * 0 - sucess
 * 1 - benchmark failed (server is not on-line)
 * 2 - bad param
 * 3 - internal error, fork failed
 *
 */
#include "socket.c"
#include <unistd.h>
#include <sys/param.h>
#include <rpc/types.h>
#include <getopt.h>
#include <strings.h>
#include <time.h>
#include <signal.h>

/* values */
volatile int timerexpired=0;
int speed=0;
int failed=0;
int bytes=0;
/* globals */

//各种全局变量
//这里是http协议的版本号
int http10=1; /* 0 - http/0.9, 1 - http/1.0, 2 - http/1.1 */

/* Allow: GET, HEAD, OPTIONS, TRACE */
//对http方法进行宏定义
#define METHOD_GET 0
#define METHOD_HEAD 1
#define METHOD_OPTIONS 2
#define METHOD_TRACE 3
#define PROGRAM_VERSION "1.5"

int method=METHOD_GET;//指定http方法为GET
int clients=1;//指定客户端数目，即socket连接并发数
int force=0;//指定是否等待服务器响应，0为不等待
int force_reload=0;//是否使用cache，默认为0，使用
int proxyport=80;//代理服务器端口号，默认为80
char *proxyhost=NULL;//代理服务器地址
int benchtime=30;//bench运行多久，默认是30秒。可以通过-t自定
/* internal */
int mypipe[2];//管道的文件描述符，0表示读管道，1表示写管道
char host[MAXHOSTNAMELEN];//服务器ip

#define REQUEST_SIZE 2048//请求最大个数
char request[REQUEST_SIZE];//http请求数组


//配合getopt_long函数使用
/*每个元素对应一个长选项，由四个域组成
  第一个域，描述长选项的名称
  第二个域，代表该选项是否需要跟着参数，1需要，0不需要
  第三个域，对应第四个元素需要的某个属性值，不需要则设为NULL
  第四个域，长选项对应短选项的名称
 option的定义：
 struct option{    //长选项表
 const char *name; //选项名，前面没有短横线，help,verbose之类
 int has_arg;      //描述选项是否需要选项参数，no_argument 0 表示选项没有参数，required_argument 1 表示需要参数，optional_argument 2 选项参数可选
 int *flag;  //如果这个指针为NULL，那么getopt_long()返回该结构val字段中的数值。如果该指针不为NULL，getopt_long()会使得它所指向的变量中填入val字段中的数值，并且getopt_long()返回0
 int val;       //如果flag是NULL，那么val通常是个字符常量，如果短选项和长选项一致，那么该字符就应该与optstring中出现的这个选项的参数相同；
 };
 */
static const struct option long_options[]=
{
 {"force",no_argument,&force,1},
 {"reload",no_argument,&force_reload,1},
 {"time",required_argument,NULL,'t'},
 {"help",no_argument,NULL,'?'},
 {"http09",no_argument,NULL,'9'},
 {"http10",no_argument,NULL,'1'},
 {"http11",no_argument,NULL,'2'},
 {"get",no_argument,&method,METHOD_GET},
 {"head",no_argument,&method,METHOD_HEAD},
 {"options",no_argument,&method,METHOD_OPTIONS},
 {"trace",no_argument,&method,METHOD_TRACE},
 {"version",no_argument,NULL,'V'},
 {"proxy",required_argument,NULL,'p'},
 {"clients",required_argument,NULL,'c'},
 {NULL,0,NULL,0}
};

/* prototypes */
/*协议*/

static void benchcore(const char* host,const int port, const char *request);//
static int bench(void);
static void build_request(const char *url);


//信号处理，将timerexpored置1，timerexpired表示压测时长是否已经到达设定的时间
static void alarm_handler(int signal)
{
   timerexpired=1;
}	
//说明程序命令使用方法
static void usage(void)
{
   fprintf(stderr,
	"webbench [option]... URL\n"
	"  -f|--force               Don't wait for reply from server.\n"
	"  -r|--reload              Send reload request - Pragma: no-cache.\n"
	"  -t|--time <sec>          Run benchmark for <sec> seconds. Default 30.\n"
	"  -p|--proxy <server:port> Use proxy server for request.\n"
	"  -c|--clients <n>         Run <n> HTTP clients at once. Default one.\n"
	"  -9|--http09              Use HTTP/0.9 style requests.\n"
	"  -1|--http10              Use HTTP/1.0 protocol.\n"
	"  -2|--http11              Use HTTP/1.1 protocol.\n"
	"  --get                    Use GET request method.\n"
	"  --head                   Use HEAD request method.\n"
	"  --options                Use OPTIONS request method.\n"
	"  --trace                  Use TRACE request method.\n"
	"  -?|-h|--help             This information.\n"
	"  -V|--version             Display program version.\n"
	);
};


//主函数，argc指示命令行输入参数的个数，argv存储所有命令行参数，字符串数组
int main(int argc, char *argv[])
{
 int opt=0;
 int options_index=0;
 char *tmp=NULL;

 if(argc==1)    //在不带参数的时候显示程序命令的使用方法
 {
	  usage();      //调用usage（）函数显示
          return 2;
 } 

 while((opt=getopt_long(argc,argv,"912Vfrt:p:c:?h",long_options,&options_index))!=EOF )//匹配输入参数，获取对应选项
 {
  switch(opt)   //根据命令行参数执行不同操作，设置所涉及到的全局变量的值
  {
   case  0 : break;
   case 'f': force=1;break; //等待服务器响应
   case 'r': force_reload=1;break; //不使用cache，不缓存页面
   case '9': http10=0;break;//使用的http版本
   case '1': http10=1;break;
   case '2': http10=2;break;
   case 'V': printf(PROGRAM_VERSION"\n");exit(0);//打印版本号
   case 't': benchtime=atoi(optarg);break;	     //设置压测时间
   case 'p': 
	     /* proxy server parsing server:port */
         /*optarg表示命令后的参数*/
	     tmp=strrchr(optarg,':');   //寻找“:“在optarg中最后一次出现的位置，返回从该位置起到字符结束的所有字符，这里相当于获取主机端口号
	     proxyhost=optarg;  //设定地址
	     if(tmp==NULL)
	     {
		     break;
	     }
	     if(tmp==optarg)//若optarg和tmp相等，表示主机名缺失
	     {
		     fprintf(stderr,"Error in option --proxy %s: Missing hostname.\n",optarg);
		     return 2;
	     }
	     if(tmp==optarg+strlen(optarg)-1)//若tmp和optarg最后的字符相同，表示端口号缺失
	     {
		     fprintf(stderr,"Error in option --proxy %s Port number is missing.\n",optarg);
		     return 2;
	     }
	     *tmp='\0';
	     proxyport=atoi(tmp+1);break;//重设端口号
   case ':':
   case 'h':
   case '?': usage();return 2;break;
   case 'c': clients=atoi(optarg);break;    //设置并发数
  }
 }
 //optind：命令行参数中未读取的下一个元素下标
 if(optind==argc) {
                      fprintf(stderr,"webbench: Missing URL!\n");
		      usage();
		      return 2;
                    }

 if(clients==0) clients=1;  //并发数至少设置为1
 if(benchtime==0) benchtime=60;//测试时间至少为1分钟
 /* Copyright */
 fprintf(stderr,"Webbench - Simple Web Benchmark "PROGRAM_VERSION"\n"
	 "Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.\n"
	 );
 build_request(argv[optind]);   //封装http请求
 /* print bench info */
 printf("\nBenchmarking: ");//输出http请求信息
 switch(method)
 {
	 case METHOD_GET:
	 default:
		 printf("GET");break;
	 case METHOD_OPTIONS:
		 printf("OPTIONS");break;
	 case METHOD_HEAD:
		 printf("HEAD");break;
	 case METHOD_TRACE:
		 printf("TRACE");break;
 }
 printf(" %s",argv[optind]);//打印使用的http版本
 switch(http10)
 {
	 case 0: printf(" (using HTTP/0.9)");break;
	 case 2: printf(" (using HTTP/1.1)");break;
 }
 printf("\n");
    //打印实际客户端个数，也就是并发数目
 if(clients==1) printf("1 client");
 else
   printf("%d clients",clients);
//打印实际测试时间
 printf(", running %d sec", benchtime);
 if(force) printf(", early socket close");  //
 if(proxyhost!=NULL) printf(", via proxy server %s:%d",proxyhost,proxyport);//打印服务器ip和port
 if(force_reload) printf(", forcing reload");//打印是否缓存
 printf(".\n");
 return bench();    //开始测试，返回结果
}

//这个函数用于封装http请求的信息，参数是url地址，所有封装好的http请求信息放在request数组中
void build_request(const char *url)
{
  char tmp[10];
  int i;
    //bzero（s,n)的功能是把字节字符串s的前n个字节设置为0
    //这里将请求地址和请求初始化为0
  bzero(host,MAXHOSTNAMELEN);
  bzero(request,REQUEST_SIZE);
    //根据情况设置http版本
  if(force_reload && proxyhost!=NULL && http10<1) http10=1;
  if(method==METHOD_HEAD && http10<1) http10=1;
  if(method==METHOD_OPTIONS && http10<2) http10=2;
  if(method==METHOD_TRACE && http10<2) http10=2;
    //根据method设置http请求的方式
  switch(method)
  {
	  default:
	  case METHOD_GET: strcpy(request,"GET");break;
	  case METHOD_HEAD: strcpy(request,"HEAD");break;
	  case METHOD_OPTIONS: strcpy(request,"OPTIONS");break;
	  case METHOD_TRACE: strcpy(request,"TRACE");break;
  }

  strcat(request," ");
  if(NULL==strstr(url,"://"))   //判断url地址是否合法
  {
	  fprintf(stderr, "\n%s: is not a valid URL.\n",url);
	  exit(2);
  }
  if(strlen(url)>1500)  //判断url是否过长
  {
         fprintf(stderr,"URL is too long.\n");
	 exit(2);
  }
  if(proxyhost==NULL)   //判断是否有代理服务器
	   if (0!=strncasecmp("http://",url,7))     //判断url的开头是不是http://
	   { fprintf(stderr,"\nOnly HTTP protocol is directly supported, set --proxy for others.\n");
             exit(2);
           }
  /* protocol/host delimiter */
  i=strstr(url,"://")-url+3;    //获取”://“在url中第一次出现的位置，并返回://之后的第一个位置
  /* printf("%d\n",i); */

  if(strchr(url+i,'/')==NULL) { //查找/在url后面第一次出现的位置
                                fprintf(stderr,"\nInvalid URL syntax - hostname don't ends with '/'.\n");
                                exit(2);
                              }
  if(proxyhost==NULL)   //判断是否有代理服务器
  {
   /* get port from hostname */
      //index(s,c)返回s中第一个出现的参数c的地址并返回
   if(index(url+i,':')!=NULL &&
      index(url+i,':')<index(url+i,'/'))//判断：是否出现，：后面一般接要访问的主机的端口地址
   {
	   strncpy(host,url+i,strchr(url+i,':')-url-i); //给全局变量host即主机地址赋值
	   bzero(tmp,10);   //置0
	   strncpy(tmp,index(url+i,':')+1,strchr(url+i,'/')-index(url+i,':')-1);    //给端口号赋值
	   /* printf("tmp=%s\n",tmp); */
	   proxyport=atoi(tmp);//代理服务器端口
	   if(proxyport==0) proxyport=80;   //默认设置为80
   } else
   {
     strncpy(host,url+i,strcspn(url+i,"/"));    //若url没有设置主机的访问端口，直接获取主机名
   }
   // printf("Host=%s\n",host);
   strcat(request+strlen(request),url+i+strcspn(url+i,"/"));    //将url中的主机名和端口号拼接到请求中
  } else
  {
   // printf("ProxyHost=%s\nProxyPort=%d\n",proxyhost,proxyport);
   strcat(request,url);     //没有代理服务器，直接将url拼接到请求中
  }
    
    //开始封装http请求信息
  if(http10==1)
	  strcat(request," HTTP/1.0");  //设置版本号
  else if (http10==2)
	  strcat(request," HTTP/1.1");
  strcat(request,"\r\n");
  if(http10>0)
	  strcat(request,"User-Agent: WebBench "PROGRAM_VERSION"\r\n");//用户代理
  if(proxyhost==NULL && http10>0)   //添加主机名
  {
	  strcat(request,"Host: ");
	  strcat(request,host);
	  strcat(request,"\r\n");
  }
  if(force_reload && proxyhost!=NULL)//添加是否缓存
  {
	  strcat(request,"Pragma: no-cache\r\n");
  }
  if(http10>1)//设置连接
	  strcat(request,"Connection: close\r\n");
  /* add empty line at end */
  if(http10>0) strcat(request,"\r\n"); 
  // printf("Req=%s\n",request);
}

/* vraci system rc error kod */
//函数功能：创建管道，派生子进程，用子进程来测试http请求
static int bench(void)
{
  int i,j,k;	
  pid_t pid=0;
  FILE *f;

  /* check avaibility of target server */
    //使用socket函数创建socket连接，测试地址是否可以正常访问
  i=Socket(proxyhost==NULL?host:proxyhost,proxyport);
  if(i<0) { 
	   fprintf(stderr,"\nConnect to server failed. Aborting benchmark.\n");
           return 1;
         }
  close(i);//通过文件描述符关闭socket
  /* create pipe */
  if(pipe(mypipe))  //创建管道，用于父子进程数据传输
  {
	  perror("pipe failed.");
	  return 3;
  }

  /* not needed, since we have alarm() in childrens */
  /* wait 4 next system clock tick */
  /*
  cas=time(NULL);
  while(time(NULL)==cas)
        sched_yield();
  */

  /* fork childs */
    //根据客户端个数即并发数创建子进程
  for(i=0;i<clients;i++)
  {
	   pid=fork();
	   if(pid <= (pid_t) 0)
	   {
		   /* child process or error*/
	           sleep(1); /* make childs faster */
		   break;       //若本进程为子进程，跳出循环，防止子进程也创建子进程，造成混乱
	   }
  }

  if( pid< (pid_t) 0)//创建子进程失败返回负数
  {
          fprintf(stderr,"problems forking worker no. %d\n",i);
	  perror("fork failed.");
	  return 3;
  }

  if(pid== (pid_t) 0)   //子进程的pid为0
  {
    /* I am a child */
    if(proxyhost==NULL)     //判断是否使用代理主机
      benchcore(host,proxyport,request);  //测试http请求
         else
      benchcore(proxyhost,proxyport,request);

         /* write results to pipe */
	 f=fdopen(mypipe[1],"w");//打开管道写
	 if(f==NULL)//打开失败
	 {
		 perror("open pipe for writing failed.");
		 return 3;
	 }
	 /* fprintf(stderr,"Child - %d %d\n",speed,failed); */
	 fprintf(f,"%d %d %d\n",speed,failed,bytes);    //把测试的结果写到管道里面
	 fclose(f); //关闭管道
	 return 0;
  } else
  {
	  f=fdopen(mypipe[0],"r");  //打开管道读取数据
	  if(f==NULL) 
	  {
		  perror("open pipe for reading failed.");
		  return 3;
	  }
	  setvbuf(f,NULL,_IONBF,0); //设置文件缓冲区函数，可以让用户自己建立自己的文件缓冲情趣，而不是用fopen默认的缓冲区，可以使用malloc分配缓冲区
	  speed=0;//传输速度
          failed=0;//失败的请求数
          bytes=0;//传输的字节数

	  while(1)
	  {
		  pid=fscanf(f,"%d %d  %d",&i,&j,&k);//父进程读取管道数据
		  if(pid<2)
                  {
                       fprintf(stderr,"Some of our childrens died.\n");
                       break;
                  }
		  speed+=i; //传输速度计数
		  failed+=j;   //失败请求计数
		  bytes+=k;     //传输字节数计数
		  /* fprintf(stderr,"*Knock* %d %d read=%d\n",speed,failed,pid); */
		  if(--clients==0) break;
	  }
	  fclose(f);//关闭文件
      //输出测试结果
  printf("\nSpeed=%d pages/min, %d bytes/sec.\nRequests: %d susceed, %d failed.\n",
		  (int)((speed+failed)/(benchtime/60.0f)),  //
		  (int)(bytes/(float)benchtime),
		  speed,
		  failed);
  }
  return i;
}

//函数功能：测试http请求，参数含义：地址，端口，请求内容
void benchcore(const char *host,const int port,const char *req)
{
 int rlen;
 char buf[1500];    //存放服务器相应请求返回的数据
 int s,i;
 struct sigaction sa;

 /* setup alarm signal handler */
 sa.sa_handler=alarm_handler;//设置信号处理句柄
 sa.sa_flags=0;
 if(sigaction(SIGALRM,&sa,NULL))//在超时的时候产生SIGALRM信号，调用alarm_handler函数处理信号
    exit(3);
 alarm(benchtime);  //设置闹钟函数，开始计时

 rlen=strlen(req);  //请求的长度
 nexttry:while(1)
 {
     //计时结束的时候产生信号，信号处理函数将timerexpored置1，退出函数
    if(timerexpired)
    {
       if(failed>0)
       {
          /* fprintf(stderr,"Correcting failed by signal\n"); */
          failed--;
       }
       return;
    }
    s=Socket(host,port);     //建立socket连接，获取文件描述符
    if(s<0) { failed++;continue;} //建立连接，failed+1
    if(rlen!=write(s,req,rlen)) {failed++;close(s);continue;}//发出请求，若失败，failed+1，关闭socket
    if(http10==0) //如果http版本号为HTTP0.9，进行特殊处理
	    if(shutdown(s,1)) { failed++;close(s);continue;}
    if(force==0) //判断是否等待服务器响应
    {
            /* read all available data from socket */
	    while(1)
	    {
              if(timerexpired) break;//如果超时，停止
	      i=read(s,buf,1500);   //读取服务器相应数据，存放在数组buf中，返回值i表示读取成功或是失败，成功返回读取的字节数
              /* fprintf(stderr,"%d\n",i); */
	      if(i<0) //读取失败。failed+1，关闭socket，继续循环
              { 
                 failed++;
                 close(s);
                 goto nexttry;
              }
	       else//读取成功
		       if(i==0) break;
		       else
			       bytes+=i;    //把成功读取的字节数记录好
	    }
    }
    if(close(s)) {failed++;continue;}   //关闭socket，继续循环
    speed++;    //http测试成功，速度加1
 }
}
