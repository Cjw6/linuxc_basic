#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<string.h>
#define server_port 6666
#define server_ip "127.242.35.4"
int main()
{
	struct sockaddr_in servaddr;
	int lfd, cfd,n;
	char buf[BUFSIZ];
	lfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(server_port);
	inet_pton(lfd, server_ip, &servaddr.sin_addr.s_addr);
	bind(lfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	listen(lfd, 128);
	socklen_t servlen = sizeof(servaddr);
	cfd = accept(lfd,(struct sockaddr*)&servaddr, &servlen);
	while (1) {
		n=read(cfd, buf, BUFSIZ);
		for (int i = 0; i < n; i++) {
			buf[i] = toupper(buf[i]);
		}
		write(cfd, buf, n);
	}
	close(lfd);
}
/*socket 建立客户端的程序
1、socket()
2、bind
3、listen
4、accept
5、执行服务器程序
对应的客户端是socket_server.c*/