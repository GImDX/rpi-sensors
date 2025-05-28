#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <math.h>

#include <i2c/smbus.h>
#include "89BSD.h"

#define PMIN 0.0
#define PMAX 2800000.0

// oversampling setting(OSR)
// 0 OSR = 256
// 1 OSR = 512
// 2 OSR = 1024
// 3 OSR = 2048
// 4 OSR = 4096
#define _89BSD_OVERSAMPLING_SETTING 4

/***I2C I/O operation functions***/

// Open a connection to the 89BSD
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
	if (ioctl(fd, I2C_SLAVE, _89BSD_I2C_ADDRESS) < 0)
	{
		close(fd);
		exit(1);
	}
	return fd;
}

// Send a single byte to the 89BSD
void i2cWriteByte(int fd, __u8 address)
{
	if (0 > i2c_smbus_write_byte(fd, address))
	{
		close(fd);
		exit(1);
	}
}

// Read a WORD of data 89BSD
unsigned int i2cReadWordData(int fd, __u8 address)
{
	int word = i2c_smbus_read_word_data(fd, address);
	if (0 > word)
	{
		close(fd);
		exit(1);
	}
	else
	{
		return (unsigned int)word;
	}
}

// Read a block of data 89BSD
void i2cReadBlockData(int fd, __u8 address, __u8 length, __u8 *values)
{
	if (0 > i2c_smbus_read_i2c_block_data(fd, address, length, values))
	{
		close(fd);
		exit(1);
	}
}

/***Device functional operation function***/

// Send Reset Command to 89BSD
void reset(void)
{
	int fd = begin();
	i2cWriteByte(fd, 0x1E);
	close(fd);
}

// CRC4 code from TE 'AN520'
// Initial register 0x0000
// Left shift
// Polyminal 0x3000
unsigned char _89BSD_CRC4(unsigned int n_prom[])
{
	// Simple counter
	int cnt;
	// CRC reminder
	unsigned int n_rem;
	// Original value of the CRC
	unsigned int crc_read;
	unsigned char n_bit;
	unsigned char crc;
	n_rem = 0x0000;
	// Save read CRC
	crc_read = n_prom[7];
	// CRC byte is replaced by 0, save 4bit crc4
	crc = 0x000F & n_prom[7];
	n_prom[7] = 0xFFF0 & n_prom[7];
	// Operation is performed on bytes, 16 bytes total
	for (cnt = 0; cnt < 16; cnt++)
	{
		if (cnt % 2 == 1)
			// Choose LSB
			n_rem ^= (unsigned short)((n_prom[cnt >> 1]) & 0x00FF);
		else
			// Choose MSB
			n_rem ^= (unsigned short)(n_prom[cnt >> 1] >> 8);
		for (n_bit = 8; n_bit > 0; n_bit--)
		{
			if (n_rem & (0x8000))
			{
				n_rem = (n_rem << 1) ^ 0x3000;
			}
			else
			{
				n_rem = (n_rem << 1);
			}
		}
	}
	// Final 4-bit reminder is  caculated CRC code
	n_rem = (0x000F & (n_rem >> 12));
	// Restore the crc_read to its original place
	n_prom[7] = crc_read;
return (n_rem ^ crc);
}

// Convert from twos complement if required
int TenBitConversion(int value)
{
	int converted = value;
	if (value > 512)
	{
		converted = ((value - 512 - 1) ^ 511) * -1;
	}
	return converted;
}

// Convert from twos complement if required
int FourteenBitConversion(int value)
{
	int converted = value;
	if (value > 8192)
	{
		converted = ((value - 8192 - 1) ^ 8191) * -1;
	}
	return converted;
}

// Read calibration data from PROM, MSB LSB Swapped
void readCalibration()
{
	int fd = begin();
	unsigned char i;
	do
	{
		for (i = 0; i < 8; i++)
		{
			cal.PROM[i] = i2cReadWordData(fd, 0xA0 + (i << 1));
			cal.PROM[i] = (cal.PROM[i] >> 8) | (cal.PROM[i] << 8) & 0xFFFF;
		}
	} while (_89BSD_CRC4(cal.PROM));

	// Assignment result to calibration factor
	cal.C0 = (cal.PROM[1] & 0xFFFC) >> 2;
	cal.C0 = FourteenBitConversion(cal.C0);
	cal.C1 = ((cal.PROM[1] & 0x03) << 12) + ((cal.PROM[2] & 0xFFF0) >> 4);
	cal.C1 = FourteenBitConversion(cal.C1);
	cal.C2 = (((cal.PROM[2] & 0xF) << 6) + (cal.PROM[3] >> 10)) & 0x3FF;
	cal.C2 = TenBitConversion(cal.C2);
	cal.C3 = cal.PROM[3] & 0x3FF;
	cal.C3 = TenBitConversion(cal.C3);
	cal.C4 = (cal.PROM[4] >> 6) & 0x3FF;
	cal.C4 = TenBitConversion(cal.C4);
	cal.C5 = (((cal.PROM[4] & 0x3F) << 4) + (cal.PROM[5] >> 12)) & 0x3FF;
	cal.C5 = TenBitConversion(cal.C5);
	cal.C6 = (cal.PROM[5] >> 2) & 0x3FF;
	cal.C6 = TenBitConversion(cal.C6);
	cal.A0 = (((cal.PROM[5] & 0x3) << 8) + (cal.PROM[6] >> 8)) & 0x3FF;
	cal.A0 = TenBitConversion(cal.A0);
	cal.A1 = (((cal.PROM[6] & 0xFF) << 2) + (cal.PROM[7] >> 14)) & 0x3FF;
	cal.A1 = TenBitConversion(cal.A1);
	cal.A2 = (cal.PROM[7] >> 4) & 0x3FF;
	cal.A2 = TenBitConversion(cal.A2);

	close(fd);
}

// Read the uncompensated pressure value
unsigned int readRawPressure(void)
{
	int fd = begin();

	// Send pressure conversion command with oversampling setting(OSR)
	i2cWriteByte(fd, 0x40 + (_89BSD_OVERSAMPLING_SETTING << 1));

	// Wait for conversion, delay time dependent on oversampling setting
	switch (_89BSD_OVERSAMPLING_SETTING)
	{
	case 0:
		usleep(600);
		break;

	case 1:
		usleep(1200);
		break;

	case 2:
		usleep(2300);
		break;

	case 3:
		usleep(4600);
		break;

	case 4:
		usleep(9100);
		break;
	}

	// Read the three byte result
	// MSB, LSB, XLSB
	__u8 values[3];
	i2cReadBlockData(fd, 0x00, 3, values);

	unsigned long D1 = (((unsigned long)values[0] << 16) | ((unsigned long)values[1] << 8) | (unsigned long)values[2]);

	// Close the i2c file
	close(fd);

	return D1;
}

// Read the uncompensated temperature value
unsigned int readRawTemperature(void)
{
	int fd = begin();

	// Send temperature conversion command with oversampling setting(OSR)
	i2cWriteByte(fd, 0x50 + (_89BSD_OVERSAMPLING_SETTING << 1));

	// Wait for conversion, delay time dependent on oversampling setting
	switch (_89BSD_OVERSAMPLING_SETTING)
	{
	case 0:
		usleep(600);
		break;

	case 1:
		usleep(1200);
		break;

	case 2:
		usleep(2300);
		break;

	case 3:
		usleep(4600);
		break;

	case 4:
		usleep(9100);
		break;
	}

	// Read the three byte result
	// MSB, LSB, XLSB
	__u8 values[3];
	i2cReadBlockData(fd, 0x00, 3, values);

	unsigned long D2 = (((unsigned long)values[0] << 16) | ((unsigned long)values[1] << 8) | (unsigned long)values[2]);

	// Close the i2c file
	close(fd);

	return D2;
}

// Calculate pressure given uncalibrated pressure
// Value returned will be in units of Pa
double getPressure(void)
{
	unsigned int D1 = readRawPressure();
	unsigned int D2 = readRawTemperature();

	// Calculate temperature compensated pressure
	double D2by2power24 = D2 / 16777216.0;
	double D2by2power24power2 = D2by2power24 * D2by2power24;
	double Y = (D1 + cal.C0 * 512.0 + cal.C3 * 32768.0 * D2by2power24 + cal.C4 * 32768.0 * D2by2power24power2) / (cal.C1 * 2048.0 + cal.C5 * 65536.0 * D2by2power24 + cal.C6 * 65536.0 * D2by2power24power2);
	double C2mul2powerQ2by2power24 = cal.C2 / 32768.0;
	double P = Y * (1 - C2mul2powerQ2by2power24) + C2mul2powerQ2by2power24 * Y * Y;
	double Pressure = (P - 0.1) / 0.8 * (PMAX - PMIN) + PMIN;
	return Pressure;
}

// Compensate and calculate temperature
double getTemperature(void)
{
	unsigned int D2 = readRawTemperature();
	double D2by2power24 = D2 / 16777216.0;
	double TEMP = cal.A0 / 3.0 + cal.A1 * 2.0 * D2by2power24 + cal.A2 * 2.0 * D2by2power24 * D2by2power24;
	return TEMP;
}
