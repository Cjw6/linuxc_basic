#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<string.h>
#include<signal.h>
#include<sys/wait.h>
#include<errno.h>
#include<pthread.h>
#define server_port 6666
#define server_ip "127.242.35.4"
typedef struct _Sockinfo{
	int cfd;
	pthread_t tid;
	struct sockaddr_in cliaddr;
}Sockinfo;
void* work(void *arg) 
{
	int n;
	Sockinfo *info = (Sockinfo*)arg;
	char buf[BUFSIZ];
	char ip_buf[64];
	printf("client ip:%s,port:%d\n",
		inet_ntop(AF_INET, &info->cliaddr.sin_addr.s_addr, ip_buf, sizeof(ip_buf)),
		ntohs(info->cliaddr.sin_port));
	while (1) {
		n = read(info->cfd, buf, BUFSIZ);
		if (n == 0) {
			printf("client disconnect\n");
			break;
		}
		for (int i = 0; i < n; i++) {
			buf[i] = toupper(buf[i]);
		}
		write(info->cfd, buf, n);
	}
	pthread_exit(NULL);
}
int main()
{
	struct sockaddr_in servaddr;
	int lfd, n;
	lfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(server_port);
	inet_pton(lfd, server_ip, &servaddr.sin_addr.s_addr);
	//设置端口复用
	int flag = 1;
	setsockopt(lfd, SOL_SOCKET,SO_REUSEADDR, &flag, sizeof(flag));

	bind(lfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	listen(lfd, 128);
	Sockinfo cliinfo[128];
	int i = 0;
	while (1)
	{
		printf("start new connect\n");
		socklen_t clilen = sizeof(struct sockaddr_in);
		//如果阻塞的时候被信号中断，返回时这个时候cfd=-1，errno=EINTR
		cliinfo[i].cfd= accept(lfd, (struct sockaddr*)&(cliinfo[i].cliaddr), &clilen);
		while (cliinfo[i].cfd == -1 && errno == EINTR) {
			cliinfo[i].cfd = accept(lfd, (struct sockaddr*)&(cliinfo[i].cliaddr), &clilen);
			if (cliinfo[i].cfd == -1 && errno != EINTR) {
				perror("accept error");
				exit(1);
			}
		}
		pthread_create(&cliinfo[i].tid, NULL, work, (void*)&cliinfo[i]);
		pthread_detach(cliinfo->tid);
		i++;
		if (i>127) {
			printf("sever is full\n");
			i = 127;
		}
	}
}
/*socket 建立客户端的程序
1、socket()
2、bind
3、listen
4、accept
5、执行服务器程序
对应的客户端是socket_server.c*/