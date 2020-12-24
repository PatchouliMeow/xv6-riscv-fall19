#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int parent_fd[2], child_fd[2];
    pipe(parent_fd);
    pipe(child_fd);
    char buffer[8];

    if(fork() == 0)
    {
        read(parent_fd[0], buffer, 4);
        printf("%d: received %s\n", getpid(), buffer);
        write(child_fd[1], "pong", strlen("pong"));
    }
    else
    {
        write(parent_fd[1], "ping", strlen("ping"));
        read(child_fd[0], buffer, 4);
        printf("%d: received %s\n", getpid(), buffer);
    }

    exit();
}