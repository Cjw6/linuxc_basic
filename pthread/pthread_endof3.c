#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>

void* tfn1(void *arg)
{
	printf("thread1 is running\n");
	return (void*)111;
}
void* tfn2(void* arg)
{
	printf("thread2 is running\n");
	pthread_exit((void*)222);
}
void* tfn3(void* arg)
{
	while (1) {
		//printf("3 going to die in 3s\n");
		//sleep(1);
		pthread_testcancel();//自己添加取消点
	}
}
int main()
{
	pthread_t tid;
	void *tret=NULL;
	
	pthread_create(&tid, NULL, tfn1, (void*)tret);
	pthread_join(tid, (void**)&tret);
	printf("thread1 exit code=%d\n", (int)tret);

	pthread_create(&tid, NULL, tfn2, (void*)tret);
	pthread_join(tid, (void**)&tret);
	printf("thread2 exit code=%d\n", (int)tret);

	pthread_create(&tid, NULL, tfn3, (void*)tret);
	sleep(3);
	pthread_cancel(tid);
	pthread_join(tid, (void**)&tret);
	printf("thread3 exit code=%d\n", (int)tret);

	return 0;
}