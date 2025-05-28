#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "MPM3801.h"

int main(int argc, char *argv[])
{
    struct timeval start;
    struct timeval end;
    while (1)
    {
        gettimeofday(&start, NULL);
        ReadData();
        usleep(100);
        gettimeofday(&end, NULL);
    }

    return 0;
}