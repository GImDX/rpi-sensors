#include <linux/i2c.h>

#ifndef _MPM3801_H_
#define _MPM3801_H_

#define _MPM3801_I2C_ADDRESS 0x28

typedef struct {
    unsigned char Raw[32];
    float Pressure;
    float Temperature;
} MPM3801_Data;

// Calculate pressure in Pa
MPM3801_Data ReadData(void);

#endif
