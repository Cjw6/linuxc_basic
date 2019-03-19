//Ïß³ÌÍË³ö
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
void sys_err(char *str) {
	perror(str);
	exit(1);
}
void *tfn(void*arg)
{
	printf("thread var is running\n");
	printf("thread id=%lu\n", pthread_self());
	return NULL;
}
int main()
{
	printf("main is running,main id=%lu\n",pthread_self());
	pthread_t tid;
	int ret=pthread_create(&tid, NULL, tfn, NULL);
	if (ret < 0)
		sys_err("pthread_create error");
	sleep(1);
	printf("main thread id=%lu\n", pthread_self());
	pthread_exit((void*)1);
	sleep(1);
	printf("main finish\n");
	return 0;
}