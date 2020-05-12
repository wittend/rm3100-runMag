//=========================================================================
// runMag.c
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
#include "i2c.h"
#include "runMag.h"

//------------------------------------------
// mag_set_sample_rate()
//------------------------------------------
unsigned short mag_set_sample_rate(pList *p, unsigned short sample_rate)
{
    int i;
    static char i2cbuffer[1];
    const unsigned short int supported_rates[][2] = \
    {
        /* [Hz], register value */
        {   2, 0x0A},   // up to 2Hz
        {   4, 0x09},   // up to 4Hz
        {   8, 0x08},   // up to 8Hz
        {  16, 0x07},   // up to 16Hz
        {  31, 0x06},   // up to 31Hz
        {  62, 0x05},   // up to 62Hz
        {  125, 0x04},  // up to 125Hz
        {  220, 0x03}   // up to 250Hz
        };
        
    for(i = 0; i < sizeof(supported_rates)/(sizeof(unsigned short int)*2) - 1; i++)
    {
        if(sample_rate <= supported_rates[i][0])
        {
            break;
        }
    }
    //if(mSensorMode == SensorPowerModeActive) 
    //{
    //    mag_disable_interrupts();
    //}
    p->mSampleRate = supported_rates[i][0];
//    i2cbuffer[0] = (char)supported_rates[i][1];
//    rm3100_i2c_write(RM3100_TMRC_REG, i2cbuffer, 1);
    i2c_write(p->i2c_fd, RM3100I2C_TMRC, p->mSampleRate);
    //if (mSensorMode == SensorPowerModeActive) 
    //{
    //    mag_enable_interrupts();
    //}
    return p->mSampleRate;
}

//------------------------------------------
// mag_get_sample_rate();
// The actual sample rate of the sensor.
//------------------------------------------
unsigned short mag_get_sample_rate(pList *p)
{
    return p->mSampleRate;
}

//------------------------------------------
// setup_mag()
//------------------------------------------
int setup_mag(pList *p)
{
    uint8_t ver = 0;
    uint8_t i2cbuffer[2];
    int rv = SensorOK;

    // Set address of the RM3100
    i2c_setAddress(p->i2c_fd,  p->magnetometerAddr);

    // Check Version
    if((ver = i2c_read(p->i2c_fd, RM3100I2C_REVID)) != (uint8_t)RM3100_VER_EXPECTED)
    {
        // Fail, exit...
        fprintf(stderr, "RM3100 REVID NOT CORRECT: ");
        fprintf(stderr, "RM3100 REVID: 0x%X <> EXPECTED: 0x%X.\n\n", ver, RM3100_VER_EXPECTED);
        fflush(stdout);
        exit(1);
    }
    else
    {
        if(p->verboseFlag)
        {
             printf("RM3100 Detected Properly: ");
             printf("REVID: %x.\n", ver);
        }
        i2c_write(p->i2c_fd, RM3100_MAG_POLL, 0);
        i2c_write(p->i2c_fd, RM3100I2C_CMM,  0);
        // Initialize CC settings
        setCycleCountRegs(p);
    }
    // Sleep for 1 second
    usleep(100000);                           // delay to help monitor DRDY pin on eval board
    return rv;
}

//------------------------------------------
// runBIST()
// Runs the Built In Self Test.
//------------------------------------------
int runBIST(pList *p)
{
    return 0;
    //return i2c_read(p->i2c_fd, RM3100I2C_TMRC);
}

//------------------------------------------
// getTMRSReg()
// Sets Continuous Measurement Mode Data Rate
//------------------------------------------
int getCMMReg(pList *p)
{
    return i2c_read(p->i2c_fd, RM3100I2C_TMRC);
}

//------------------------------------------
// setTMRSReg()
// Sets Continuous Measurement Mode Data Rate
//------------------------------------------
void setCMMReg(pList *p)
{
    i2c_write(p->i2c_fd, RM3100I2C_TMRC, p->TMRCRate);
}

//------------------------------------------
// getTMRSReg()
// Sets Continuous Measurement Mode Data Rate
//------------------------------------------
int getTMRCReg(pList *p)
{
    //To set the TMRC register, send the register address, 0x0B, followed by the desired
    //TMRC register value. To read the TMRC register, send 0x8B.
    return i2c_read(p->i2c_fd, RM3100I2C_TMRC);
}

//------------------------------------------
// setTMRSReg()
// Sets Continuous Measurement Mode Data Rate
//------------------------------------------
void setTMRCReg(pList *p)
{
    //To set the TMRC register, send the register address, 0x0B, followed by the desired
    //TMRC register value. To read the TMRC register, send 0x8B.
    i2c_write(p->i2c_fd, RM3100I2C_TMRC, p->TMRCRate);
}

//------------------------------------------
// setCycleCountRegs()
//------------------------------------------
void setCycleCountRegs(pList *p)
{
    int i = 0;
    i2c_write(p->i2c_fd, RM3100I2C_CCX_1, (p->cc_x >> 8));
    i2c_write(p->i2c_fd, RM3100I2C_CCX_0, (p->cc_x & 0xff));
    i2c_write(p->i2c_fd, RM3100I2C_CCY_1, (p->cc_y >> 8));
    i2c_write(p->i2c_fd, RM3100I2C_CCY_0, (p->cc_y & 0xff));
    i2c_write(p->i2c_fd, RM3100I2C_CCZ_1, (p->cc_x >> 8));
    i2c_write(p->i2c_fd, RM3100I2C_CCZ_0, (p->cc_y & 0xff));
    i2c_write(p->i2c_fd, RM3100I2C_NOS,   NOS);
}

//------------------------------------------
// readCycleCountRegs()
//------------------------------------------
void readCycleCountRegs(pList *p)
{
    int i = 0;
    uint8_t regCC[7]= { 0, 0, 0, 0, 0, 0, 0 };

    i2c_setAddress(p->i2c_fd, p->magnetometerAddr);
    //  Read register settings
    i2c_readbuf(p->i2c_fd, RM3100I2C_CCX_1, regCC, 7);
    printf("regCC[%i]: 0x%X\n",    0, (uint8_t)regCC[0]);
    printf("regCC[%i]: 0x%X\n",    1, (uint8_t)regCC[1]);
    printf("regCC[%i]: 0x%X\n",    2, (uint8_t)regCC[2]);
    printf("regCC[%i]: 0x%X\n",    3, (uint8_t)regCC[3]);
    printf("regCC[%i]: 0x%X\n",    4, (uint8_t)regCC[4]);
    printf("regCC[%i]: 0x%X\n",    5, (uint8_t)regCC[5]);
    printf("regCC[%i]: 0x%X\n\n",  6, (uint8_t)regCC[6]);
}

