#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
void *pthread_fun(void *arg)
{
    int i=(int)arg;//int i=*((int*)arg)
    sleep(i);
    printf("in pthread%d:pthread id=%lu,pid=%d\n",i,pthread_self(),getpid());
    return NULL;//返回到调用者处
	//exit(1) 退出进程
	//pthread_exit() 退出线程
}
int main(int argc, char const *argv[])
{
    printf("in main1:pthread id=%lu,pid=%d\n",pthread_self(),getpid());
    pthread_t tid;
    int i;
    //子线程不会创建子线程
    for(i=0;i<5;i++){
        int ret=pthread_create(&tid,NULL,pthread_fun,(void*)i);//(void*)&i
        if(ret!=0){
            fprintf(stderr,"thread create:error:%s\n",strerror(ret));//将错误号转变为字符串描述
            printf("pthread_create fail\n");
            exit(1);
        }
    }
   
    printf("in main2:pthread id=%lu,pid=%d\n",pthread_self(),getpid());
	pthread_exit(NULL);//线程退出 主控线程退出 ，只有所有子线程运行完才可以退出
	//return  返回到调用者
	//exit    将整个进程退出  在线程中不能使用exit函数
    return 0;
}
// gcc pthread_create.c -lpthread
//ps aux 查看进程
//ps -Lf 加进程pid 查看制定pid的线程