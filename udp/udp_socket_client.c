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
	cfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(server_port);
	inet_pton(cfd, server_ip, &servaddr.sin_addr.s_addr);
	//lfd = connect(cfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); udp²»ÐèÒª
	printf("goto while(1)\n");
	socklen_t clilen = sizeof(servaddr);
	while (1) {
		fgets(buf, BUFSIZ, stdin);
		//buf[strlen(buf) + 1] = '\0';
		n=sendto(cfd, buf, strlen(buf), 0, (struct sockaddr*)&servaddr, clilen);
		n = recvfrom(cfd, buf, n, 0, (struct sockaddr*)&servaddr, &clilen);
		write(STDOUT_FILENO, buf, n);
	}
	close(cfd);
}
