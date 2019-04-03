//使用read write 函数完成复制文件
#include<stdio.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>
int main(int argc, char const *argv[])
{
    int fd=open("test.txt",O_CREAT|O_RDWR,0777);
    char buf[BUFSIZ];
    int ret=read(fd,buf,BUFSIZ);
    close(fd);
    fd=open("des.txt",O_RDWR|O_CREAT,0777);
    ret=write(fd,buf,strlen(buf));
    ret=read(fd,buf,BUFSIZ);
    printf("%s\n",buf);
    return 0;
}
