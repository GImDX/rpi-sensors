#ifndef  _89BSD_H_
#define _89BSD_H_

#define _89BSD_I2C_ADDRESS 0x77

struct calibrate
{
    // 128bit PROM
	unsigned int PROM[8];
    int C0;
    int C1;
    int C2;
    int C3;
    int C4;
    int C5;
    int C6;
    int A0;
    int A1;
    int A2;
} cal;

// Send Reset Command to 89BSD
void reset(void);

// Calbration 89BSD
void readCalibration(void);

// Calculate pressure in Pa
double getPressure(void);

// Calculate temperature in Celsius
double getTemperature(void);

#endif