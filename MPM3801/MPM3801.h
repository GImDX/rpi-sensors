#include <linux/i2c.h>


#ifndef _MPM3801_H_
#define _MPM3801_H_

#define _MPM3801_I2C_ADDRESS 0x28

// Pressure and Temperature data struct
struct MPM3801_Data
{
    __u8 Raw[27];
    double Pressure;
    double Temperature;
} datas;

// Calculate pressure in Pa
void ReadData(void);

#endif