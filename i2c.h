//=========================================================================
// i2c.h
//
// An interface for the RM3100 3-axis magnetometer from PNI Sensor Corp.
// 
// Author:      David Witten, KD0EAG
// Date:        April 21, 2020
// License:     GPL 3.0
//=========================================================================
int i2cOpen(pList *p);
int i2c_init2(char *i2c_fname);      // eliminate me
void i2c_close(int fd);
void i2c_SetAddress(int fd, int devAddr);
void i2c_write(int fd, uint8_t reg, uint16_t value);
uint8_t i2c_read(int fd, uint8_t reg);
int i2c_writebuf(int fd, uint8_t reg, char* buffer, short int length);
int i2c_readbuf(int fd, uint8_t reg,  char* buffer, short int length);
uint16_t readRegister(int fd, uint8_t reg);
void writeRegister(int fd, uint8_t reg, uint16_t value);

//int i2c_readbuf(int fd, char reg,  char* buf, short int length);

//SensorStatus mag_enable_interrupts();
//SensorStatus mag_disable_interrupts();
//SensorPowerMode mag_set_power_mode(SensorPowerMode mode);
//SensorPowerMode mag_get_power_mode();
//unsigned short mag_set_sample_rate(unsigned short sample_rate);
//unsigned short mag_get_sample_rate();

