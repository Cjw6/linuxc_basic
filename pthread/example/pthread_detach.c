#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>
void* tfn(void *arg)
{
	int n = 3;
	while (n--) {
		printf("thread count=%d\n", n);
		sleep(1);
	}
	return (void*)1;
}
int main()
{
	pthread_t tid;
	void *tret;
	int err;

	pthread_create(&tid, NULL, tfn, NULL);
	pthread_detach(tid);//���̷߳���
	while (1)
	{
		err = pthread_join(tid, (void**)&tret);//�����ȴ����߳̽���
		printf("err=%d\n", err);
		if (err != 0) {
			fprintf(stderr, "thread_join error:%s\n", strerror(err));
		}
		else {
			fprintf(stderr, "thread exit code=%d\n", (int)tret);
		}
		sleep(1);
	}
	return 0;
}