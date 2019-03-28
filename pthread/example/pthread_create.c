#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
void *pthread_fun(void *arg)
{
    printf("in pthread:pthread id=%lu,pid=%d\n",pthread_self(),getpid());
    return NULL;
}
int main(int argc, char const *argv[])
{
    printf("in main1:pthread id=%lu,pid=%d\n",pthread_self(),getpid());
    pthread_t tid;
    int ret=pthread_create(&tid,NULL,pthread_fun,NULL);
    if(ret!=0){
        fprintf(stderr,"thread create:error:%s\n",strerror(ret));//将错误号转变为字符串描述
        printf("pthread_create fail\n");
        exit(1);
    }
    printf("in main2:pthread id=%lu,pid=%d\n",pthread_self(),getpid());
    sleep(1);
    return 0;
}
// gcc pthread_create.c -lpthread
//ps aux 查看进程
//ps -Lf 加进程pid 查看制定pid的线程