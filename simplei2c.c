//=========================================================================
// simplei2c.c
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

char version[] = SIMPLEI2C_VERSION;
    
int setup_mag(pList *p);
long currentTimeMillis();
void mag_get_sample_data(int * XYZ);
int readTemp(pList *p, int devAddr);
int readMag(pList *p, int devAddr, int32_t *XYZ);
void showSettings(pList *p);
int getCommandLine(int argc, char** argv, pList *p);
int main(int argc, char** argv);

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
// readTemp()
//------------------------------------------
int readTemp(pList *p, int devAddr)
{
    int temp = -99;
    char data[2] = {0};
    char reg[1] = {MCP9808_REG_AMBIENT_TEMP};

    i2c_SetAddress(p->i2c_fd, devAddr);
    write(p->i2c_fd, reg, 1);
    if(read(p->i2c_fd, data, 2) != 2)
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
//int setup_mag(int verboseFlag, int pList *p, int device)
int setup_mag(pList *p)
{
    uint8_t ver = 0;
    int rv = SensorOK;

    //fprintf(stdout, "Before ioctl():: verboseFlag: %s, i2c_fd: %d,  i2cBusNumber: %d\n", p->verboseFlag ? "TRUE" : "FALSE", p->i2c_fd, p->i2cBusNumber);
    //fflush(stdout);
    
    // Set address of the RM3100
    i2c_SetAddress(p->i2c_fd,  p->magnetometerAddr);
    
    //fprintf(stdout, "Before Check Version: %d\n", p->i2c_fd);
    //fflush(stdout);

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
        uint8_t i2cbuffer[2];
        // Zero buffer content
        i2cbuffer[0] = 0;
        i2cbuffer[1] = 0;
        if(p->verboseFlag)
        {
             printf("RM3100 Detected Properly: ");
             printf("REVID: %x.\n", ver);
        }
    //fprintf(stdout, "After Check Version\n");
    //fflush(stdout);
        // Clears RM3100I2C_POLL and RM3100I2C_CMM register and any pending measurement
        i2c_writebuf(p->i2c_fd, RM3100I2C_POLL, i2cbuffer, 2);
    //fprintf(stdout, "After writebuf() #1\n");
    //fflush(stdout);
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
    //fprintf(stdout, "After writebuf() #2\n");
    //fflush(stdout);
        i2c_writebuf(p->i2c_fd, RM3100I2C_CCX_1, settings, 7);
        //mag_set_power_mode(SensorPowerModePowerDown);
    }
    if(p->verboseFlag)
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
int readMag(pList *p, int devAddr, int32_t *XYZ)
{
    uint8_t mSamples[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    char data[2] = {0};
    int bytes_read;
    char reg[1] = { RM3100I2C_XYZ };

    // set address of the RM3100.
    i2c_SetAddress(p->i2c_fd, devAddr);
    // Write command to  use polling.
    writeRegister(p->i2c_fd, RM3100I2C_POLL, RM3100I2C_POLLXYZ);

    // Check if DRDY went high and wait unit high before reading results
    while((p->i2c_fd, readRegister(p->i2c_fd, RM3100I2C_STATUS) & RM3100I2C_READMASK) != RM3100I2C_READMASK) {}

    // Read the XYZ registers
    write(p->i2c_fd, reg, 1);
    if((bytes_read = read(p->i2c_fd, mSamples, sizeof(mSamples))) != sizeof(mSamples))
    {
        perror("i2c transaction i2c_readbuf() failed.\n");
    }

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
void showSettings(pList *p)
{
    fprintf(stdout, "\nVersion = %s\n", version);
    fprintf(stdout, "\nCurrent Parameters:\n\n");
    fprintf(stdout, "   I2C bus number as integer:                  %i\n", p->i2cBusNumber);
    fprintf(stdout, "   Cycle count as integer:                     %i\n", p->cc_x);
    fprintf(stdout, "   Hide raw measurements:                      %s\n", p->hideRaw ? "TRUE" : "FALSE" );
    fprintf(stdout, "   Format output as JSON:                      %s\n", p->jsonFlag ? "TRUE" : "FALSE" );
    fprintf(stdout, "   Read local temperature only:                %s\n", p->localTempOnly ? "TRUE" : "FALSE");
    fprintf(stdout, "   Local temperature address:                  %2X\n", p->localTempAddr);
    fprintf(stdout, "   Read magnetometer only:                     %s\n", p->magnetometerOnly ?  "TRUE" : "FALSE");
    fprintf(stdout, "   Magnetometer address:                       %2X\n", p->magnetometerAddr);
    fprintf(stdout, "   Show Parameters:                            %s\n", p->showParameters ? "TRUE" : "FALSE" );
    fprintf(stdout, "   Quiet mode:                                 %s\n", p->quietFlag ? "TRUE" : "FALSE" );
    fprintf(stdout, "   Read remote temperature only:               %s\n", p->remoteTempOnly ? "TRUE" : "FALSE");
    fprintf(stdout, "   Remote temperature address:                 %2X\n", p->remoteTempAddr);
    fprintf(stdout, "   Return single magnetometer reading:         %s\n", p->singleRead ? "TRUE" : "FALSE");
    fprintf(stdout, "   Read Simple Magnetometer Support Board:     %i\n", p->boardType);
    fprintf(stdout, "   Verbose output:                             %s\n", p->verboseFlag ? "TRUE" : "FALSE");
    fprintf(stdout, "   Read Board with Extender:                   %i\n", p->boardType);
    fprintf(stdout, "\n\n");
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
    
    p->boardType        = 0;
    p->cc_x             = CC_200;
    p->cc_y             = CC_200;
    p->cc_z             = CC_200;

    p->hideRaw          = FALSE;
    p->i2cBusNumber     = 1;
    p->i2c_fd           = 0;
    p->jsonFlag         = FALSE;

    p->localTempOnly    = FALSE;
    p->localTempAddr    = 0x19;
    p->remoteTempOnly   = FALSE;
    p->remoteTempAddr   = 0x18;  
    p->magnetometerOnly = FALSE;
    p->magnetometerAddr = 0x20;
    p->outDelay         = 1000000;
    p->quietFlag        = TRUE;
    p->showParameters   = FALSE;
    p->singleRead       = FALSE;

    p->x_gain           = GAIN_75;
    p->y_gain           = GAIN_75;
    p->z_gain           = GAIN_75;
    
    p->verboseFlag      = FALSE;
    p->Version          = version;
   
    while((c = getopt(argc, argv, "?b:c:HjlL:mM:o:PqsSrR:vXhqV")) != -1)
    {
        int this_option_optind = optind ? optind : 1;
        switch (c)
        {
            case 'b':
                // fprintf(stdout, "Bus Id: '%s'\n", optarg);
                p->i2cBusNumber = atoi(optarg);
                break;
            case 'c':
                // fprintf(stdout, "CycleCount: '%s'\n", optarg);
                p->cc_x = p->cc_y = p->cc_z = atoi(optarg);
                break;
            case 'H':
                // fprintf(stdout, "Output JSON formatted data.\n");
                p->hideRaw = TRUE;
                break;
            case 'j':
                // fprintf(stdout, "Output JSON formatted data.\n");
                p->jsonFlag = TRUE;
                break;
            case 'l':
                // fprintf(stdout, "Read local temperature only.\n");
                p->localTempOnly = TRUE;
                break;
            case 'L':
                // fprintf(stdout, "Local temp address [default 19 hex]: %s hex\n", optarg);
                p->localTempAddr = atoi(optarg);
                break;
            case 'm':
                // fprintf(stdout, "Read magnetometer only.\n");
                p->magnetometerOnly = TRUE;
                break;
            case 'M':
                // fprintf(stdout, "Magnetometer address [default 20 hex]: %s hex\n", optarg);
                p->magnetometerAddr = atoi(optarg);
                break;
            case 'P':
                // fprintf(stdout, "Magnetometer address [default 20 hex]: %s hex\n", optarg);
                p->showParameters = TRUE;
                break;
            case 'o':
                // fprintf(stdout, "Outout delay rate [default 1000 ms]: %s ms\n", optarg);
                p->outDelay = atoi(optarg) * 1000;
                break;
            case 'q':
                p->quietFlag = TRUE;
                p->verboseFlag = FALSE;
                break;
            case 's':
                // fprintf(stdout, "Return single magnetometer reading.\n");
                p->singleRead = TRUE;
                break;
            case 'S':
                // fprintf(stdout, "Read Simple Magnetometer Support Board.\n");
                p->boardType = 1;
                break;
            case 'r':
                // fprintf(stdout, "Read remote temp only.'\n");
                p->remoteTempOnly = TRUE;
                break;
            case 'R':
                // fprintf(stdout, "Remote temp address [default 18 hex]: %s hex\n", optarg);
                p->remoteTempAddr = atoi(optarg);
                break;
            case 'V':
                // fprintf(stdout, "Version = %s\n", version);
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
                fprintf(stdout, "   -H                     :  Hide raw measurments.\n");
                fprintf(stdout, "   -j                     :  Format output as JSON.\n");
                fprintf(stdout, "   -l                     :  Read local temperature only.\n");
                fprintf(stdout, "   -L [addr as integer]   :  Local temperature address [default 19 hex].\n");
                fprintf(stdout, "   -m                     :  Read magnetometer only.\n");
                fprintf(stdout, "   -M [addr as integer]   :  Magnetometer address [default 20 hex].\n");
                fprintf(stdout, "   -P                     :  Show Parameters.\n");
                fprintf(stdout, "   -q                     :  Quiet mode.\n");
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
    return 0;
}

//------------------------------------------
//  main()
//------------------------------------------
int main(int argc, char** argv)
{
    pList p;

    int32_t rXYZ[3];
    int32_t xyz[3];
    int temp = 0;
    float cTemp = 0.0;
    //char i2cFname[] = ODROIDN2_I2C_BUS;
    int rv = 0;
       
    if((rv = getCommandLine(argc, argv, &p)) != 0)
    {
        return rv;
    }
    if(p.showParameters)
    {
        showSettings(&p);
    }
    
    //fprintf(stdout, "\n\nBefore i2cInit().\n");
    //fflush(stdout);

    i2cOpen(&p);

    //fprintf(stdout, "Before setup_mag().\n");
    //fflush(stdout);
    // Setup the magnetometer.
    // setup_mag(p.verboseFlag, p.i2c_fd, RM3100_I2C_ADDRESS);
    setup_mag(&p);
    
    //fprintf(stdout, "After setup_mag().\n");
    //fflush(stdout);
    
    xyz[0] = (rXYZ[0] / p.x_gain);
    xyz[1] = (rXYZ[1] / p.y_gain);
    xyz[2] = (rXYZ[2] / p.z_gain);
    // loop
    while(1)
    {
        //fprintf(stdout, "Before readTemp()\n");
        //fflush(stdout);
        //

        //  Read temp sensor.
        temp = readTemp(&p, MCP9808_I2CADDR_DEFAULT);
        cTemp = temp * 0.0625;

        // Read Magnetometer.
        readMag(&p, RM3100_I2C_ADDRESS, rXYZ);
        if(!p.hideRaw)
        {
            // Output the results.
            if(!(p.jsonFlag))
            {
                fprintf(stdout, "Time: %ld, Temp: %.2f C   X: %i,  Y: %i,  Z: %i, rX: %i,  rY: %i,  rZ: %i\n", currentTimeMillis(), cTemp, xyz[0], xyz[1], xyz[2], rXYZ[0], rXYZ[1], rXYZ[2]);
            }
            else
            {
                fprintf(stdout, "{ \"%ld\", \"%.2f\", \"%i\",  \"%i\",  \"%i\", \"%i\",  \"%i\",  \"%i\"}\n", currentTimeMillis(), cTemp, xyz[0], xyz[1], xyz[2], rXYZ[0], rXYZ[1], rXYZ[2]);
            }
        }
        else
        {
            // Output the results.
            if(!(p.jsonFlag))
            {
                fprintf(stdout, "Time: %ld, Temp: %.2f C   X: %i,  Y: %i,  Z: %i\n", currentTimeMillis(), cTemp, xyz[0], xyz[1], xyz[2]);
            }
            else
            {
                fprintf(stdout, "{ \"%ld\", \"%.2f\", \"%i\",  \"%i\",  \"%i\"}\n", currentTimeMillis(), cTemp, xyz[0], xyz[1], xyz[2]);
            }
        }

        fflush(stdout);
        if(p.singleRead)
        {
            break;
        }
        // wait 1000 ms for next animation step.
        usleep(p.outDelay);
    }
    i2c_close(p.i2c_fd);
    return 0;
}
