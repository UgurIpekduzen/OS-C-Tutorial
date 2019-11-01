#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>

/* 
 - To compile the code: gcc zamanOlc.c -o <output name> -lrt
 - To execute the output: ./<output name> <terminal command> <parameter>(opt.)
*/

unsigned long getTimeOfDayMS();
char* concatenate(const char *, const char *);

int main(int argc, char **argv)
{
    /*size of allocated shared memory*/
    const int SIZE = 4096;
    
    /* name of the shared memory object and name of binary folder*/
    const char *name = "OS", *binaryPath = "/bin/";

    /* shared memory file descriptor */
    int fd;
    
    /* pointer to shared memory object */
    char *ptr;

    pid_t pid;

    if (argc < 2)
    {
        fprintf(stderr, "usage:./<output name> <command> <parameter>(opt.)\n");
        return 1;
    }

    /* fork a child process */
    pid = fork();

    if (pid < 0)
    { /* error occurred */
        fprintf(stderr, "Fork Failed\n");
        return 1;
    }
    else if (pid == 0)
    { /* child process */
        
         /* create the shared memory object */
        fd = shm_open(name, O_CREAT | O_RDWR, 0666);
        
        /* configure the size of the shared memory object */
        ftruncate(fd, SIZE);
        
        /* memory map the shared memory object */
        ptr = (char *)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        
        /* write to the shared memory object */
        sprintf(ptr, "%lu", getTimeOfDayMS());

        /* concatenate the given arguments in a single string */
        char * path = concatenate(binaryPath, argv[1]);
        
        execv(path, argv + 1);
        /* remove string */
        free(path);

        fprintf(stderr,"execv() failed\n");
        exit(1);
    }
    else
    { /* parent process */
        
        /* parent will wait for the child to complete */
        wait(NULL);
        fd = shm_open(name, O_CREAT | O_RDWR, 0666);
        
        /* configure the size of the shared memory object */
        ftruncate(fd, SIZE);
        
        /* memory map the shared memory object */
        ptr = (char *)
            mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        /* calculate the elapsed time */
        unsigned long startTime = strtoul((char *)ptr, NULL, 10);
        unsigned long stopTime = getTimeOfDayMS();
        printf("Geçen süre %lu ms' dir\n", stopTime - startTime);
        
        /* remove the shared memory object */
        shm_unlink(name);

        return 0;
    }
}

unsigned long getTimeOfDayMS()
{
    /*tv_sec = seconds + tv_usec = microseconds*/
    struct timeval tv; 
    gettimeofday(&tv, NULL);
    return (tv.tv_usec /1000) + (tv.tv_sec * 1000);
}

char* concatenate(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

/*
 - Uğur İPEKDÜZEN
 - 150101045
*/
