//线程共享全局变量
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
int var = 100;
void *tfn(void*arg)
{
	//var = 200;
	printf("thread var=%d\n",var);
}
int main()
{
	printf("main1 var=%d\n", var);
	pthread_t tid;
	pthread_create(&tid, NULL, tfn, NULL);
	printf("main2 var=%d\n", var);
	sleep(1);
	return 0;
}