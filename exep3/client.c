//client.c 
#include <sys/types.h>
#include <sys/socket.h>	 // 包含套接字函数库
#include <stdio.h>
#include <stdlib.h> 
#include <netinet/in.h>	 // 包含AF_INET相关结构
#include <arpa/inet.h>	 // 包含AF_INET相关操作的函数
#include <unistd.h>
//#include <netdb.n>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#define PORT 8080

int main(){
	int sockf,newsockf;//客户端套接字标识符
	struct sockaddr_in addr; //存储远程服务器的ip与端口号地址 
	char sendbuffer[1024];
	char recebuffer[1024];//定义自己的发送和接收缓冲区
	sockf=socket(AF_INET,SOCK_STREAM,0);//socket()函数创建套接字，ipv4,TCP
	int optval=1;
	
	if(setsockopt(sockf,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval))<0){
		perror("setsockopt error");//若无错误发生，setsockopt()返回0。否则返回-1错误
		exit(1);
	}//setsockopt()函数用于任意类型、任意状态套接口的设置选项值
	
	addr.sin_family=AF_INET;//sin_family表示协议簇，一般用AF_INET表示TCP/IP协议
	addr.sin_addr.s_addr=htonl(INADDR_ANY);//填写ip 
	addr.sin_port=htons(PORT);//客户端的socket端口
	
	newsockf=connect(sockf,(struct sockaddr*) &addr, sizeof(addr));//发送连接服务器的请求
	if(newsockf==-1){
		perror("connect failed");
		return 1;
	} 
	while(1){
		printf("Send some message to the server:\n");
		scanf("%[^\n]",sendbuffer);
		getchar();
		send(sockf,sendbuffer,sizeof(sendbuffer),0);//将消息发送出去
		printf("wait the server respond ...\n");
		while(recv(sockf,recebuffer,1024,0)<=0){
			sleep(1);//不断等待直到收到回复 
		} 
		printf("message received from server:\n");
		printf("%s\n",recebuffer);//将收到的信息输出 
	}
	close(sockf);
	return 0; 
}
