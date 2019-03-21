#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdio.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <sys/time.h>
#include<string.h>
#define server_port 6666
#define server_ip "127.242.35.4"

int main()
{
	int cfd, lfd, n;
	char buf[BUFSIZ];
	cfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in cliaddr;
	bzero(&cliaddr, sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(server_port);
	inet_pton(cfd, server_ip, &cliaddr.sin_addr.s_addr);
	lfd = connect(cfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
	char ch = '1';
	char *str = "hello";
	//µÚÒ»´Î¶ÁÐ´
	n=write(cfd,str, strlen(str));
	read(cfd,buf, n);
	printf("pid:%d the first time: char from server = %s\n", getpid(),buf);
	printf("end\n");
	while (1);
	close(cfd);
	return 0;
}
