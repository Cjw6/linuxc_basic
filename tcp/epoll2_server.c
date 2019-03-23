#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<string.h>
#include<errno.h>
#include<sys/epoll.h>
#include<fcntl.h>
#define server_port 6666
#define server_ip "127.242.35.4"
//��Ե������ģʽ
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
	socklen_t clilen = sizeof(cliaddr);
	//����epoll�ĸ��ڵ� 
	int epfd = epoll_create(128);
	//��ʼ��epoll�� ���������ļ��������ҵ�������
	struct epoll_event ev;
	ev.data.fd = lfd;
	ev.events = EPOLLIN;
	epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
	struct epoll_event all[128];
	int i;
	while (1)
	{
		//ʹ��epoll֪ͨ�ں�fd,�ļ�io���
		int ret = epoll_wait(epfd, all, sizeof(all) / sizeof(struct epoll_event), -1);//����-1 Ϊ����
		//����all�����ǰret��Ԫ��m
		for ( i = 0; i < ret; i++)
		{
			int fd = all[i].data.fd;
			if (fd == lfd)
			{
				//������������
				int cfd = accept(lfd,(struct sockaddr*)&cliaddr, &clilen);
				printf("new connect\n");
				if (cfd == -1)
				{
					perror("accept error");
					exit(1);
				}
				//�����ļ�������Ϊ������
				int flag = fcntl(cfd, F_GETFL);
				flag |= O_NONBLOCK;
				fcntl(cfd, F_SETFL, flag);
				//���õ���cfd�ҵ�����
				struct epoll_event temp;
				//temp.events = EPOLLIN;//ˮƽ���� ��ֻҪ������������ epoll_wait �ͷ��� Ƶ������epoll����wait ϵͳ������
				temp.events = EPOLLIN|EPOLLET;//��Ե���� ���ܱ�֤������������ȫ������ ��һ�����ݵ���һ��epollwait 
				temp.data.fd = cfd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &temp);
			}
			else
			{
				//�����Ѿ����ӵĿͻ��˷�����������
				if (!all[i].events&EPOLLIN)
				{
					continue;
				}
				//������
				int sockfd = all[i].data.fd;
				//ѭ��������
				int len;
				while ((len = recv(sockfd, buf, sizeof(buf), 0)) > 0)
				{
					printf("recv data:");
					//��ӡ���ݵ��ն�
					write(STDOUT_FILENO, buf, strlen(buf));
					//s���ݷ��͵��ͻ���
					send(sockfd, buf, len, 0);
				}
				//ǿ�ж�һ��û�����ݵĻ����� errno�ᱻ����ΪEAGIN
				if (len == 0)
				{
					printf("client disconnect\n");
					//sockfd ������ɾ��
					epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
					close(sockfd);
				}
				else if (len == -1)
				{
					if (errno == EAGAIN)
					{
						printf("buf read finish\n");
					}
					else
					{
						perror("recv error");
						exit(1);
					}
				}
			}
		}
	}
}