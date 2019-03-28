/*#########################################################################

					v2.0 将地址修改，去除缓冲区禁用写  可以正常工作了

	测试笔记：nc需要使用标准的ip地址 如ipv4下：127.0.0.1
####################################################################
*/


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
conn_readcb(struct bufferevent *bev, void *user_data)
{
	char buf[BUFSIZ]={0};
    bufferevent_read(bev,buf,sizeof(buf)-1);
    printf("recv buf:%s",buf);
    char *str="server received\n";
    bufferevent_write(bev,str,strlen(str)+1);
    printf("已经发送\n");
}
//写缓冲区调用函数
void
conn_writecb(struct bufferevent *bev, void *user_data)
{
	printf("conn_writecb is running\n");
}
//事件调度函数
void
conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");//断开连接
	} else if (events & BEV_EVENT_ERROR) {//缓冲事件错误
		printf("Got an error on the connection: %s\n",
		    strerror(errno));/*XXX win32*/
	}
	/* None of the other events can happen here, since we haven't enabled
	 * timeouts 这里没有其他事件发生，因为我们没有启用超时*/
	bufferevent_free(bev);
}
static void
listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base *base = user_data;//新的事件库 用户数据
	struct bufferevent *bev;//缓冲事件

	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);//新的缓冲事件套接字
	//事件库 文件描述符 释放链接监听器释放底层套接字
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);
		return;
	}
	//设置读写事件回调函数
	bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, NULL);
	//设置带缓冲区的回调函数
	bufferevent_enable(bev, EV_READ);
	//bufferevent_disable(bev, EV_WRITE);
	//使能写，禁用读
}
int main()
{
    struct event_base* base;
    base=event_base_new();
    struct evconnlistener * listener=NULL;
    struct sockaddr_in servaddr;
    memset(&servaddr,0,sizeof(servaddr));
    inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr.s_addr);
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(6666);
	listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
	    LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
	    (struct sockaddr*)&servaddr,
	    sizeof(servaddr));  
    event_base_dispatch(base);
    evconnlistener_free(listener);//清理监听
    event_base_free(base);
    return 0;
}