#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

void clean(void *arg)
{
    printf("%s\n",(char*)arg);
}

void* thread_fun1(void *arg)
{
   // sleep(5);
    printf("thread%d is running\n",(int)pthread_self());
    pthread_cleanup_push(clean,"thread1 clenanup1"); //按照堆栈顺序进行对函数的操作
    pthread_cleanup_push(clean,"thread1 clenanup2");  
    pthread_cleanup_pop(1);  
    pthread_cleanup_pop(1);
    return ((void*)1);
}
void* thread_fun2(void *arg)//这个进程直接被cancel了
{
    sleep(5);
    printf("thread%d is running\n",(int)pthread_self());
    pthread_cleanup_push(clean,"thread2 clenanup1"); 
    pthread_cleanup_push(clean,"thread2 clenanup2");  
    pthread_cleanup_pop(1);  
    pthread_cleanup_pop(1);//设置为0 调用清理函数
    pthread_exit((void*)2);
}

int main()
{
    pthread_t tid1,tid2,tid3;
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
    pthread_cancel(tid2);//用来取消统一进程中的其他进程
    int *a1,*a2;
    sleep(2);
    pthread_join(tid1,(void**)&a1);
    pthread_join(tid2,(void**)&a2);
    printf("a1=%d,a2=%d\n",(int)a1,(int)a2);
    while(1);
    return 0;
}
//线程的两种退出方式 return pthread_exit