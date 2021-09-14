//count.c,include count() and exit_count()
#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<time.h>
#include"count.h" //head files

char *para[]={"echo","exit","success!","\n",NULL};

void exit_count(int iSignNum,siginfo_t *p_info,void *p_reserved){
	printf("\n exit count game. \n");
	if(execv("/bin/echo",para)==-1){
		perror("execv");
		exit(1);
	} 
	exit(0);
	return;
}

void count(){
	int times,t,lock=1;//store the system time,user's answer,lock the child process
	int p;
	time_t timep;
	struct tm *s;
	time(&timep);
	s=gmtime(&timep);
	times=s->tm_sec;//get the system time
	
	while((p=fork())==-1);//cycle until create a new process
	
	if(p<0){
		perror("fork");
		exit(1);
	}
	else if(p==0){//locked until exit or success
		while(lock);
		exit(0);
	}
	else{//parent process
		printf("guess the current second of the system,range is 0-60. ctrl+c to exit\n");
		while(scanf("%d",&t)!=-1){
			if(t<0||t>60)	printf("out of range,range is 0-60\n");
			else if(t==times){
				printf("Success!\n");
				break;
			}
			else if(t>times)	printf("this answer is too big. ctrl+c to exit\n");
			else	printf("this answer is too small. ctrl+c to exit\n");
		}
		lock=0;//unlock the child process
		exit(0);
	}
	return;
}