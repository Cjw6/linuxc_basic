#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>


struct test{
    int a;
    int b;
};
void* thread_fun1(void *arg)
{
    printf("thread%d is running\n",(int)pthread_self());
    return ((void*)1);
}
void* thread_fun2(void *arg)
{
    printf("thread%d is running\n",(int)pthread_self());
    pthread_exit((void*)2);
}
void* thread_fun3(void* arg)
{
    printf("thread%d is running\n",(int)pthread_self());
    struct test *p=arg;
    p->a=4;
   p->b=4;
    pthread_exit((void*)p);
}
int main()
{
    pthread_t tid1,tid2,tid3;
    struct test *Test=(struct test*)malloc(sizeof(struct test));
    memset(Test,0,sizeof(struct test));
    printf("thread_id:%d\n",(int)pthread_self());
    printf("start create thread\n");
    int err=pthread_create(&tid1,NULL,thread_fun1,NULL);
    if(err!=0)
    {
        fprintf(stderr,"pthread_create error:%s\n",strerror(err));
        exit(1);
    }
    err=pthread_create(&tid2,NULL,thread_fun2,NULL);
    if(err!=0)
    {
        fprintf(stderr,"pthread_create error:%s\n",strerror(err));
        exit(1);
    }
   err=pthread_create(&tid3,NULL,thread_fun3,Test);
    if(err!=0)
    {
        fprintf(stderr,"pthread_create error:%s\n",strerror(err));
        exit(1);
    }
    int *a1,*a2;
    sleep(2);
    pthread_join(tid1,(void**)&a1);
    pthread_join(tid2,(void**)&a2);
    pthread_join(tid3,(void**)(&Test));
    printf("a1=%d,a2=%d\n",(int)a1,(int)a2);
    printf("test a=%d,b=%d\n",Test->a,Test->b);
    while(1);
    return 0;
}
//线程的两种退出方式 return pthread_exit