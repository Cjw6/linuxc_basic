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
#include<sys/time.h>
#include<poll.h>

#define server_port 6666
#define server_ip "127.242.35.4"
#define maxclient_count 128

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
	struct pollfd clientfd[maxclient_count];
	int i,count,cfd_count=0;
	for (i = 1; i < maxclient_count; i++)
	{
		clientfd[i].fd = -1;
	}
	clientfd[0].fd = lfd;
	clientfd[0].events = POLLRDNORM;
	while (1)
	{
		count = poll(clientfd, cfd_count + 1, 0);
		if (clientfd[0].revents&POLLRDNORM)
		{
			socklen_t clilen = sizeof(cliaddr);
			cfd = accept(lfd, (struct sockaddr*)&cliaddr, &clilen);
			printf("new connect\n");
			for (i = 1; i < maxclient_count; i++)
			{
				if (clientfd[i].fd<0)
				{
					clientfd[i].fd = cfd;
					break;
				}
			}
			if (cfd_count == maxclient_count)
				printf("server is full\n");
			else
				clientfd[i].events = POLLRDNORM;
			cfd_count = cfd_count > i ? cfd_count : i;
		}
		for (i = 1; i <= cfd_count; i++)
		{
			if (clientfd[i].fd < 0)
				continue;
			if (clientfd[i].revents&(POLLERR | POLLRDNORM))
			{
				int sockfd = clientfd[i].fd;
				n = recv(sockfd, buf, sizeof(buf), 0);

				if (n == 0)
				{
					printf("disconnect\n");
					close(sockfd);
					clientfd[i].fd = -1;
				}
				else if (n > 0)
				{
					printf("recv buf:%s\n", buf);
					send(sockfd, buf, strlen(buf) + 1, 0);
				}
			}

		}
	}
}