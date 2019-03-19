#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#define size 100
void sys_err(char *str)
{
	perror(str);
	exit(1);
}
void *pthread_fun(void *arg)
{
	//printf("in pthread:pthread id=%lu,pid=%d\n", pthread_self(), getpid());
	printf("thread is running\n");
	return NULL;
}
int main(int argc, char const *argv[])
{
	printf("in main1:pthread id=%lu,pid=%d\n", pthread_self(), getpid());
	pthread_t tid;
	pthread_attr_t attr;
	size_t stacksize;
	void* stackaddr;
	int err,detatchstat;
	int ret = pthread_attr_init(&attr);
	pthread_attr_getstack(&attr, &stackaddr, &stacksize);//���Ĭ��ջ�ռ��ַ��ջ�ռ��С
	pthread_attr_getdetachstate(&attr,&detatchstat);//���Ĭ�Ϸ���״̬
	if (detatchstat == PTHREAD_CREATE_DETACHED) {
		printf("PTHREAD_CREATE_DETACHED\n");
	}
	else if(detatchstat == PTHREAD_CREATE_JOINABLE) {
		printf("PTHREAD_CREATE_JOINABLE\n");
	}
	else {
		printf("unknow\n");
	}
	if (ret != 0) {
		fprintf(stderr, "thread_init:error:%s\n", strerror(ret));//�������ת��Ϊ�ַ�������
		exit(1);
	}
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//ʹ��atr���������߳�����Ϊ����̬
	stackaddr = maloc(size);
	stacksize = size;
	pthread_attr_setstack(&attr, stackaddr, stacksize);//���ô������̵߳Ķ�ջ
	ret = pthread_create(&tid, &attr, pthread_fun, NULL);//�����߳�
	if (ret != 0) {
		fprintf(stderr, "thread create:error:%s\n", strerror(ret));
		exit(1);
	}
	pthread_attr_destroy(&attr);
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
//ps aux �鿴����
//ps -Lf �ӽ���pid �鿴�ƶ�pid���߳�