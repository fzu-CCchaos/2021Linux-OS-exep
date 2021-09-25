//server.c
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
#define FILENAME "message.txt" //在当前目录下的message.txt文件中进行
#define FLAG O_RDWR | O_CREAT | O_TRUNC | O_APPEND 
#define MODE 0600 //读写方式 

int main(){
	int sfd,cfd;//服务器 客户端套接字标识符 
	int filef; //I/O文件
	const char *filepath=FILENAME;//指针指向将要操作的文件
	if((filef=open(filepath,FLAG,MODE))==-1){
		printf("open file failed\n");
		exit(1);
	} 
	int len=sizeof(struct sockaddr_in);
	struct sockaddr_in saddr,caddr;//套接字接口数据结构 
	 
	char sendbuffer[1024];
	char recebuffer[1024];//定义自己的发送和接收缓冲区
	char s_to_c[]="server --> client";
	char c_to_s[]="client --> server";
	sfd=socket(AF_INET,SOCK_STREAM,0);//socket()函数创建套接字，ipv4,TCP
	if(sfd==-1){
		printf("socket error\n");
		exit(1);
	}
	int optval=1;
	
	if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval))<0){
		perror("setsockopt error");//若无错误发生，setsockopt()返回0。否则返回-1错误
		exit(-1);
	}//setsockopt()函数用于任意类型、任意状态套接口的设置选项值
	
	saddr.sin_family=AF_INET;//sin_family表示协议簇，一般用AF_INET表示TCP/IP协议
	saddr.sin_addr.s_addr=htonl(INADDR_ANY);//填写ip,为本机地址 
	saddr.sin_port=htons(PORT);//服务器socket端口
	
	if(bind(sfd,(struct sockaddr*) &saddr,len)==-1){
		printf("bind error\n");
		exit(1);
	}
	if(listen(sfd,5)==-1){
		printf("lissen error\n");
		exit(1);
	}
	if((cfd=accept(sfd,(struct sockaddr*) &caddr,&len))==-1){
		printf("accept error\n");
		exit(1);
	}//错误处理 
	printf("connect success\n");//连接成功 
	
	while(1){
		while(recv(cfd,recebuffer,1024,0)<=0){
			sleep(1);
		} 
		printf("receive some message from client:\n");
		printf("%s\n",recebuffer);//将收到的信息输出
		write(filef,c_to_s,strlen(c_to_s));
		int p=strlen(recebuffer);
		recebuffer[p]='\n';
		recebuffer[p+1]='\0';
		write(filef,recebuffer,strlen(recebuffer));
		
		printf("Send some message to the client:\n");
		scanf("%[^\n]",sendbuffer);
		getchar();
		send(cfd,sendbuffer,sizeof(sendbuffer),0);//将消息发送出去
		p=strlen(sendbuffer);
		sendbuffer[p]='\n';
		sendbuffer[p+1]='\0';
		write(filef,s_to_c,strlen(s_to_c));
		write(filef,sendbuffer,strlen(sendbuffer));
	}
	close(sfd);
	close(cfd);
	return 0; 
}
