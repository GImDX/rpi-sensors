#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <math.h>

#include <i2c/smbus.h>
#include "MPM3801.h"

#define PMIN 0.0
#define PMAX 2000000.0

/***I2C I/O operation functions***/

// Open a connection to the MPM3801
// Returns a file id
int begin(void)
{
        int fd = 0;
        char *fileName = "/dev/i2c-1";

        // Open port for reading and writing
        if ((fd = open(fileName, O_RDWR)) < 0)
        {
                exit(1);
        }

        // Set the port options and set the address of the device
        if (ioctl(fd, I2C_SLAVE, _MPM3801_I2C_ADDRESS) < 0)
        {
                close(fd);
                exit(1);
        }
        return fd;
}

// Read a block of data MPM3801 without specifying a device register
__u8 i2cReadBlock(int fd, __u8 length, __u8 *values)
{
        if (0 > i2c_smbus_read_i2c_block_data(fd, 0x28, length, values))
        {
                close(fd);
                exit(1);
        }
}

// Read bytes of data mpm3801 without specifying a device register
__u8 i2cReadBytes(int fd, __u8 length, __u8 *values)
{
        if (read(fd, values, length) != length)
        {
                close(fd);
                exit(1);
        }
}

/***Device functional operation function***/

// Read pressure and temperature data from MPM3801
// Value returned will be in units of Pa
MPM3801_Data ReadData(void)
{
        MPM3801_Data result;
        int fd = begin();
        //i2cReadBlock(fd, 32, result.Raw);
        i2cReadBytes(fd, 27, result.Raw);
        close(fd);
        result.Pressure = (PMAX - PMIN) * (((result.Raw[0] & 0x3F) << 8 | result.Raw[1]) - 1638) / 13108.0 + PMIN;
        result.Temperature = (result.Raw[2] << 3 | result.Raw[3] >> 5) / 2047.0 * 200 - 50;
        return result;
}
