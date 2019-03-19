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
	return NULL;
}
int main(int argc, char const *argv[])
{
	printf("in main1:pthread id=%lu,pid=%d\n", pthread_self(), getpid());
	pthread_t tid;
	pthread_attr_t attr;
	int ret=pthread_attr_init(&attr);
	if (ret != 0) {
		fprintf(stderr, "thread_init:error:%s\n", strerror(ret));//将错误号转变为字符串描述
		exit(1);
	}
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//使用atr将建立的线程设置为分离态
	ret = pthread_create(&tid, &attr, pthread_fun, NULL);
	if (ret != 0) {
		fprintf(stderr, "thread create:error:%s\n", strerror(ret));
		exit(1);
	}
	ret = pthread_join(tid, NULL);
	if (ret != 0) {
		fprintf(stderr, "thread join:error:%s\n", strerror(ret));
		exit(1);
	}
	printf("pthread_join ret=%d\n", ret);
	printf("in main2:pthread id=%lu,pid=%d\n", pthread_self(), getpid());
	sleep(1);
	return 0;
}
// gcc pthread_create.c -lpthread
//ps aux 查看进程
//ps -Lf 加进程pid 查看制定pid的线程