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
#define server_port 6666
#define server_ip "127.242.35.4"
void sigchild_handler(int signao) {
	pid_t pid;
	while ((pid=waitpid(-1, NULL, WNOHANG)) > 0)
		printf("child process wait\n");
	printf("sigchild_handler is running\n");
	return;
}
int main()
{
	struct sockaddr_in servaddr,cliaddr;
	int lfd, cfd, n;
	char buf[BUFSIZ];
	struct sigaction act;
	act.sa_flags = 0;
	act.sa_handler = sigchild_handler;
	sigemptyset(&act.sa_mask);
	sigaction(SIGCHLD, &act, NULL);
	lfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(server_port);
	inet_pton(lfd, server_ip, &servaddr.sin_addr.s_addr);
	bind(lfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	listen(lfd, 128);

	while (1)
	{
		printf("start new connect\n");
		socklen_t clilen = sizeof(cliaddr);
		//如果阻塞的时候被信号中断，返回时这个时候cfd=-1，errno=EINTR
		cfd = accept(lfd, (struct sockaddr*)&cliaddr, &clilen);
		while (cfd == -1 && errno == EINTR) {
			cfd = accept(lfd, (struct sockaddr*)&cliaddr, &clilen);
			if (cfd == -1 && errno != EINTR) {
				perror("accept error");
				exit(1);
			}
		}
		printf("client connect\n");
		pid_t pid = fork();
		if (pid < 0) {
			perror("fork error");
			exit(1);
		}
		else if (pid == 0) {
			char ip_buf[64];
			printf("client ip:%s,port:%d\n", 
				inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip_buf, sizeof(ip_buf)),
				ntohs(cliaddr.sin_port));
			while(1) {
				n = read(cfd, buf, BUFSIZ);
				if (n == 0) {
					printf("client disconnect\n");
					break;
				}
				for (int i = 0; i < n; i++) {
					buf[i] = toupper(buf[i]);
				}
				write(cfd, buf, n);
			}
			exit(1);
		}
		else {
			close(cfd);
			printf("connect success\n");
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