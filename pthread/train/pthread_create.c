#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
void* thread_fun(void *arg)
{
    while(1)
    {
        static int num=0;
        printf("thread%d is running:%d\n",(int)pthread_self(),num++);
        sleep(1);
    }
    return NULL;
}
int main()
{
    pthread_t tid;
    printf("thread_id:%d\n",(int)pthread_self());
    printf("start create thread\n");
    int err=pthread_create(&tid,NULL,thread_fun,NULL);
   // pthread_detach(tid);
    if(err!=0)
    {
        fprintf(stderr,"pthread_create error:%s\n",strerror(err));
        exit(1);
    }
    while(1);
    return 0;
}