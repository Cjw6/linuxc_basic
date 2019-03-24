//线程等待退出
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
typedef struct {
	int a;
	int b;
}exit_t;
void sys_err(char *str) {
	perror(str);
	exit(1);
}
void *tfn(void*arg)
{
	printf("thread is running\n");
	exit_t*ret = malloc(sizeof(exit_t));
	ret->a = 100;
	ret->b = 200;
	pthread_exit((void*)ret);
}
int main()
{
	exit_t *reval;
	pthread_t tid;
	int ret = pthread_create(&tid, NULL, tfn, NULL);
	if (ret < 0)
		sys_err("pthread_create error");
	pthread_join(tid, (void**)&reval);//tid线程id 回收线程退出的传出值  类似于wait函数
	printf("reval->a=%d\treval->b=%d\n", reval->a, reval->a);
	free(reval);
	printf("main finish\n");
	return 0;
}
