#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>
#include <event2/event.h>
#include<time.h>
void read_fifo_fun(evutil_socket_t fd, short event, void *arg)
{
    struct event *ev = arg;
    char buf[BUFSIZ];
    int len=read(fd,buf,BUFSIZ-1);
    if(len==-1)
    {
        perror("read");
        event_del(ev);
        event_base_loopbreak(event_get_base(ev));    
    }
    else if(len>0)
    {
        buf[len]='\0';
        printf("%s\n",buf);
    }
}
int main()
{
    char *fifo="my_fifo";
    int fd=open(fifo,O_RDONLY);
    if(fd<0)
    {
        perror("open error");
    }
    struct event_base *base_event;
    base_event=event_base_new();//创建事件库
    if(base_event==NULL)
    {
        perror("base event new error");
        exit(1);
    }
    struct event *read_fifo;
    read_fifo=event_new(base_event,fd,EV_READ|EV_PERSIST,read_fifo_fun,event_self_cbarg());//创建新的事件
    if(read_fifo==NULL)
    {
        perror("event new error");
        exit(1);
    }
    struct timeval tval;
    tval.tv_sec=1;
    tval.tv_usec=0;
    event_add(read_fifo,&tval);//添加事件
    event_base_dispatch(base_event);//开始事件库循环
    event_free(read_fifo);
    event_base_free(base_event);
    close(fd);//关闭文件描述符
    libevent_global_shutdown();//退出libevent
    return 0;
}