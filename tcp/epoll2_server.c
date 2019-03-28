#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<string.h>
#include<errno.h>
#include<sys/epoll.h>
#include<fcntl.h>
#define server_port 6666
#define server_ip "127.242.35.4"
//边缘非阻塞模式
int main()
{
	struct sockaddr_in servaddr, cliaddr;
	int lfd, cfd, n;
	char buf[BUFSIZ];
	bzero(buf, sizeof(buf));
	lfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(server_port);
	inet_pton(lfd, server_ip, &servaddr.sin_addr.s_addr);
	bind(lfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	listen(lfd, 128);
	socklen_t clilen = sizeof(cliaddr);
	//创建epoll的根节点 
	int epfd = epoll_create(128);
	//初始化epoll树 将监听的文件描述符挂到树上面
	struct epoll_event ev;
	ev.data.fd = lfd;
	ev.events = EPOLLIN;
	epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
	struct epoll_event all[128];
	int i;
	while (1)
	{
		//使用epoll通知内核fd,文件io检测
		int ret = epoll_wait(epfd, all, sizeof(all) / sizeof(struct epoll_event), -1);//设置-1 为阻塞
		//遍历all数组的前ret个元素m
		for ( i = 0; i < ret; i++)
		{
			int fd = all[i].data.fd;
			if (fd == lfd)
			{
				//接受连接请求
				int cfd = accept(lfd,(struct sockaddr*)&cliaddr, &clilen);
				printf("new connect\n");
				if (cfd == -1)
				{
					perror("accept error");
					exit(1);
				}
				//设置文件描述符为非阻塞
				int flag = fcntl(cfd, F_GETFL);
				flag |= O_NONBLOCK;
				fcntl(cfd, F_SETFL, flag);
				//将得到的cfd挂到树上
				struct epoll_event temp;
				//temp.events = EPOLLIN;//水平触发 【只要缓冲区有数据 epoll_wait 就返回 频繁调用epoll——wait 系统开销大
				temp.events = EPOLLIN|EPOLLET;//边缘触发 不能保证缓冲区的数据全部读完 发一次数据调用一次epollwait 
				temp.data.fd = cfd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &temp);
			}
			else
			{
				//处理已经连接的客户端发过来的数据
				if (!all[i].events&EPOLLIN)
				{
					continue;
				}
				//读数据
				int sockfd = all[i].data.fd;
				//循环读数据
				int len;
				while ((len = recv(sockfd, buf, sizeof(buf), 0)) > 0)
				{
					printf("recv data:");
					//打印数据到终端
					write(STDOUT_FILENO, buf, strlen(buf));
					//s数据发送到客户端
					send(sockfd, buf, len, 0);
				}
				//强行读一个没有数据的缓冲区 errno会被设置为EAGIN
				if (len == 0)
				{
					printf("client disconnect\n");
					//sockfd 从树上删除
					epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
					close(sockfd);
				}
				else if (len == -1)
				{
					if (errno == EAGAIN)
					{
						printf("buf read finish\n");
					}
					else
					{
						perror("recv error");
						exit(1);
					}
				}
			}
		}
	}
}