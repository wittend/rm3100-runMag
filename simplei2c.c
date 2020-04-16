//=========================================================================
// simplei2c.c
// /home/dave/Projects/SWx/SWx-C/i2c-c/i2c-rm3100.c
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
//#include "i2c_local.h"

char version[] = SIMPLEI2C_VERSION;
    
int getCommandLine(int argc, char** argv, pList *p);

int i2c_init(char *fn);
void i2c_close(int fd);
void i2c_SetAddress(int fd, int devAddr);
void i2c_write(int fd, uint8_t reg, uint16_t value);
uint8_t i2c_read(int fd, uint8_t reg);
int i2c_writebuf(int fd, uint8_t reg, char* buffer, short int length);
int i2c_readbuf(int fd, uint8_t reg,  char* buffer, short int length);
//int i2c_readbuf(int fd, char reg,  char* buf, short int length);

SensorStatus mag_enable_interrupts();
SensorStatus mag_disable_interrupts();
SensorPowerMode mag_set_power_mode(SensorPowerMode mode);
SensorStatus mag_initialize_sensor();
SensorPowerMode mag_get_power_mode();
unsigned short mag_set_sample_rate(unsigned short sample_rate);
unsigned short mag_get_sample_rate();
void mag_get_sample_data(int * XYZ);

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
//  rm3100_i2c_write()
//------------------------------------------
unsigned int rm3100_i2c_write(int fd, uint8_t reg, char *buffer, short int length)
{
    char writeBuffer[MAX_I2C_WRITE + 1];
    writeBuffer[0] = reg;
    memcpy(&writeBuffer[1], buffer, length);
    int status = i2c_writebuf(fd, RM3100_I2C_ADDRESS, (char *)writeBuffer, length + 1);
    return !status;         // return True if successfull. mbed:0=Success
}

//------------------------------------------
// rm3100_i2c_read()
//------------------------------------------
unsigned int rm3100_i2c_read(int fd, uint8_t reg, char *buffer, short int length)
{
    char readBuffer[1] = {reg};
    i2c_SetAddress(fd, RM3100_I2C_ADDRESS);
    int status = i2c_readbuf(fd, RM3100_I2C_ADDRESS, buffer, length);
    if(!status)
    {
        return length;
    }
    else
    {
        return 0;
    }
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
// i2c_init()
//
// Returns a new file descriptor for communicating with the I2C bus:
//--------------------------------------------------------------------
int i2c_init(char *i2c_fname)
{
    int i2c_fd = -1;
    if((i2c_fd = open(i2c_fname, O_RDWR)) < 0)
    {
        char err[200];
        sprintf(err, "open('%s') in i2c_init", i2c_fname);
        perror(err);
        return -1;
    }
    return i2c_fd;
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

    if(write(fd, data, 2) != 2)
    {
        perror("writeRegister");
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

    if(write(fd, data, 1) != 1)
    {
        perror("i2c_read set register");
    }
    if(read(fd, data + 1, 1) != 1)
    {
        perror("i2c_read read value");
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
    else
    {
        /* buf[0] contains the read byte */
        printf("i2c transaction i2c_readbuf() OK. bytes_read: %i\n", bytes_read);
    }
    return bytes_read;
}

//------------------------------------------
// readTemp()
//------------------------------------------
int readTemp(int fd, int devAddr)
{
    int temp = -99;
    char data[2] = {0};
    char reg[1] = {MCP9808_REG_AMBIENT_TEMP};

    i2c_SetAddress(fd, devAddr);
    write(fd, reg, 1);
    if(read(fd, data, 2) != 2)
    {
        printf("Error : Input/Output error \n");
    }
    else
    {
        // Convert the data to 13-bits
        temp = ((data[0] & 0x1F) * 256 + data[1]);
        if(temp > 4095)
        {
            temp -= 8192;
        }
    }
    return temp;
}

//------------------------------------------
// setup_mag()
//------------------------------------------
int setup_mag(int verboseFlag, int fd, int device)
{
    uint8_t ver = 0;
    int rv = SensorOK;

    // Set address of the RM3100
    ioctl(fd, I2C_SLAVE, device);

    // Check Version
    if((ver = i2c_read(fd, RM3100I2C_REVID)) != (uint8_t)RM3100_VER_EXPECTED)
    {
        // Fail, exit...
        fprintf(stderr, "RM3100 REVID NOT CORRECT: ");
        fprintf(stderr, "RM3100 REVID: 0x%x <> EXPECTED: 0x%x.\n\n", ver, RM3100_VER_EXPECTED);
        fflush(stdout);
        exit(1);
    }
    else
    {
        uint8_t i2cbuffer[2];
        // Zero buffer content
        i2cbuffer[0] = 0;
        i2cbuffer[1] = 0;
        if(verboseFlag)
        {
             printf("RM3100 Detected Properly: ");
             printf("REVID: %x.\n", ver);
        }
        // Clears RM3100I2C_POLL and RM3100I2C_CMM register and any pending measurement
        i2c_writebuf(fd, RM3100I2C_POLL, i2cbuffer, 2);
        // Initialize settings
        uint8_t settings[7];
        settings[0] = CCP1;       // CCPX1 200 Cycle Count
        settings[1] = CCP0;       // CCPX0
        settings[2] = CCP1;       // CCPY1 200 Cycle Count
        settings[3] = CCP0;       // CCPY0
        settings[4] = CCP1;       // CCPZ1 200 Cycle Count
        settings[5] = CCP0;       // CCPZ0
        settings[6] = NOS;
        //  Write register settings
        i2c_writebuf(fd, RM3100I2C_CCX_1, settings, 7);
        //mag_set_power_mode(SensorPowerModePowerDown);
    }
    if(verboseFlag)
    {
        // poll the RM3100 for a three axis measurement
        printf("Polling I2C device: %x.\n\n", RM3100_I2C_ADDRESS);
        fflush(stdout);
    }
    // Sleep for 1 second
    usleep(100000);                           // delay to help monitor DRDY pin on eval board
    return rv;
}

//------------------------------------------
// readMag()
//------------------------------------------
int readMag(int fd, int devAddr, int32_t *XYZ)
{
    uint8_t mSamples[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    char data[2] = {0};
    int bytes_read;
    char reg[1] = { RM3100I2C_XYZ };

    // set address of the RM3100.
    i2c_SetAddress(fd, devAddr);
    // Write command to  use polling.
    writeRegister(fd, RM3100I2C_POLL, RM3100I2C_POLLXYZ);

    // Check if DRDY went high and wait unit high before reading results
    while((fd, readRegister(fd, RM3100I2C_STATUS) & RM3100I2C_READMASK) != RM3100I2C_READMASK) {}

    // Read the XYZ registers
    write(fd, reg, 1);
    if((bytes_read = read(fd, mSamples, sizeof(mSamples))) != sizeof(mSamples))
    {
        perror("i2c transaction i2c_readbuf() failed.\n");
    }

    //printf("\nBytes Read: %i\n", bytes_read);

    // hh:  For integer types, causes printf to expect an int-sized integer argument which was promoted from a char
    //printf("mSamples[0]: %hhi, mSamples[1]: %hhi, mSamples[2]: %hhi\n", mSamples[0], mSamples[1], mSamples[2]);
    //printf("mSamples[3]: %hhi, mSamples[4]: %hhi, mSamples[5]: %hhi\n", mSamples[3], mSamples[4], mSamples[5]);
    //printf("mSamples[6]: %hhi, mSamples[7]: %hhi, mSamples[8]: %hhi\n", mSamples[6], mSamples[7], mSamples[8]);
    //fflush(stdout);

    XYZ[0] = ((signed char)mSamples[0]) * 256 * 256;
    XYZ[0] |= mSamples[1] * 256;
    XYZ[0] |= mSamples[2];
    XYZ[1] = ((signed char)mSamples[3]) * 256 * 256;
    XYZ[1] |= mSamples[4] * 256;
    XYZ[1] |= mSamples[5];
    XYZ[2] = ((signed char)mSamples[6]) * 256 * 256;
    XYZ[2] |= mSamples[7] * 256;
    XYZ[2] |= mSamples[8];

    return bytes_read;
}

//------------------------------------------
// currentTimeMillis()
//------------------------------------------
long currentTimeMillis()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec * 1000 + time.tv_usec / 1000;
}

//------------------------------------------
// showSettings()
//------------------------------------------
int showSettings(pList *p)
{
    fprintf(stdout, "\nVersion = %s\n", version);
    fprintf(stdout, "\nCurrent Parameters:\n\n");
    fprintf(stdout, "   -b <I2C bus number>    :  I2C bus number as integer: %i\n", p->i2c_bus_number);
    fprintf(stdout, "   -c <count>             :  Cycle count as integer: %i\n", p->cc_x);
    fprintf(stdout, "   -j                     :  Format output as JSON: %s\n", p->jsonFlag ? "TRUE" : "FALSE" );
    fprintf(stdout, "   -l                     :  Read local temperature only: %s\n", p->localTempOnly ? "TRUE" : "FALSE");
    fprintf(stdout, "   -L [addr as integer]   :  Local temperature address: %i\n", p->localTempAddr);
    fprintf(stdout, "   -m                     :  Read magnetometer only: %s\n", p->magnetometerOnly ?  "TRUE" : "FALSE");
    fprintf(stdout, "   -M [addr as integer]   :  Magnetometer address: %i\n", p->magnetometerAddr);
    fprintf(stdout, "   -q                     :  Quiet mode: %s\n", p->quietFlag ? "TRUE" : "FALSE" );
    fprintf(stdout, "   -r                     :  Read remote temperature only: %s\n", p->remoteTempOnly ? "TRUE" : "FALSE");
    fprintf(stdout, "   -R [addr as integer]   :  Remote temperature address: %i\n", p->remoteTempAddr);
    fprintf(stdout, "   -s                     :  Return single magnetometer reading: %s\n", p->singleRead ? "TRUE" : "FALSE");
    fprintf(stdout, "   -S                     :  Read Simple Magnetometer Support Board: %i\n", p->boardType);
    fprintf(stdout, "   -v                     :  Verbose output: %s\n", p->verboseFlag ? "TRUE" : "FALSE");
    fprintf(stdout, "   -X                     :  Read Board with Extender: %i\n", p->boardType);
}

//------------------------------------------
// getCommandLine()
//------------------------------------------
int getCommandLine(int argc, char** argv, pList *p)
{
    int c;
    int digit_optind = 0;
    
    if(p != NULL)
    {
        memset(p, 0, sizeof(pList));
    }
    p->verboseFlag      = FALSE;
    p->quietFlag        = FALSE;
    p->jsonFlag         = FALSE;
    p->i2c_bus_number   = 1;
    p->i2c_fd           = 0;

    p->localTempOnly    = FALSE;
    p->localTempAddr    = 0x19;
    p->remoteTempOnly   = FALSE;
    p->remoteTempAddr   = 0x18;  
    p->magnetometerOnly = FALSE;
    p->singleRead       = FALSE;
    p->boardType        = 0;
    p->outDelay         = 1000000;
    
    p->cc_x             = CC_200;
    p->cc_y             = CC_200;
    p->cc_z             = CC_200;

    p->x_gain           = GAIN_75;
    p->y_gain           = GAIN_75;
    p->z_gain           = GAIN_75;
    
    p->Version          = version;
   
    while((c = getopt(argc, argv, "?b:c:hjlL:mM:o:qsSrR:vXhqV")) != -1)
    {
        int this_option_optind = optind ? optind : 1;
        switch (c)
        {
            case 'b':
                fprintf(stdout, "Bus Id: '%s'\n", optarg);
                p->i2c_bus_number = atoi(optarg);
                break;
            case 'c':
                fprintf(stdout, "CycleCount: '%s'\n", optarg);
                p->cc_x = p->cc_y = p->cc_z = atoi(optarg);
                break;
            case 'j':
                fprintf(stdout, "Output JSON formatted data.\n");
                p->jsonFlag = TRUE;
                break;
            case 'l':
                fprintf(stdout, "Read local temperature only.\n");
                p->localTempOnly = TRUE;
                break;
            case 'L':
                fprintf(stdout, "Local temp address [default 19 hex]: %s hex\n", optarg);
                p->localTempAddr = atoi(optarg);
                break;
            case 'm':
                fprintf(stdout, "Read magnetometer only.\n");
                p->magnetometerOnly = TRUE;
                break;
            case 'M':
                fprintf(stdout, "Magnetometer address [default 20 hex]: %s hex\n", optarg);
                p->magnetometerAddr = atoi(optarg);
                break;
            case 'o':
                fprintf(stdout, "Outout delay rate [default 1000 ms]: %s ms\n", optarg);
                p->outDelay = atoi(optarg) * 1000;
                break;
            case 'q':
                p->quietFlag = TRUE;
                p->verboseFlag = FALSE;
                break;
            case 's':
                fprintf(stdout, "Return single magnetometer reading.\n");
                p->singleRead = TRUE;
                break;
            case 'S':
                fprintf(stdout, "Read Simple Magnetometer Support Board.\n");
                p->boardType = 1;
                break;
            case 'r':
                fprintf(stdout, "Read remote temp only.'\n");
                p->remoteTempOnly = atoi(optarg);
                break;
            case 'R':
                fprintf(stdout, "Remote temp address [default 18 hex]: %s hex\n", optarg);
                p->remoteTempAddr = atoi(optarg);
                break;
            case 'V':
                fprintf(stdout, "Version = %s\n", version);
                return 1;
                break;
            case 'v':
                p->verboseFlag = TRUE;
                p->quietFlag = FALSE;
                break;
            case 'X':
                p->boardType = 0;
                break;
            case 'h':
            case '?':
                fprintf(stdout, "\n%s Version = %s\n", argv[0], version);
                fprintf(stdout, "\nParameters:\n\n");
                fprintf(stdout, "   -b <bus as integer>    :  I2C bus number as integer.\n");
                fprintf(stdout, "   -c <count>             :  Cycle count as integer (default 200).\n");
                fprintf(stdout, "   -j                     :  Format output as JSON.\n");
                fprintf(stdout, "   -l                     :  Read local temperature only.\n");
                fprintf(stdout, "   -L [addr as integer]   :  Local temperature address [default 19 hex].\n");
                fprintf(stdout, "   -m                     :  Read magnetometer only.\n");
                fprintf(stdout, "   -M [addr as integer]   :  Magnetometer address [default 20 hex].\n");
                fprintf(stdout, "   -q                     :  Quiet mode.");
                fprintf(stdout, "   -r                     :  Read remote temperature only.\n");
                fprintf(stdout, "   -R [addr as integer]   :  Remote temperature address [default 18 hex].\n");
                fprintf(stdout, "   -s                     :  Return single magnetometer reading.\n");
                fprintf(stdout, "   -S                     :  Read Simple Magnetometer Support Board.\n");
                fprintf(stdout, "   -v                     :  Verbose output.\n");
                fprintf(stdout, "   -V                     :  Display software version and exit.\n");
                fprintf(stdout, "   -X                     :  Read Board with Extender (default).\n");
                fprintf(stdout, "   -h or -?               :  Display this help.\n\n");
                return 1;
                break;
            default:
                fprintf(stdout, "?? getopt returned character code 0x%2X ??\n", c);
                break;
        }
    }
    if(optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
        {
            printf ("%s ", argv[optind++]);
        }
        printf ("\n");
    }
    //exit (0);
    return 0;
}

//------------------------------------------
//  main()
//------------------------------------------
int main(int argc, char** argv)
{
    pList p;

    int32_t xyz[3];
    int fd = -1;
    int temp = 0;
    float cTemp = 0.0;
    char i2cFname[] = ODROIDN2_I2C_BUS;
    int rv = 0;
       
    if((rv = getCommandLine(argc, argv, &p)) != 0)
    {
        return rv;
    }
    showSettings(&p);
    //fprintf(stdout, "p.verboseFlag:    %i\n", p.verboseFlag);
    //fprintf(stdout, "p.jsonFlag:       %i\n", p.jsonFlag);
    //fprintf(stdout, "p.Version:        %s\n", p.Version);
    //fprintf(stdout, "p.i2c_bus_number: %i\n", p.i2c_bus_number);
//exit(0);    
    // Initialize the I2C bus
    fd = i2c_init(i2cFname);
    if(fd >= 0)
    {
        fprintf(stdout, "i2c_init OK!\n");
    }
    else
    {
        perror("i2c_init failed!\n");
        exit(1);
    }

    // Setup the magnetometer.
    setup_mag(1, fd, RM3100_I2C_ADDRESS);
    if((p.verboseFlag))
    {
        fprintf(stdout, "Selected Settings:\n");
        fprintf(stdout, "\n");
        fprintf(stdout, "Selected Settings:\n");
        //fprintf(stdout, "  Time,      Temp C,   Raw X,    Raw Y,    Raw Z\n", currentTimeMillis(), cTemp, xyz[0], xyz[1], xyz[2]);
        fprintf(stdout, "---------------------------------------------------:\n");
    }
    // loop forever.
    while(1)
    {
        //  Read temp sensor.
        temp = readTemp(fd, MCP9808_I2CADDR_DEFAULT);
        cTemp = temp * 0.0625;
        //fTemp = cTemp * 1.8 + 32;

        // Read Magnetometer.
        readMag(fd, RM3100_I2C_ADDRESS, xyz);

        // Output the results.
        if(!(p.jsonFlag))
        {
            fprintf(stdout, "Time: %ld, Temp: %.2f C   X: %8i,  Y: %8i,  Z: %8i\n", currentTimeMillis(), cTemp, xyz[0], xyz[1], xyz[2]);
        }
        else
        {
            fprintf(stdout, "{ \"%ld\", \"%.2f\", \"%8i\",  \"%8i\",  \"%8i\"}\n", currentTimeMillis(), cTemp, xyz[0], xyz[1], xyz[2]);
        }
        fflush(stdout);

        // wait 1000 ms for next animation step.
        usleep(p.outDelay);
    }
    i2c_close(fd);
    return 0;
}
