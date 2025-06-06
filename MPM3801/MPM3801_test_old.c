#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

#define FS 2000000.0
#define ZP 0.0

int main(int argc, char *argv[])
{
    unsigned int D1;
    unsigned int D2;
    unsigned int D3;
    unsigned int D4;
    unsigned int Data;
    unsigned int Data1;
    unsigned int Data2;
    unsigned int Data3;
    unsigned int Data4;

    float D_P;
    float D_T;

    FILE *fstream = NULL;
    char buff[32];
    memset(buff, 0, sizeof(buff));

    unsigned char hex[] = {0x3d, 0x81};
    short *flt = (short*)hex;

    // while (1)
    // {
    //     popen("i2ctransfer -y 1 r4@0x28", "r");
    // }

    while (1)
    {
        if (NULL == (fstream = popen("i2ctransfer -y 1 r4@0x28", "r")))
        {
            fprintf(stderr, "execute command failed: %s", strerror(errno));
            return -1;
        }

        while (NULL != fgets(buff, sizeof(buff), fstream))
        {
            printf("%s", buff);
            if (buff[0] == '0')
            {
                sscanf(buff, "%x %x %x %x", &D1, &D2, &D3, &D4);
                Data = D1 << 24 | D2 << 16 | D3 << 8 | D4;
                Data1 = Data >> 16;
                Data2 = Data1 & 0x3fff;
                D_P = (FS - ZP) * (Data2 - 1638.0) / 13108;
                printf("%.1f\n", D_P);

                Data3 = Data >> 5;
                Data4 = Data3 & 0x7ff;
                D_T = (Data4/2047.0) * 200 -50;
                printf("%.1f\n\n", D_T);
            }
        }
        pclose(fstream);
        usleep(1000000);
        }
    return 0;
}
// ————————————————
// 版权声明：本文为CSDN博主「阿基米东」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
// 原文链接：https://blog.csdn.net/lu_embedded/article/details/78669939

