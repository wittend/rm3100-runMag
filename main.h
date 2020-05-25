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
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>
#include <linux/i2c-dev.h>
#include "device_defs.h"
#include "i2c.h"
#include "MCP9808.h"

#define RUNMAG_VERSION "0.0.5"

//------------------------------------------
// Parameter List struct
//------------------------------------------
typedef struct tag_pList
{
    int SBCType;
    int boardType;
    int boardMode;
    int doBistMask; 

    int cc_x;
    int cc_y;
    int cc_z;
 
    int x_gain;
    int y_gain;
    int z_gain;
    int NOSRegValue;
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
    int outDelay;
    int quietFlag;
    int remoteTempOnly;
    int remoteTempAddr;
    int showParameters;
    int singleRead;
    int tsMilliseconds;
    int TMRCRate;
    int verboseFlag;
    int mSampleRate;
    int showTotal;

    char *Version;
} pList;

//------------------------------------------
// Prototypes
//------------------------------------------
long currentTimeMillis();
struct tm *getUTC();
void listSBCs();
void showCountGainRelationship();
int readConfigFromFile(pList *p, char *cfgFile);
int saveConfigToFile(pList *p, char *cfgFile);
void showSettings(pList *p);
int getCommandLine(int argc, char** argv, pList *p);
int readTemp(pList *p, int devAddr);
int readMag(pList *p, int devAddr, int32_t *XYZ);
int main(int argc, char** argv);

#endif //SWX3100MAIN_h