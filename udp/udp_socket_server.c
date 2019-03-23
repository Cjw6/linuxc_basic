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
	struct sockaddr_in servaddr,cliaddr;
	int lfd, cfd,n;
	char buf[BUFSIZ];
	//lfd = socket(AF_INET, SOCK_STREAM, 0);  tcp
	lfd = socket(AF_INET, SOCK_DGRAM, 0); //udp
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(server_port);
	inet_pton(lfd, server_ip, &servaddr.sin_addr.s_addr);
	bind(lfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	socklen_t clilen = sizeof(cliaddr);
	//listen(lfd, 128);                            udp不需要
	//cfd = accept(lfd,(struct sockaddr*)&cliaddr, &clilen);  在udp中不需要
	char  ip_str[100];
	while (1) {
#if 0
		n=read(cfd, buf, BUFSIZ);
		for (int i = 0; i < n; i++) {
			buf[i] = toupper(buf[i]);
		}
		write(cfd, buf, n);
#endif 
		//使用recvfrom sendto
		n = recvfrom(lfd, buf, BUFSIZ, 0, (struct sockaddr*)&cliaddr, &clilen);
		//printf("recv buf:%s", buf);  在udp中这个会出现死循环 
		printf("new connect ip:%s,port:%d\n", 
			inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip_str, sizeof(ip_str)), ntohs(cliaddr.sin_port));
		sendto(lfd, buf, n+1, 0, (struct sockaddr*)&cliaddr, clilen);
	}
	close(lfd);
}
//