#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int i, j = 0;
    int k, m = 0;
    char block[32];
    char buf[32];
    char *p = buf;
    char *lineSplit[32];
    for(i = 1; i < argc; i++)
    {
        lineSplit[j] = argv[i];
        j++;
    }
    while((k = read(0, block, sizeof(block))) > 0)
    {
        for(i = 0; i < k; i++)
        {
            if(block[i] == '\n')
            {
                buf[m] = 0;
                m = 0;
                lineSplit[j] = p;
                j++;
                p = buf;
                lineSplit[j] = 0;
                j = argc - 1;
                if(fork() == 0)
                {
                    exec(argv[1], lineSplit);
                }                
                wait();
            }
            else if(block[i] == ' ')
            {
                buf[m] = 0;
                m++;
                lineSplit[j++] = p;
                p = &buf[m];
            }
            else
            {
                buf[m] = block[i];
                m++;
            }
        }
    }
    exit();
}