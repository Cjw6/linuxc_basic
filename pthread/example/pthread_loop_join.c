#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
int var = 100;
void* tfn(void* arg)
{
	int i;
	i = (int)arg;
	sleep(i);
	if (i == 1) {
		var = 333;
		printf("var=%d\n", var);
		return (void*)&var;
	}
	else if (i == 3) {
		var = 777;
		printf("pthred%d,pthread_id=%lu,var=%d\n", i, pthread_self(), var);
		pthread_exit((void*)&var);
	}
	else
	{
		printf("pthred%d,pthread_id=%lu,var=%d\n", i, pthread_self(), var);
		pthread_exit((void*)&var);
	}
	return NULL;
}
int main()
{
	int i = 0;
	pthread_t tid[5];
	int *ret[5];
	for (i = 0; i < 5; i++) {
		pthread_create(&tid[i], NULL, tfn, (void*)i);
	}
	for (i = 0; i < 5; i++) {
		pthread_join(tid[i], (void**)&ret[i]);
		printf("------------------%d var=%d ret=%d\n", i, var,*(ret[i]));
	}
	printf("i'm main,var=%d", var);
	return 0;
}