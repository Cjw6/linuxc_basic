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
#define server_port 6666
#define server_ip "127.242.35.4"

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

	fd_set rset_tmp, rset;
	FD_ZERO(&rset_tmp);
	FD_SET(lfd, &rset_tmp);
	struct timeval timev;
	timev.tv_sec = 1;
	timev.tv_usec = 0;
	int count, max_fd = lfd;
	int i;
	while (1)
	{
		rset = rset_tmp;
		count = select(max_fd + 1, &rset, NULL, NULL, NULL);
		if (FD_ISSET(lfd, &rset))
		{
			socklen_t clilen = sizeof(cliaddr);
			cfd = accept(lfd, (struct sockaddr*)&cliaddr, &clilen);
			printf("new connect\n");
			FD_SET(cfd, &rset_tmp);
			max_fd = max_fd > cfd ? max_fd : cfd;
		}
		for (i = lfd+1; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &rset))
			{
				n = recv(i, buf, sizeof(buf), 0);
				
				if (n == 0)
				{
					printf("disconnect\n");
					close(i);
					FD_CLR(i, &rset_tmp);
				}
				else if (n > 0)
				{
					printf("recv buf:%s\n", buf);
					send(i, buf, strlen(buf)+1, 0);
				}
			}
		}
	}
}