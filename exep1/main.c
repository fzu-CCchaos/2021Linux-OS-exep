//main.c 
#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include"count.h" //head files

int main(){
	struct sigaction si;//define the struct variable
	si.sa_sigaction=exit_count;//signal handle function
	si.sa_flags=SA_SIGINFO;//表明信号处理函数由sa_sigaction指定
	sigemptyset(&si.sa_mask);//clear the signal set
	sigaction(SIGINT,&si,NULL);//register SIGINT signal
	
	count(); //run count() function 
	return 0;
}
