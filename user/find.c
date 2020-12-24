#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname(char *path)
{
    static char buf[DIRSIZ+1];
    char *p;

    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    memmove(buf, p, strlen(p)+1);
    return buf;
}

void match(char *fileName, char *findName)
{
	if(strcmp(fmtname(fileName), findName) == 0)
    {
		printf("%s\n", fileName);
	}
}

void find(char *path, char *name)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    if((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type)
    {
        case T_FILE:
            match(path, name);
            break;

        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
            {
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf+strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de))
            {
                if(de.inum == 0 || de.inum == 1 || strcmp(de.name, ".")==0 || strcmp(de.name, "..")==0)
                    continue;
                memmove(p, de.name, strlen(de.name));
                p[strlen(de.name)] = 0;

                find(buf, name);
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("error input\n");
        exit();
    }

    find(argv[1], argv[2]);
    exit();
}
