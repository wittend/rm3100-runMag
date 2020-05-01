//=========================================================================
// i2c.h
//
// An interface for the RM3100 3-axis magnetometer from PNI Sensor Corp.
// 
// Author:      David Witten, KD0EAG
// Date:        April 21, 2020
// License:     GPL 3.0
//=========================================================================
#ifndef PNIRM3100_I2C_H
#define PNIRM3100_I2C_H

int i2c_open(pList *p);
void i2c_close(int fd);
void i2c_setAddress(int fd, int devAddr);
void i2c_write(int fd, uint8_t reg, uint16_t value);
uint8_t i2c_read(int fd, uint8_t reg);
int i2c_writebuf(int fd, uint8_t reg, char* buffer, short int length);
int i2c_readbuf(int fd, uint8_t reg, char* buf, short int length);
//uint16_t i2c_regRead(int fd, uint8_t reg);
//void i2c_regWrite(int fd, uint8_t reg, uint16_t value);

//int i2c_readbuf(int fd, char reg,  char* buf, short int length);

//SensorStatus mag_enable_interrupts();
//SensorStatus mag_disable_interrupts();
//SensorPowerMode mag_set_power_mode(SensorPowerMode mode);
//SensorPowerMode mag_get_power_mode();
//unsigned short mag_set_sample_rate(unsigned short sample_rate);
//unsigned short mag_get_sample_rate();

#endif //PNIRM3100_I2C_H