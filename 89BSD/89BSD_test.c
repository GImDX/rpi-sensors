#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "89BSD.h"

// Dump char array in HEX
void print_hex_memory(void *mem)
{
    int i;
    unsigned char *p = (unsigned char *)mem;
    for (i = 0; i < 16; i++)
    {
        printf("0x%02x ", p[i]);
        if (((i + 1) % 2 == 0) && i)
            printf("\n");
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    double Temperature = 0;
    double Pressure = 0;
    struct timeval start;
    struct timeval end;
    reset();
    usleep(5000);
    readCalibration();
    print_hex_memory(cal.PROM);
    printf("C0 = %d \n", cal.C0);
    printf("C1 = %d \n", cal.C1);
    printf("C2 = %d \n", cal.C2);
    printf("C3 = %d \n", cal.C3);
    printf("C4 = %d \n", cal.C4);
    printf("C5 = %d \n", cal.C5);
    printf("C6 = %d \n", cal.C6);
    printf("A0 = %d \n", cal.A0);
    printf("A1 = %d \n", cal.A1);
    printf("A2 = %d \n", cal.A2);

    while (1)
    {
        gettimeofday(&start,NULL);
        Temperature = getTemperature();
        Pressure = getPressure();
        // printf("Temperature\t%0.3f C\t", getTemperature());
        // printf("Pressure\t%1.6f MPa\n", getPressure() / 1000000.0);
        usleep(1);
        gettimeofday(&end,NULL);
    }

    return 0;
}
