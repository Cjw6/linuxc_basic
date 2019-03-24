//本地套接字通信
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<string.h>
#include<sys/un.h>
int main(int argc,char**argv)
{
	unlink("server.socket");//删除连接文件
	int lfd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (lfd == -1)
	{
		perror("socket error");
		exit(1);
	}
	struct sockaddr_un servaddr;
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path, "server.socket");
	int ret = bind(lfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (ret == -1)
	{
		perror("bind error");
		exit(1);
	}
	ret = listen(lfd, 128);
	if (ret == -1)
	{
		perror("listen error");
		exit(1);
	}
	struct sockaddr_un cliaddr;
	socklen_t clilen = sizeof(cliaddr);
	int cfd = accept(lfd, (struct sockaddr*)&cliaddr,&clilen);
	if (cfd == -1)
	{
		perror("accept error");
		exit(1);
	}
	char buf[BUFSIZ] = { 0 };
	printf("client bind file:%s\n", cliaddr.sun_path);
	while (1)
	{
		int recvlen = recv(cfd, buf, BUFSIZ, 0);
		if (recvlen < 0)
		{
			perror("recv error");
			exit(1);
		}
		else if (recvlen==-1)
		{
			printf("client disconnect ......\n");
			close(cfd);
			break;
		}
		else
		{
			printf("recv buf:%s\n", buf);
			send(cfd, buf,recvlen, 0);
		}
	}
	close(lfd);
	return 0;
}