//=========================================================================
// main.h
// 
// An interface for the RM3100 3-axis magnetometer from PNI Sensor Corp.
// 
// Author:      David Witten, KD0EAG
// Date:        April 21, 2020
// License:     GPL 3.0
//=========================================================================
#ifndef SWX3100MAIN_h
#define SWX3100MAIN_h

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <memory.h>
#include <linux/i2c-dev.h>
#include "device_defs.h"
#include "i2c.h"
#include "MCP9808.h"
#include "uthash/uthash.h"

#define _DEBUG 0

#define RUNMAG_VERSION "0.0.10"
#define UTCBUFLEN 64
#define MAXPATHBUFLEN 1025
#define JSONBUFLEN 1025
#define JSONBUFTOKENCOUNT 1024
#define SITEPREFIXLEN 32

//------------------------------------------
// Parameter List struct
//------------------------------------------
typedef struct tag_pList
{
    int SBCType;
    int boardType;
    int boardMode;
    int doBistMask; 
    int buildLogPath;
    
    int cc_x;
    int cc_y;
    int cc_z;
 
    int x_gain;
    int y_gain;
    int z_gain;
    
    int TMRCRate;
    int mSampleRate;

    int samplingMode;

    int NOSRegValue;
    
    int DRDYdelay;

    int readBackCCRegs;
    int magRevId;
    
    int hideRaw;
    int i2cBusNumber;
    int i2c_fd;
    int jsonFlag;

    int localTempOnly;
    int localTempAddr;

    int magnetometerOnly;
    int magnetometerAddr;

    int remoteTempOnly;
    int remoteTempAddr;

    int outDelay;
    int quietFlag;
    int showParameters;
    int singleRead;
    int tsMilliseconds;
    int verboseFlag;
    int showTotal;
    char *outputFilePath;
    char *sitePrefix;
    char *logOutputTime;
    int  logOutput;
    char *Version;
} pList;

//-------------------------------------------
// Device paths for different platforms
//-------------------------------------------
#define OTHER_BUS               ""
#define KHADAS_EDGE_I2C3        "/dev/i2c-3"
#define KHADAS_VIM3_I2C3        "/dev/i2c-3"
#define KHADAS_VIM3_I2C4        "/dev/i2c-4"
#define ODROIDC0_I2C_BUS        "/dev/i2c-1"
#define ODROIDC1_I2C_BUS        "/dev/i2c-1"
#define ODROIDC2_I2C_BUS        "/dev/i2c-1"
#define ODROIDC4_I2C_BUS        "/dev/i2c-2"
#define ODROIDC4_I2C_BUS3       "/dev/i2c-3"
#define ODROIDN2_I2C_BUS        "/dev/i2c-2"
#define ODROIDN2_I2C_BUS3       "/dev/i2c-3"
#define ODROIDN2PLUS_I2C_BUS0   "/dev/i2c-0"
#define ODROIDN2PLUS_I2C_BUS1   "/dev/i2c-1"
#define NV_XAVIER_I2C_BUS       "/dev/i2c-8"
#define NV_NANO_I2C_BUS         "/dev/i2c-1"
#define RASPI_I2C_BUS           "/dev/i2c-1"

//-------------------------------------------
// Known SBC device platforms 
//-------------------------------------------
#define sOTHER_BUS              "Other             "
#define sKHADAS_EDGE_BUS3       "KHADAS EDGE bus 3 "
#define sVIM3_I2C_BUS3          "KHADAS VIM3 bus 3 "
#define sVIM3_I2C_BUS4          "KHADAS VIM3 bus 4 "
#define sNV_XAVIER_I2C_BUS      "NV Xavier   bus 8 "
#define sNV_NANO_I2C_BUS        "NV Nano     bus 1 "
#define sODROIDC0_I2C_BUS       "Odroid CO   bus 1 "
#define sODROIDC1_I2C_BUS       "Odroid C1   bus 1 "
#define sODROIDC2_I2C_BUS       "Odroid C2   bus 1 "
#define sODROIDC4_I2C_BUS       "Odroid C4   bus 1 "
#define sODROIDC4_I2C_BUS3      "Odroid C4   bus 3 "
#define sODROIDN2_I2C_BUS       "Odroid N2   bus 2 "
#define sODROIDN2_I2C_BUS3      "Odroid N2   bus 3 "
#define sODROIDN2PLUS_I2C_BUS0  "Odroid N2+  bus 0 "
#define sODROIDN2PLUS_I2C_BUS1  "Odroid N2+  bus 1 "
#define sRASPI_I2C_BUS          "Raspberry Pi 3/4  "

//-------------------------------------------
// Device paths for different platforms
//-------------------------------------------
typedef enum
{
    eKHADAS_EDGE_I2C3 = 0,
    eVIM3_I2C_BUS3,
    eVIM3_I2C_BUS4,
    eNV_XAVIER_I2C_BUS,
    eNV_NANO_I2C_BUS,
    eODROIDC0_I2C_BUS,
    eODROIDC1_I2C_BUS,
    eODROIDC2_I2C_BUS,
    eODROIDC4_I2C_BUS,
    eODROIDC4_I2C_BUS3,
    eODROIDN2_I2C_BUS,
    eODROIDN2_I2C_BUS3,
    eODROIDN2PLUS_I2C_BUS0,
    eODROIDN2PLUS_I2C_BUS1,
    eRASPI_I2C_BUS,
} i2cBusEnum;

//-------------------------------------------
// I2C Bus Path entries
//-------------------------------------------
struct busDev
{
    const char *devPath;
    const char *SBCString;
    int         enumVal;
    int         busNumber;
};

//------------------------------------------
// Prototypes
//------------------------------------------
int readTemp(pList *p, int devAddr);
int readMagCMM(pList *p, int devAddr, int32_t *XYZ);
int readMagPOLL(pList *p, int devAddr, int32_t *XYZ);
int main(int argc, char** argv);

#endif //SWX3100MAIN_h