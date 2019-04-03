//使用fread fwrite 函数完成复制文件
#include<stdio.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>
int main()
{
    FILE *fp=fopen("test.txt","r+");
    char buf[BUFSIZ]={0}; 
    int len=fread(buf,1,BUFSIZ,fp);
    fclose(fp);
    printf("%s\n",buf);
    fp=fopen("des.txt","w+");
    fwrite(buf,1,len,fp);
    fclose(fp);
    return 0;
}