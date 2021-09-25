//server.c，服务器端 
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

int main(){
	int shmid;//共享内存id 
	int key=5678;
	pid_t p;
	char c;
	char *shmptr,*s;
	
	if((shmid=shmget(key,1024,0666|IPC_CREAT))<0){//打开key=5678的共享内存，如果不存在则创建一个 
		printf("Shmget failed\n");//shmget出错退出
		exit(1); 
	}
	if((shmptr=shmat(shmid,0,0))==(char*)-1){//如果成功，返回内存区域的指针，出错返回-1
		shmctl(shmid,IPC_RMID,(struct shmid_ds*)shmptr);//调用shmctl删除 
		printf("Shmat error\n");
		exit(2);
	}
	if((p=fork())<0){//创建一个子进程 
		printf("Fork failed\n");
		exit(1);
	} 
	
	while(1){
		if(p==0){//子进程 
			while(*shmptr!='c')		sleep(1);//如果客户端还没发送信息，则阻塞读操作
			printf("Receive from the client\n");
			for(s=shmptr+1;*s!='\0';s++){
				putchar(*s);//将读到的内容进行输出 
			}
			printf("\n");//结束输出，换行 
			*shmptr='!';//把'!'写入到共享内存，表示读取结束，同时阻塞读操作
			printf("Finish receive, wait for next time entering\n"); 
		}
		else if(p>0){//父进程 
			printf("Server is waiting for entering\n");
			scanf("%[^\n]%*c",shmptr+1);//[^\n]表示读入换行字符就结束读入，%c可以把’\n’吸收掉 
			*shmptr='s';//解除客户端的读操作阻塞 
			while(*shmptr!='!')		sleep(1);//读取完成才能再写入 
			printf("Send message to client successfully\n"); 
		}
	}
	return 0;
}
