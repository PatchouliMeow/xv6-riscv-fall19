#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/fcntl.h"

#define MAX_CMD 100

char cmd[MAX_CMD];
char data[MAXARG][MAX_CMD];

//去除两侧空格
void trimcmd(char* str)
{
    int len = strlen(str);
    int i = 0, j = 0;
    while (i < len && str[i] == ' ')
        i++;
    
    while (i < len)
        str[j++] = str[i++];
    
    while (str[j-1] == ' ')
        j--;
    
    str[j] = '\0';
}

void splitcmd(char* src, int* size, char* dst)
{
    trimcmd(src);
    int siz = 0;
    int len = strlen(src);
    int l = 0;
    for(int i = 0; i < len; i++)
    {
        if(src[i] == ' ' && src[i-1] != ' ')
        {
            dst[siz*MAX_CMD+l] = '\0';
            siz++;
            l = 0;
        }
        else
        {
            dst[siz*MAX_CMD+l] = src[i];
            l++;
        }
    }
    siz++;
    *size = siz;
}

void runcmd(char* cmd)
{
    for(int i = 0; i < strlen(cmd); i++)
    {
        if(cmd[i] == '|')
        {
            cmd[i] = '\0';
            int pd[2];
            pipe(pd);
            if(fork() == 0)
            {
                //关闭自己的标准输出，执行左边的命令
                close(1);
                dup(pd[1]);
                close(pd[0]);
                close(pd[1]);
            }
            else
            {         
                //关闭自己的标准输入，执行右边的命令       
                close(0);
                dup(pd[0]);
                close(pd[0]);
                close(pd[1]);
                cmd = cmd + i + 1;
            }
            wait(0);
            break;
        }
    }

    int num;
    splitcmd(cmd, &num, (char*)data);
    int count = 0;
    for(int i = 1; i < num; i++)
    {
        //输出重定向
        if(data[i][0] == '>')
        {
            int fd = open(data[i+1], O_CREATE | O_WRONLY);
            close(1);
            dup(fd);
            count++;
            close(fd);
        }
        //输入重定向
        else if(data[i][0] == '<')
        {
            int fd = open(data[i+1], O_RDONLY);
            close(0);
            dup(fd);
            count++;
            close(fd);
        }
    }
    char *argv[MAX_CMD];
    int siz = num - count * 2;
    for(int i = 0; i < siz; i++)
        argv[i] = data[i];

    argv[siz] = 0;
    exec(data[0], argv);
}

int getcmd(char* buf)
{
    printf("@ ");
    int len = 0;
    char c;
    //读取一行指令
    while(read(0, &c, 1))
    {
        if (c == '\n')
            break;
        buf[len++] = c;
    }
    buf[len] = '\0';
    return len;
}

void main(int argc, char *argv[])
{
    static char buf[MAX_CMD];
    while(getcmd(buf) > 0)
    {
        if(fork() == 0)
            runcmd(buf);
        
        wait(0);
    }
    exit(0);
}