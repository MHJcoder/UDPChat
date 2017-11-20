
//simpleclient.c
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int sockfd;
    int len;
    struct sockaddr_in address;
    int result;
    char ch = 'A';
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(9734);
    len = sizeof(address);
    result = connect(sockfd, (struct sockaddr *)&address, len);
    
    if (result == -1)
    {
        perror("oops: client1");
        exit(1);
    }
    write(sockfd, &ch, 1);
    read(sockfd, &ch, 1);
    printf("char from server = %c\n", ch);
    close(sockfd);
    exit(0);
}





/* J. David's webserver */
/* This is a simple webserver.
 * Created November 1999 by J. David Blackstone.
 * CSE 4344 (Network concepts), Prof. Zeigler
 * University of Texas at Arlington
 */
/* This program compiles for Sparc Solaris 2.6.
 * To compile for Linux:
 *  1) Comment out the #include <pthread.h> line.
 *  2) Comment out the line that defines the variable newthread.
 *  3) Comment out the two lines that run pthread_create().
 *  4) Uncomment the line that runs accept_request().
 *  5) Remove -lsocket from the Makefile.
 */
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>

#define ISspace(x) isspace((int)(x))

#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n"

void accept_request(int);
void bad_request(int);
void cat(int, FILE *);
void cannot_execute(int);
void error_die(const char *);
void execute_cgi(int, const char *, const char *, const char *);
int get_line(int, char *, int);
void headers(int, const char *);
void not_found(int);
void serve_file(int, const char *);
int startup(u_short *);
void unimplemented(int);

/**********************************************************************/
/* A request has caused a call to accept() on the server port to
 * return.  Process the request appropriately.
 * Parameters: the socket connected to the client */
/**********************************************************************/
//处理从Socket上监听到的一个http请求
void accept_request(int client)
{
    char buf[1024];
    int numchars;
    char method[255];
    char url[255];
    char path[512];
    size_t i, j;
    struct stat st;
    int cgi = 0;      /* becomes true if server decides this is a CGI
                       * program */
    char *query_string = NULL;
    //获得http请求的第一行
    numchars = get_line(client, buf, sizeof(buf));
    i = 0; j = 0;
    while (!ISspace(buf[j]) && (i < sizeof(method) - 1))//把客户端的请求方法存到method数组
    {
        method[i] = buf[j];
        i++; j++;
    }
    method[i] = '\0';
    
    if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))// 如果请求方式不是GET也不是POST，则不能处理该请求
    {
        unimplemented(client);//
        return;
    }
    /*
     *CGI程序主要用于对动态的请求要返回的结果
     *（例如数据库查询后获得的数据）进行处理，
     *转化为服务器或浏览器可以识别的格式，然后输出
     */
    if (strcasecmp(method, "POST") == 0)//请求方式为POST
        cgi = 1;    //开启CGI程序
    
    i = 0;
    while (ISspace(buf[j]) && (j < sizeof(buf)))//去除空格
        j++;
    while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < sizeof(buf)))//读取URL地址到url[]
    {
        url[i] = buf[j];
        i++; j++;
    }
    url[i] = '\0';//字符串结尾
    
    if (strcasecmp(method, "GET") == 0) //请求方式为GET
    {
        query_string = url; //获取url值
        while ((*query_string != '?') && (*query_string != '\0'))//遍历到请求的参数的位置
            query_string++;
        if (*query_string == '?')
        {
            cgi = 1;                //开启CGI
            *query_string = '\0';   //url结尾
            query_string++;
        }
    }
    
    sprintf(path, "htdocs%s", url); //格式化url并存入path数组，html文件都在htdocs文件夹
    if (path[strlen(path) - 1] == '/')  //没有指明html文件，默认为index.html
        strcat(path, "index.html");
    if (stat(path, &st) == -1) {        //获取服务器文件信息
        while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
            numchars = get_line(client, buf, sizeof(buf));
        not_found(client);  //返回页面不存在
    }
    else
    {
        if ((st.st_mode & S_IFMT) == S_IFDIR)   //如果是个目录，默认使用目录下的index.html文件
            strcat(path, "/index.html");
        if ((st.st_mode & S_IXUSR) ||
            (st.st_mode & S_IXGRP) ||
            (st.st_mode & S_IXOTH)    )         //需要开启CGI
            cgi = 1;
        if (!cgi)                               //不需要CGI
            serve_file(client, path);           //直接返回服务器文件
        else
            execute_cgi(client, path, method, query_string);    //执行CGI
    }
    
    close(client);  //关闭客户端
}

/**********************************************************************/
/* Inform the client that a request it has made has a problem.
 * Parameters: client socket */
/**********************************************************************/
void bad_request(int client)
{
    char buf[1024];
    //返回给客户端一个信息，提示错误的http请求
    
    sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");//格式化信息
    send(client, buf, sizeof(buf), 0);           //通过socket发送信息
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "<P>Your browser sent a bad request, ");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "such as a POST without a Content-Length.\r\n");
    send(client, buf, sizeof(buf), 0);
}

/**********************************************************************/
/* Put the entire contents of a file out on a socket.  This function
 * is named after the UNIX "cat" command, because it might have been
 * easier just to do something like pipe, fork, and exec("cat").
 * Parameters: the client socket descriptor
 *             FILE pointer for the file to cat */
/**********************************************************************/
void cat(int client, FILE *resource)    //给对应的客户端发送文件
{
    char buf[1024];
    
    fgets(buf, sizeof(buf), resource);//获取文件数据
    while (!feof(resource))           //读取文件数据并发送给客户端，直到到达文件尾
    {
        send(client, buf, strlen(buf), 0);
        fgets(buf, sizeof(buf), resource);
    }
}

/**********************************************************************/
/* Inform the client that a CGI script could not be executed.
 * Parameter: the client socket descriptor. */
/**********************************************************************/
void cannot_execute(int client)
{
    char buf[1024];
    //返回给客户端一个信息，提示CGI无法执行
    sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
    send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Print out an error message with perror() (for system errors; based
 * on value of errno, which indicates system call errors) and exit the
 * program indicating an error. */
/**********************************************************************/
void error_die(const char *sc)
{
    //出错信息处理
    perror(sc);
    exit(1);
}

/**********************************************************************/
/* Execute a CGI script.  Will need to set environment variables as
 * appropriate.
 * Parameters: client socket descriptor
 *             path to the CGI script */
/**********************************************************************/
void execute_cgi(int client, const char *path,
                 const char *method, const char *query_string)
{
    char buf[1024];
    int cgi_output[2];
    int cgi_input[2];
    pid_t pid;
    int status;
    int i;
    char c;
    int numchars = 1;
    int content_length = -1;
    
    buf[0] = 'A'; buf[1] = '\0';
    if (strcasecmp(method, "GET") == 0)
        //读取并丢弃所有HTTP header
        while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
            numchars = get_line(client, buf, sizeof(buf));
    else    /* POST */
    {
        numchars = get_line(client, buf, sizeof(buf));  //在POST的HTTP请求中找到Content-Length
        while ((numchars > 0) && strcmp("\n", buf))
        {
            buf[15] = '\0'; //利用\0结尾分隔
            if (strcasecmp(buf, "Content-Length:") == 0)
                content_length = atoi(&(buf[16]));      //获取Content-Length
            numchars = get_line(client, buf, sizeof(buf));
        }
        if (content_length == -1) {//没有找到Content-Length，返回客户端显示错误请求
            bad_request(client);
            return;
        }
    }
    //返回请求正确的http状态码
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    //建立输出管道
    if (pipe(cgi_output) < 0) {
        cannot_execute(client);//错误处理
        return;
    }
    //建立输入管道
    if (pipe(cgi_input) < 0) {
        cannot_execute(client);//错误处理
        return;
    }
    //子进程创建
    if ( (pid = fork()) < 0 ) {
        //创建失败处理
        cannot_execute(client);
        
        return;
    }
    //子进程操作
    if (pid == 0)  /* child: CGI script */
    {
        char meth_env[255];
        char query_env[255];
        char length_env[255];
        
        dup2(cgi_output[1], 1); //把标准输出流重定向到cgi_output的写入端
        dup2(cgi_input[0], 0);  //把标准输入流重定向到cgi_input的读取端
        //关闭cgi_input的写入端和cgi_output的读取端
        close(cgi_output[0]);
        close(cgi_input[1]);
        
        //设置request_method 的环境变量
        sprintf(meth_env, "REQUEST_METHOD=%s", method);
        putenv(meth_env);
        if (strcasecmp(method, "GET") == 0) {
            //get中设置 query_string 的环境变量
            sprintf(query_env, "QUERY_STRING=%s", query_string);
            putenv(query_env);
        }
        else {   /* POST */
            //post中设置 content_length 的环境变量
            sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
            putenv(length_env);
        }
        execl(path, path, NULL);    //用execl函数运行CGI程序
        exit(0);
    } else {    /* parent */
        //父进程
        //关闭cgi_input的读取端和cgi_output的写入端
        close(cgi_output[1]);
        close(cgi_input[0]);
        if (strcasecmp(method, "POST") == 0)
            for (i = 0; i < content_length; i++) {
                //接收POST的数据，写入到cgi_input
                recv(client, &c, 1, 0);
                write(cgi_input[1], &c, 1);
            }
        //读取cgi_output的管道输出到客户端，该管道输入是标准输出流
        while (read(cgi_output[0], &c, 1) > 0)
            send(client, &c, 1, 0);
        //关闭管道
        close(cgi_output[0]);
        close(cgi_input[1]);
        waitpid(pid, &status, 0);//等待子进程
    }
}

/**********************************************************************/
/* Get a line from a socket, whether the line ends in a newline,
 * carriage return, or a CRLF combination.  Terminates the string read
 * with a null character.  If no newline indicator is found before the
 * end of the buffer, the string is terminated with a null.  If any of
 * the above three line terminators is read, the last character of the
 * string will be a linefeed and the string will be terminated with a
 * null character.
 * Parameters: the socket descriptor
 *             the buffer to save the data in
 *             the size of the buffer
 * Returns: the number of bytes stored (excluding null) */
/**********************************************************************/
int get_line(int sock, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;
    
    while ((i < size - 1) && (c != '\n'))
    {
        n = recv(sock, &c, 1, 0);   //每次接收一个字节
        /* DEBUG printf("%02X\n", c); */
        if (n > 0)
        {
            if (c == '\r')//接收到‘\r',继续接收后续字节
            {
                n = recv(sock, &c, 1, MSG_PEEK);    //使用MSG_PEEK标志查看下一个字节是不是\n,并且不影响缓冲队列中的数据
                /* DEBUG printf("%02X\n", c); */
                if ((n > 0) && (c == '\n'))
                    recv(sock, &c, 1, 0);   //是\n，则直接读取下个字节
                else
                    c = '\n';       //不是\n,保存换行到buf
            }
            buf[i] = c;
            i++;
        }
        else
            c = '\n';   //如果读取到的字节数小于0，则保存一个\n到buf
    }
    buf[i] = '\0';  //添加结束符
    //返回buf数组大小
    return(i);
}

/**********************************************************************/
/* Return the informational HTTP headers about a file. */
/* Parameters: the socket to print the headers on
 *             the name of the file */
/**********************************************************************/
void headers(int client, const char *filename)
{
    char buf[1024];
    (void)filename;  /* could use filename to determine file type */
  
    strcpy(buf, "HTTP/1.0 200 OK\r\n");  //返回给客户端提示，HTTP header是正常的
    send(client, buf, strlen(buf), 0);
    
    strcpy(buf, SERVER_STRING);     //返回给客户端服务器信息
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Give a client a 404 not found status message. */
/**********************************************************************/
void not_found(int client)
{
    char buf[1024];
    //返回给客户端显示HTTP header没找到，不存在
    sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n"); //格式化信息
    send(client, buf, strlen(buf), 0);          //发送数据给客户端
    sprintf(buf, SERVER_STRING);                //格式化信息
    send(client, buf, strlen(buf), 0);          //发送数据给客户端
    sprintf(buf, "Content-Type: text/html\r\n");//格式化信息
    send(client, buf, strlen(buf), 0);           //发送数据给客户端
    sprintf(buf, "\r\n");                        //格式化信息
    send(client, buf, strlen(buf), 0);             //发送数据给客户端
    sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n"); //使用html显示返回内容，提示Not Found
    send(client, buf, strlen(buf), 0);          //发送数据给客户端
    sprintf(buf, "<BODY><P>The server could not fulfill\r\n");//格式化信息
    send(client, buf, strlen(buf), 0);           //发送数据给客户端
    sprintf(buf, "your request because the resource specified\r\n");//格式化信息
    send(client, buf, strlen(buf), 0);           //发送数据给客户端
    sprintf(buf, "is unavailable or nonexistent.\r\n");//格式化信息
    send(client, buf, strlen(buf), 0);           //发送数据给客户端
    sprintf(buf, "</BODY></HTML>\r\n");             //格式化信息
    send(client, buf, strlen(buf), 0);            //发送数据给客户端
}

/**********************************************************************/
/* Send a regular file to the client.  Use headers, and report
 * errors to client if they occur.
 * Parameters: a pointer to a file structure produced from the socket
 *              file descriptor
 *             the name of the file to serve */
/**********************************************************************/
void serve_file(int client, const char *filename)
{
    FILE *resource = NULL;
    int numchars = 1;
    char buf[1024];
    
    //
    buf[0] = 'A'; buf[1] = '\0';
    while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
        numchars = get_line(client, buf, sizeof(buf));
    
    resource = fopen(filename, "r");//打开服务器中要发送的文件
    if (resource == NULL)
        not_found(client);          //如果文件不存在，显示返回给客户端提示未找到
    else
    {
        headers(client, filename);  //为客户端写响应头
        cat(client, resource);      //把文件资源发送到客户端
    }
    fclose(resource);   //关闭文件
}

/**********************************************************************/
/* This function starts the process of listening for web connections
 * on a specified port.  If the port is 0, then dynamically allocate a
 * port and modify the original port variable to reflect the actual
 * port.
 * Parameters: pointer to variable containing the port to connect on
 * Returns: the socket */
/**********************************************************************/
int startup(u_short *port)
{
    int httpd = 0;
    struct sockaddr_in name;
    
    httpd = socket(PF_INET, SOCK_STREAM, 0);    //创建Socket
    if (httpd == -1)
        error_die("socket");    //失败处理
    memset(&name, 0, sizeof(name)); //name初始化为0
    name.sin_family = AF_INET;      //设置协议族
    name.sin_port = htons(*port);   //设置端口
    name.sin_addr.s_addr = htonl(INADDR_ANY);   //服务端设置任意主机可连
    if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)    //将主机地址端口与socket绑定在一起
        error_die("bind");              //失败处理
    //若参数port传进来为0，则动态分配一个端口
    if (*port == 0)  /* if dynamically allocating a port */
    {
        int namelen = sizeof(name);
        if (getsockname(httpd, (struct sockaddr *)&name, &namelen) == -1)
            error_die("getsockname");
        *port = ntohs(name.sin_port);
    }
    if (listen(httpd, 5) < 0)//开始监听，设置并发数为5
        error_die("listen");//错误处理
    return(httpd);  //返回Socket的文字描述符
}

/**********************************************************************/
/* Inform the client that the requested web method has not been
 * implemented.
 * Parameter: the client socket */
/**********************************************************************/
void unimplemented(int client)
{
    //返回客户端提示客户端的请求方式不被支持，
    char buf[1024];
    //响应头
    sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    //返回服务器的信息
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    //响应客户端请求的主体
    sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</TITLE></HEAD>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

/**********************************************************************/

int main(void)
{
    int server_sock = -1;
    u_short port = 0;
    int client_sock = -1;
    struct sockaddr_in client_name;
    int client_name_len = sizeof(client_name);
    pthread_t newthread;
    
    //根据对应端口创建httpd服务
    server_sock = startup(&port);
    printf("httpd running on port %d\n", port);
    
    while (1)
    {
        //接收客户端的连接请求
        client_sock = accept(server_sock,
                             (struct sockaddr *)&client_name,
                             &client_name_len);
        if (client_sock == -1)  //连接失败
            error_die("accept");//失败处理
        /* accept_request(client_sock); */
        //开辟新线程
        if (pthread_create(&newthread , NULL, accept_request, client_sock) != 0)
            perror("pthread_create");
    }
    
    close(server_sock);
    
    return(0);
}
