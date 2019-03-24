#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<string.h>
#include<sys/un.h>
#include<signal.h>
int main(int argc ,char ** argv )
{
	unlink("client.socket");
	int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (fd == -1)
	{
		perror("socket error");
	}
    struct sigaction act;
	struct sockaddr_un clientaddr;
	clientaddr.sun_family = AF_LOCAL;
	strcpy(clientaddr.sun_path, "client.socket");
	int ret = bind(fd, (struct sockaddr*)&clientaddr, sizeof(clientaddr));
	if (ret == -1)
	{
		perror("bind error");
	}
	struct sockaddr_un servaddr;
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path, "server.socket");
	int lfd =connect(fd, (struct sockaddr*)&servaddr,sizeof(servaddr));
	if (lfd == -1)
	{
		perror("accept error");
	}
	char buf[BUFSIZ] = { 0 };
	while (1)
	{
		fgets(buf, BUFSIZ, stdin);
		send(fd, buf, strlen(buf) + 1, 0);
		recv(fd, buf, BUFSIZ, 0);
		printf("buf:%s", buf);
	}
	close(fd);
	return 0;
}
