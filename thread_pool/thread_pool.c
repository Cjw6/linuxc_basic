#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

typedef struct _Admin_thread admin_thread_t;
typedef struct _Work_thread work_thread_t;

typedef void* (pf_thread_fun)(void* arg);

//工作函数结构 存储需要添加的给线程池内线程工作的函数
typedef struct {
    void *(*function)(void *);          /* 函数指针，回调函数 */
    void *arg;                          /* 上面函数的参数 */
}workThreadFun;                    /* 各子线程任务结构体 */

//使用环形队列来储存任务队列
typedef struct _Circular_queue {
	int queue_size;//环形队列大小
	int rear;//环形队列尾
	int front;//环形队列头
	//pf_thread_fun *queue_arr;//试验中这个数组中没有办法放函数指针
    workThreadFun *taskArr;
}circularQueue;

struct _Work_thread{
    int tid;
    admin_thread_t *admin;
};

struct _Admin_thread{
    int  max_thread;//最大线程数
    int  min_thread;//最小线程数
    int alive_thread;//活着的线程数
    int busyThreadCount;
    int stepThreadCount;
    pthread_t admin_thread_tid;//管理者线程的tid
    pthread_t *thread_tid_arr;
    pthread_mutex_t work_thread_mutex_lock;
    int shutdown;//是否开启关闭线程池
    circularQueue* taskQueue;//任务环形队列
    pthread_cond_t has_task_cond;//发信号给工作信号可以接任务
    int exit_threadCount;
};

void* threadIdle(void *arg);

//任务队列函数
int queue_is_full(circularQueue* p)
{
	return (p->rear + 2) % (p->queue_size + 1) == p->front;
}

int queue_is_empty(circularQueue* p)
{
	return (p->rear + 1) % (p->queue_size + 1) == p->front;
}

void queue_push(workThreadFun value, circularQueue* p)
{
	if (queue_is_full(p))
	{
		printf("queue is full,don't push\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		exit(1);
	}
	p->rear = (p->rear + 1) % (p->queue_size + 1);
    p->taskArr[p->rear]=value;
}

workThreadFun queue_pop(circularQueue* p)
{
	if (queue_is_empty(p))
	{
		printf("queue is empty,don't pop\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        workThreadFun result;
        result.function=threadIdle;
        result.arg=NULL;
        return result;
	}
	workThreadFun result = p->taskArr[p->front];
    p->taskArr[p->front].function=threadIdle;//将环形队列中原先的函数变为空函数
	p->front = (p->front + 1) % (p->queue_size + 1);

	return result;
}

circularQueue* create_cirQueue(int size)//创建任务环形队列
{
	circularQueue* queue = malloc(sizeof(circularQueue));
	memset(queue, 0, sizeof(circularQueue));
	queue->queue_size = size;
	queue->taskArr = malloc((size + 1) * sizeof(workThreadFun));
	queue->rear = 0;
	queue->front = 1;
	return queue;
}

void destroy_cirQueue(circularQueue* p)
{
	free(p->taskArr);
	free(p);
}

//线程池工作函数
void* work_thread_fun(void *arg)
{
    admin_thread_t *admin=(admin_thread_t*) arg;//这个不可以传递空指针，这样会造成内存错误
    printf("%lu\n",pthread_self());
    while(!admin->shutdown)
    {
        pthread_mutex_lock(&admin->work_thread_mutex_lock);
        while((queue_is_empty(admin->taskQueue))&&(admin->exit_threadCount==0))
        {
            pthread_cond_wait(&admin->has_task_cond,&admin->work_thread_mutex_lock);
        }
        if(admin->shutdown==1)
        {
            printf("thread exit\n");
            admin->alive_thread--;
            pthread_mutex_unlock(&admin->work_thread_mutex_lock);
            pthread_exit(NULL);
        }
       //printf("try to recycle work thread exit_threadCount:%d\n",admin->exit_threadCount);
        if(admin->exit_threadCount>0)
        {
           printf("try to recycle work thread exit_threadCount:%d\n",admin->exit_threadCount);
            admin->exit_threadCount--;
            if(admin->alive_thread>admin->min_thread)
            {
                printf("recycle work thread exit_threadCount:%d\n",admin->exit_threadCount);
                admin->alive_thread--;
                pthread_mutex_unlock(&admin->work_thread_mutex_lock);
                pthread_exit(NULL);
            }
            
        }
        workThreadFun fun=queue_pop(admin->taskQueue);
        if(fun.function==threadIdle)
            continue;
        admin->busyThreadCount++;
        pthread_mutex_unlock(&admin->work_thread_mutex_lock);

        fun.function(fun.arg);        //工作线程开始工作

        pthread_mutex_lock(&admin->work_thread_mutex_lock);
        admin->busyThreadCount--;
        pthread_mutex_unlock(&admin->work_thread_mutex_lock);
    }
gotoExit:
    pthread_mutex_lock(&admin->work_thread_mutex_lock);
    admin->alive_thread--;
    pthread_mutex_unlock(&admin->work_thread_mutex_lock);
    pthread_exit(NULL);
}

void* adminThread(void *arg)
{
    admin_thread_t* admin=(admin_thread_t*)arg;
    pthread_mutex_t thread_lock =admin->work_thread_mutex_lock;
    //先初始化最小线程
    for(int i=0;i<admin->min_thread;i++)
    {
        pthread_create(&(admin->thread_tid_arr[i]),NULL,work_thread_fun,(void*)admin);
        pthread_mutex_lock(&thread_lock);
        admin->alive_thread++;
        pthread_mutex_unlock(&thread_lock);
    }
    while(!admin->shutdown)
    {
       sleep(1);
        if(!queue_is_empty(admin->taskQueue))
        {
            pthread_cond_signal(&admin->has_task_cond);   
        }
        //在什么时候创建新的多线程可以修改
        if((admin->busyThreadCount>0.8*admin->alive_thread)&&(admin->alive_thread<admin->max_thread))
        {
            //printf("add1 thread-admin\n");
            for(int i=0;i<admin->stepThreadCount;i++)
            {
                //printf("add2 thread-admin\n");
                for(int j=0;j<admin->max_thread;j++)//查找数组中的空值可以值得修改
                {
                    //printf("find tid arr\t");                    
                    if(admin->thread_tid_arr[j]==0)
                    {
                        //printf("\nfinded tid arr\n");
                        pthread_create(&admin->thread_tid_arr[j],NULL,work_thread_fun,(void *)admin);   
                        pthread_mutex_lock(&thread_lock);
                        admin->alive_thread++;
                        pthread_mutex_unlock(&thread_lock);
                        break;     
                    }
                }

            }
        }

        if(admin->busyThreadCount<0.7*admin->alive_thread)
        {
            pthread_mutex_lock(&thread_lock);
            admin->exit_threadCount=0.15*admin->alive_thread;
            pthread_mutex_unlock(&thread_lock);
            while(admin->exit_threadCount>0)
            {
                pthread_cond_signal(&admin->has_task_cond);
                //printf("recycle cond signal\n");
            }
        }

        if(admin->shutdown==1)
        {
            printf("shutdown*******************\n");
            pthread_cond_broadcast(&admin->has_task_cond);
            pthread_exit(NULL);
        }
#if 0
        for(int i=0;i<admin->max_thread;i++)
        {
            printf("ken :\t");
            printf("%lu \t",admin->thread_tid_arr[i]);
        }
#endif
        //printf("admin thread is running\n");
    }
    
}
void threadPool_shutdown(admin_thread_t *p)
{
    p->shutdown=1;
}
void add_thread_task(admin_thread_t *admin,pf_thread_fun thread_fun,void *arg)
{
    circularQueue* q=admin->taskQueue;
    workThreadFun work;
    work.function=thread_fun;
    work.arg=arg;
    queue_push(work,admin->taskQueue);
}

admin_thread_t* create_thread_pool(int maxThreadCount,int minThreadCount,int stepCount)
{
    admin_thread_t *admin=(admin_thread_t*)malloc(sizeof(admin_thread_t));
    if(admin==NULL)
    {
        printf("admin create fail\n");
        return 0;
    }
    __bzero(admin,sizeof(admin_thread_t));
    admin->max_thread=maxThreadCount;
    admin->min_thread=minThreadCount;
    admin->shutdown=0;
    admin->alive_thread=0;
    admin->stepThreadCount=stepCount;
    admin->exit_threadCount=0;
    admin->taskQueue=create_cirQueue(admin->max_thread);
    printf("create tasskQueue\n");

    admin->thread_tid_arr=malloc(admin->max_thread*sizeof(pthread_t));
    if(admin->thread_tid_arr==NULL)
    {
        printf("admin->tid array malloc fail\n");
        exit(1);
    }
    for(int i=0;i<admin->max_thread;i++)
    {
        admin->thread_tid_arr[i]=0;
    }

    pthread_cond_init(&admin->has_task_cond,NULL);
    printf("cond_init\n");
    pthread_mutex_init(&admin->work_thread_mutex_lock,NULL);
    printf("mutex init\n");
    //创建管理者线程
    pthread_create(&(admin->admin_thread_tid),NULL,adminThread,(void*)admin);
    return admin;
}

void destroy_thread_pool(admin_thread_t* admin)
{
    pthread_mutex_destroy(&admin->work_thread_mutex_lock);
    pthread_cond_destroy(&admin->has_task_cond);
    destroy_cirQueue(admin->taskQueue);
    free(admin->thread_tid_arr);
    free(admin);
}

void* threadIdle(void *arg)
{
    return NULL;
}
void* testThreadPool(void *arg)
{
    while(1)
    {
        printf("test thread-pool tid:%lu\n",pthread_self());
        sleep(1);
    }
}
void* testThreadPool2(void *arg)
{
    int n=6;
    while(n--)
    {
        printf("test thread-pool tid:%lu\n",pthread_self());
        sleep(1);
    }
}
void print_statistic(admin_thread_t *admin)
{
    printf("------------------------------statistic--------------------------------------------\n");
    printf("\t  busy thread count:%d,alive thread count:%d,exit thread count:%d\n\n",
        admin->busyThreadCount,admin->alive_thread,admin->exit_threadCount);
}
int main(int argc, char const *argv[])
{
    admin_thread_t *admin;
    admin=create_thread_pool(50,5,5);
    add_thread_task(admin,testThreadPool,NULL);
    add_thread_task(admin,testThreadPool,NULL);
    for(int i=0;i<0;i++)
    {
        add_thread_task(admin,testThreadPool,NULL);
    }
    for(int i=0;i<35;i++)
    {
        add_thread_task(admin,testThreadPool2,NULL);
    }
    int count=0;
    while(1)
    {
        sleep(1);
        printf("\n#################################%d####################################\n",count++);
        print_statistic(admin);
    }
    destroy_thread_pool(admin);
    return 0;
}


