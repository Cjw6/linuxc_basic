#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>
#include <event2/event.h>
#include<time.h>
#include<string.h>
void write_fifo_fun(evutil_socket_t fd, short event, void *arg)
{
    static int num=0;
    struct event *ev = arg;
    char buf[BUFSIZ];
    sprintf(buf,"test fifo event:%d",num++);
    int len=write(fd,buf,strlen(buf)+1);
    if(len<=0)
    {
        if(len==-1)
        {
            perror("write error");
            event_del(ev);
            event_base_loopbreak(event_get_base(ev));
        }
        if(len==0)
            printf("write nothing\n");
    }
    sleep(1);
}
int main()
{
    char *fifo="my_fifo";
    unlink(fifo);
    int ret=mkfifo(fifo,0777);
    if(ret<0)
    {
        perror("mkfifo error");
        exit(1);
    }
    int fd=open(fifo,O_WRONLY);
    if(fd<0)
    {
        perror("open error");
        exit(1);
    }
    struct event_base *base_event;
    base_event=event_base_new();
    if(base_event==NULL)
    {
        perror("base event new error");
        exit(1);
    }
    struct event *writefifo;
    writefifo=event_new(base_event,fd,EV_READ|EV_PERSIST,write_fifo_fun,event_self_cbarg());
    if(writefifo==NULL)
    {
        perror("event new error");
        exit(1);
    }
    struct timeval tval;
    tval.tv_sec=1;
    tval.tv_usec=0;
    event_add(writefifo,&tval);
    event_base_dispatch(base_event);
    event_free(writefifo);
    event_base_free(base_event);
    close(fd);
    libevent_global_shutdown();
    return 0;
}