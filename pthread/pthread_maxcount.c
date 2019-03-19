#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
void sys_err(char *str)
{
	perror(str);
	exit(1);
}
void *pthread_fun(void *arg)
{
	//printf("in pthread:pthread id=%lu,pid=%d\n", pthread_self(), getpid());
	while (1);
}
int main(int argc, char const *argv[])
{
	int ret,count=0;
	pthread_t tid;
	while (1) {
		ret = pthread_create(&tid, NULL, pthread_fun, NULL);
		if (ret != 0)
			break;
		count++;
		printf("pthread conunt:%d\n", count);
	}
	printf("finish\npthread max count:%d", count);
	return 0;
}
// gcc pthread_create.c -lpthread
//ps aux 查看进程
//ps -Lf 加进程pid 查看制定pid的线程