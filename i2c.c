//=========================================================================
// i2c.c
//
// An interface for the RM3100 3-axis magnetometer from PNI Sensor Corp.
// Derived in part from several sources:
//      https://github.com/miguelrasteiro/RM3100.X
//      Jeremiah Mattison / Rm3100: https://os.mbed.com/users/fwrawx/code/Rm3100/
//      https://github.com/shannon-jia/rm3100
//      Song Qiang <songqiang1304521@gmail.com (Linux driver):
//          Linux kernel driver: https://github.com/torvalds/linux/tree/v5.3/drivers/iio/magnetometer
// 
// Author:      David Witten, KD0EAG
// Date:        April 21, 2020
// License:     GPL 3.0
//=========================================================================
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>
#include <linux/i2c-dev.h>
#include "device_defs.h"
#include "i2c.h"

#define RASPI_I2C_BUS       "/dev/i2c-1"
#define ODROIDC1_I2C_BUS    "/dev/i2c-1"
#define ODROIDC2_I2C_BUS    "/dev/i2c-1"
#define ODROIDN2_I2C_BUS    "/dev/i2c-2"
#define NV_XAVIER_I2C_BUS   "/dev/i2c-8"
#define NV_NANO_I2C_BUS     "/dev/i2c-1"

struct busDev
{
    const char *devPath;
    int         enumVal;
};

static struct busDev busDevs[] =
{
    /* Darice Path,     enum Value*/
    {RASPI_I2C_BUS,     eRASPI_I2C_BUS},
    {ODROIDC1_I2C_BUS,  eODROIDC1_I2C_BUS},
    {ODROIDN2_I2C_BUS,  eODROIDC2_I2C_BUS},
    {ODROIDN2_I2C_BUS,  eODROIDN2_I2C_BUS},
    {NV_XAVIER_I2C_BUS, eNV_XAVIER_I2C_BUS},
    {NV_NANO_I2C_BUS,   eNV_NANO_I2C_BUS},
    {NULL,              -1}
};


//------------------------------------------
// i2cInit()
//------------------------------------------
int i2cOpen(pList *p)
{
    char i2cFname[] = ODROIDN2_I2C_BUS;
    //char i2cFname[] = busDevs[p->i2cBusNumber].devPath;
    //fprintf(stdout, "BusPath:  %s\n", busDevs[p->i2cBusNumber].devPath);
    //fflush(stdout);
    // Initialize the I2C bus
    //p->i2c_fd = i2c_init2(i2cFname);
    p->i2c_fd = -1;
    //fprintf(stdout, "open('%s') in i2c_init", i2cFname);
    //fflush(stdout);
    if((p->i2c_fd = open(i2cFname, O_RDWR)) < 0)
    {
        //char err[200];
        perror("Bus open failed\n");
        return -1;
    }
    else
    {
        if(p->verboseFlag)
        {
            fprintf(stdout, "Device handle p->i2c_fd:  %d\n", p->i2c_fd);
            fprintf(stdout, "i2c_init OK!\n");
            fflush(stdout);
        }
    }
    return p->i2c_fd;
}

//------------------------------------------
// write an 8 bit value to a register reg.
//------------------------------------------
void writeRegister(int fd, uint8_t reg, uint16_t value)
{
    uint8_t data[2];
    data[0] = reg;
    data[1] = value & 0xff;
    if(write(fd, data, 2) != 2)
    {
        perror("writeRegister");
    }
}

//------------------------------------------
// read an 8 bit value from a register.
//------------------------------------------
uint16_t readRegister(int fd, uint8_t reg)
{
    uint8_t data[2];
    data[0] = reg;
    if(write(fd, data, 1) != 1)
    {
        perror("readRegister set register");
    }
    if(read(fd, data + 1, 1) != 1)
    {
        perror("readRegister read value");
    }
    return data[1];
}

//------------------------------------------
// i2c_SetAddress()
//
// set the I2C slave address for all
// subsequent I2C device transfers.
//------------------------------------------
void i2c_SetAddress(int fd, int devAddr)
{
    if (ioctl(fd, I2C_SLAVE, devAddr) < 0)
    {
        perror("i2cSetAddress");
        exit(1);
    }
}

//--------------------------------------------------------------------
// i2c_close()
//
// Close I2C bus
//--------------------------------------------------------------------
void i2c_close(int i2c_fd)
{
    close(i2c_fd);
}

//------------------------------------------
// write an 8 bit value to a register reg.
//------------------------------------------
void i2c_write(int fd, uint8_t reg, uint16_t value)
{
    uint8_t data[2];
    data[0] = reg;
    data[1] = value & 0xff;

    //fprintf(stdout, "i2c_write() :: File Number: %d, reg: %x.\n", fd, reg);
    //fflush(stdout);
    
    if(write(fd, data, 2) != 2)
    {
        perror("i2c_write()");
    }
}

//------------------------------------------
// i2c_writebuf()
// write a buffer to the device
//------------------------------------------
int i2c_writebuf(int fd, uint8_t reg, char *buffer, short int length)
{
    int status = 0;
    uint8_t data[2];

    i2c_write(fd, reg, 1);
    if(status = write(fd, buffer, length) != length)
    {
        perror("i2c_writebuf");
        exit(1);
    }
    return status;
}

//------------------------------------------
// read an 8 bit value from a register.
//------------------------------------------
uint8_t i2c_read(int fd, uint8_t reg)
{
    uint8_t data[2];
    data[0] = reg;
    int rv = 0;

    //fprintf(stdout, "File descriptor: %d, reg: %x.\n", fd, reg);
    //fflush(stdout);
    //
    rv = write(fd, data, 1);
    if(rv != 1)
    {
        perror("i2c_read set register");
        //fprintf(stdout, "i2c_read() :: (write(%d, %x, 1) != 1) (== %d).\n", fd, data[0], rv);
        //fflush(stdout);
    }
    
    //fprintf(stdout, "Register: 0x%x.\n", reg);
    //fflush(stdout);
    
    if(read(fd, data + 1, 1) != 1)
    {
        perror("i2c_read read value.");
    }
    return data[1];
}

//------------------------------------------
// i2c_readbuf()
// write a buffer to the device
//------------------------------------------
int i2c_readbuf(int fd, uint8_t devAddr,  char* buf, short int length)
{
    int bytes_read;

    i2c_SetAddress(fd, devAddr);
    if((bytes_read = read(fd, buf, length)) != length)
    {
        perror("i2c transaction i2c_readbuf() failed.\n");
    }
    //else
    //{
    //    /* buf[0] contains the read byte */
    //    printf("i2c transaction i2c_readbuf() OK. bytes_read: %i\n", bytes_read);
    //}
    return bytes_read;
}
