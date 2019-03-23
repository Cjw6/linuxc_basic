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
	int cfd, lfd,n;
	char buf[BUFSIZ];
	cfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in cliaddr;
	bzero(&cliaddr, sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(server_port);
	inet_pton(cfd, server_ip, &cliaddr.sin_addr.s_addr);
	lfd = connect(cfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
	printf("connect server success\n");
	while (1) {
		fgets(buf, BUFSIZ, stdin);
		//buf[strlen(buf) + 1] = '\0';
		n=write(cfd, buf, strlen(buf)+1);
		n = read(cfd, buf, n);
		write(STDOUT_FILENO, buf, n);
	}
}
