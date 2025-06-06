#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "MPM3801.h"

int main(int argc, char *argv[])
{
    struct timeval start;
    struct timeval end;
    MPM3801_Data datas;
    while (1)
    {
        gettimeofday(&start, NULL);
        datas = ReadData();
        printf("Pressure=%.3f, Temp=%.3f ", datas.Pressure, datas.Temperature);
        printf("Raw: ");
        for (int i = 0; i < 32; i++) {
            printf("%02X ", datas.Raw[i]);
        }
        printf("\n");
        usleep(1000000);
        gettimeofday(&end, NULL);
    }

    return 0;
}

