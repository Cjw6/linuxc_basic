/*##################################################################
            v2.0 错误解决 在read-cb函数中不应继续向bufferevet中写
#################################################################*/
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<event2/event.h>
#include<event2/bufferevent.h>
#include<event2/listener.h>
//读
void
read_cb(struct bufferevent *bev, void *user_data)
{
    char buf[BUFSIZ]={0};
    int len=bufferevent_read(bev,buf,sizeof(buf));
    printf("recv buf:%s",buf);
   // bufferevent_write(bev,buf,len); 不需要再向缓冲区写
    //printf("数据已经被发送\n");  
}

//写缓冲区调用函数
void
write_cb(struct bufferevent *bev, void *user_data)
{
	printf("write_cb\n");
}
//事件调度函数
void
event_cb(struct bufferevent *bev, short events, void *user_data)
{
    if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");//断开连接
	} else if (events & BEV_EVENT_ERROR) {//缓冲事件错误
		printf("Got an error on the connection: %s\n",strerror(errno));/*XXX win32*/
	}else if(events&BEV_EVENT_CONNECTED){
        printf("connnect server\n");
        return ;
    }
	/* None of the other events can happen here, since we haven't enabled
	 * timeouts 这里没有其他事件发生，因为我们没有启用超时*/
	bufferevent_free(bev);
}
void read_terminal(int fd,short what,void*arg)
{
    char buf[BUFSIZ];
    int len=read(fd,buf,sizeof(buf));
    struct bufferevent*bev=(struct bufferevent*)arg;
    bufferevent_write(bev,buf,len+1);
}
int main(int argc, char const *argv[])
{
    struct event_base * base;
    base =event_base_new();
    int fd=socket(AF_INET,SOCK_STREAM,0);
        struct sockaddr_in servaddr;
    inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr.s_addr);
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(6666);
    struct bufferevent* bev=bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);
    bufferevent_socket_connect(bev,(struct sockaddr*)&servaddr,sizeof(servaddr));
    bufferevent_setcb(bev,read_cb,write_cb,event_cb,NULL);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);
    struct event* ev=event_new(base,STDIN_FILENO,EV_READ|EV_PERSIST,read_terminal,(void*)bev);
    event_add(ev,NULL);
    event_base_dispatch(base);
    event_base_free(base);
    return 0;
}
