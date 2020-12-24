#include "kernel/types.h"
#include "user/user.h"

void func(int *input, int num)
{
    printf("prime %d\n", *input);
	if(num == 1)
        return;
    
	int p[2], i, temp;
	int prime = *input;
	pipe(p);
    for(i = 0; i < num; i++)
    {
        temp = *(input + i);
        write(p[1], (char *)(&temp), 4);
    }
	close(p[1]);

	if(fork() == 0)
    {
		int counter = 0;
		char buffer[4];
		while(read(p[0], buffer, 4) != 0)
        {
			temp = *((int *)buffer);
			if(temp % prime != 0)
            {
				*input = temp;
				input++;
				counter++;
			}
		}
		func(input - counter, counter);
		exit();
    }
    //确保子进程先退出，父进程再退出
    wait();
}

int main(int argc, char *argv[])
{
    int input[34], i;
    for(i = 0; i < 34; i++)
    {
        input[i] = i+2;
	}
    func(input, 34);
    exit();
}