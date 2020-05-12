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


//int setup_mag(pList *p);
//long currentTimeMillis();
//int getTMRCReg(pList *p);
//void setTMRCReg(pList *p);
//int getCMMReg(pList *p);
//void setCMMReg(pList *p);
//unsigned short mag_set_sample_rate(pList *p, unsigned short sample_rate);
//void mag_get_sample_data(int * XYZ);
//int readTemp(pList *p, int devAddr);
//int readMag(pList *p, int32_t *XYZ);
//void readCycleCountRegs(pList *p);
//void setCycleCountRegs(pList *p);
//void showSettings(pList *p);
//int getCommandLine(int argc, char** argv, pList *p);
//int main(int argc, char** argv);

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

//unsigned short mag_set_sample_rate(unsigned short sample_rate)
//{
//    int i;
//    static char i2cbuffer[1];
//    const unsigned short int supported_rates[][2] = \
//    {
//        /* [Hz], register value */
//        {   2, 0x0A},   // up to 2Hz
//        {   4, 0x09},   // up to 4Hz
//        {   8, 0x08},   // up to 8Hz
//        {  16, 0x07},   // up to 16Hz
//        {  31, 0x06},   // up to 31Hz
//        {  62, 0x05},   // up to 62Hz
//        {  125, 0x04},  // up to 125Hz
//        {  220, 0x03}   // up to 250Hz
//    };
//    for(i = 0; i < sizeof(supported_rates)/(sizeof(unsigned short int)*2) - 1; i++)
//    {
//        if(sample_rate <= supported_rates[i][0])
//        {
//            break;
//        }
//    }
//    if (mSensorMode == SensorPowerModeActive) 
//    {
//        mag_disable_interrupts();
//    }
//    mSampleRate = supported_rates[i][0];
//    i2cbuffer[0]= (char)supported_rates[i][1];
//    rm3100_i2c_write(RM3100_TMRC_REG, i2cbuffer, 1);
//    if (mSensorMode == SensorPowerModeActive) 
//    {
//        mag_enable_interrupts();
//    }
//    return mSampleRate;
//
//}
//
///**
// * @fn unsigned short int mag_get_sample_rate();
// *
// * @brief Retrieves the mset sample rate of the sensor.
// *
// * @returns The actual sample rate of the sensor.
// */
//unsigned short mag_get_sample_rate()
//{
//    return mSampleRate;
//}
//

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

////------------------------------------------
//// currentTimeMillis()
////------------------------------------------
//long currentTimeMillis()
//{
//    struct timeval time;
//    gettimeofday(&time, NULL);
//    return time.tv_sec * 1000 + time.tv_usec / 1000;
//}
//
//#define UTC (0) 
//
////------------------------------------------
//// getUTC()
////------------------------------------------
//struct tm *getUTC()
//{
//    time_t now = time(&now);
//    if (now == -1)
//    {
//        puts("The time() function failed");
//    }
//    struct tm *ptm = gmtime(&now);
//    if (ptm == NULL)
//    {
//        puts("The gmtime() function failed");
//    }    
//    return ptm;
//}
//
//
////------------------------------------------
//// listSBCs()
////------------------------------------------
//void listSBCs()
//{
//    int i = 0;
//    fprintf(stdout, "\nList of internally known single board computer types\n  (others may be used by setting -b to specify the bus number required\n\n");
//    fprintf(stdout, " Index        SBC Name                     Path     Bus Number \n");
//    while(busDevs[i].devPath != NULL)
//    {
//        fprintf(stdout, "   %i      %s             %s        %i\n", busDevs[i].enumVal, busDevs[i].SBCString, busDevs[i].devPath, busDevs[i].busNumber);
//        i++;
//    }
//    fprintf(stdout, "\n");
//}
//
////------------------------------------------
//// showSettings()
////------------------------------------------
//void showSettings(pList *p)
//{
//    char pathStr[64] = "";
//    snprintf(pathStr, sizeof(pathStr), "/dev/i2c-%i", p->i2cBusNumber);
//    
//    fprintf(stdout, "\nVersion = %s\n", version);
//    fprintf(stdout, "\nCurrent Parameters:\n\n");
//    //fprintf(stdout, "   Single Board Computer type:                 %i\n",      p->SBCType);
//    //fprintf(stdout, "   Single Board Computer type as string:       %s\n",      p->SBCType == -1 ? "UNKNOWN - using default." : busDevs[p->SBCType].SBCString);
//    //fprintf(stdout, "   I2C bus number as integer:                  %i\n",      p->SBCType == -1 ? 0 : busDevs[p->SBCType].busNumber);
//    //fprintf(stdout, "   I2C bus path as string:                     %s\n",      p->SBCType == -1 ? "/dev/i2c-0" : busDevs[p->SBCType].devPath);
//    fprintf(stdout, "   I2C bus number as integer:                  %i\n",      p->i2cBusNumber);
//    fprintf(stdout, "   I2C bus path as string:                     %s\n",      pathStr);
//    fprintf(stdout, "   Built in self test (BIST) value:            0x%X\n",    p->doBist);
//    fprintf(stdout, "   Cycle count X as integer:                   %i\n",      p->cc_x);
//    fprintf(stdout, "   Cycle count Y as integer:                   %i\n",      p->cc_y);
//    fprintf(stdout, "   Cycle count Z as integer:                   %i\n",      p->cc_z);
//    fprintf(stdout, "   X gain (depends on X CC):                   %i\n",      p->x_gain);
//    fprintf(stdout, "   Y gain (depends on Y CC):                   %i\n",      p->y_gain);
//    fprintf(stdout, "   Z gain (depends on Z CC):                   %i\n",      p->z_gain);
//    fprintf(stdout, "   Read back CC Regs after set:                %s\n",      p->readBackCCRegs ? "TRUE" : "FALSE" );
//    fprintf(stdout, "   Loop Delay (uSec):                          %i\n",      p->outDelay);
//    fprintf(stdout, "   Magnetometer sample rate:                   %i\n",      p->mSampleRate);
//    fprintf(stdout, "   Hide raw measurements:                      %s\n",      p->hideRaw ? "TRUE" : "FALSE" );
//    fprintf(stdout, "   Format output as JSON:                      %s\n",      p->jsonFlag ? "TRUE" : "FALSE" );
//    fprintf(stdout, "   Read local temperature only:                %s\n",      p->localTempOnly ? "TRUE" : "FALSE");
//    fprintf(stdout, "   Local temperature address:                  %2X h\n",   p->localTempAddr);
//    fprintf(stdout, "   Read magnetometer only:                     %s\n",      p->magnetometerOnly ?  "TRUE" : "FALSE");
//    fprintf(stdout, "   Magnetometer address:                       %2X h\n",   p->magnetometerAddr);
//    fprintf(stdout, "   Show Parameters:                            %s\n",      p->showParameters ? "TRUE" : "FALSE" );
//    fprintf(stdout, "   Quiet mode:                                 %s\n",      p->quietFlag ? "TRUE" : "FALSE" );
//    fprintf(stdout, "   Read remote temperature only:               %s\n",      p->remoteTempOnly ? "TRUE" : "FALSE");
//    fprintf(stdout, "   Remote temperature address:                 %2X\n",     p->remoteTempAddr);
//    fprintf(stdout, "   Return single magnetometer reading:         %s\n",      p->singleRead ? "TRUE" : "FALSE");
//    fprintf(stdout, "   Read Simple Magnetometer Support Board:     %i\n",      p->boardType);
//    fprintf(stdout, "   Timestamp format:                           %s\n",      p->tsMilliseconds ? "RAW" : "UTCSTRING");
//    fprintf(stdout, "   Continuous Measurement Mode Data Rate:      %i\n",      p->TMRCRate);
//    fprintf(stdout, "   Verbose output:                             %s\n",      p->verboseFlag ? "TRUE" : "FALSE");
//    fprintf(stdout, "   Show total field:                           %s\n",      p->showTotal ? "TRUE" : "FALSE");
//    fprintf(stdout, "   Read Board with Extender:                   %i\n",      p->boardType);
//    fprintf(stdout, "\n\n");
//}
//
////------------------------------------------
//// getCommandLine()
////------------------------------------------
//int getCommandLine(int argc, char** argv, pList *p)
//{
//    int c;
//    int digit_optind = 0;
//    
//    if(p != NULL)
//    {
//        memset(p, 0, sizeof(pList));
//    }
//    
//    p->SBCType          = eRASPI_I2C_BUS;
//    p->boardType        = 0;
//    p->boardMode        = LOCAL;
//    p->doBist           = FALSE;
//
//    p->cc_x             = CC_200;
//    p->cc_y             = CC_200;
//    p->cc_z             = CC_200;
//    p->x_gain           = GAIN_75;
//    p->y_gain           = GAIN_75;
//    p->z_gain           = GAIN_75;
//    
//    p->readBackCCRegs   = FALSE;
//    p->mSampleRate      = 100;
//    p->hideRaw          = FALSE;
//    //p->i2cBusNumber     = 1;
//    p->i2cBusNumber     = busDevs[eRASPI_I2C_BUS].busNumber;
//    p->i2c_fd           = 0;
//    p->jsonFlag         = FALSE;
//
//    p->localTempOnly    = FALSE;
//    p->localTempAddr    = 0x19;
//    p->remoteTempOnly   = FALSE;
//    p->remoteTempAddr   = 0x18;  
//    p->magnetometerOnly = FALSE;
//    p->magnetometerAddr = 0x20;
//    p->outDelay         = 1000000;
//    p->quietFlag        = TRUE;
//    p->showParameters   = FALSE;
//    p->singleRead       = FALSE;
//    p->tsMilliseconds   = FALSE;
//    p->TMRCRate         = 96;
//    p->verboseFlag      = FALSE;
//    p->showTotal        = FALSE;
//    p->Version          = version;
//   
//    while((c = getopt(argc, argv, "?ab:B:c:Cd:D:HhjlL:mM:o:PqrR:sSTt:vXxYyhqVZ")) != -1)
//    {
//        int this_option_optind = optind ? optind : 1;
//        switch (c)
//        {
//            case 'a':
//                listSBCs();
//                break;
//            case 'b':
//                p->i2cBusNumber = atoi(optarg);
//                //p->SBCType = -1;
//                break;
//            case 'B':
//                p->doBist = atoi(optarg);
//                printf("Not implemented yet.");
//                break;
//            case 'c':
//                p->cc_x = p->cc_y = p->cc_z = atoi(optarg);
//                break;
//            case 'C':
//                p->readBackCCRegs = TRUE;
//                break;
//            case 'd':
//                p->outDelay = atoi(optarg);
//                break;
//            case 'D':
//                p->mSampleRate = atoi(optarg);
//                break;
//            case 'H':
//                p->hideRaw = TRUE;
//                break;
//            case 'j':
//                p->jsonFlag = TRUE;
//                break;
//            case 'l':
//                p->localTempOnly = TRUE;
//                break;
//            case 'L':
//                p->localTempAddr = atoi(optarg);
//                break;
//            case 'm':
//                p->magnetometerOnly = TRUE;
//                break;
//            case 'M':
//                p->magnetometerAddr = atoi(optarg);
//                break;
//            case 'P':
//                p->showParameters = TRUE;
//                break;
//            case 'o':
//                p->outDelay = atoi(optarg) * 1000;
//                break;
//            case 'q':
//                p->quietFlag = TRUE;
//                p->verboseFlag = FALSE;
//                break;
//            case 'r':
//                p->remoteTempOnly = TRUE;
//                break;
//            case 'R':
//                p->remoteTempAddr = atoi(optarg);
//                break;
//            case 's':
//                p->singleRead = TRUE;
//                break;
//            case 'S':
//                //p->boardType = 1;
//                break;
//            case 'T':
//                p->tsMilliseconds = TRUE;
//                break;
//            case 't':
//                p->TMRCRate = atoi(optarg);
//                break;
//            case 'V':
//                return 1;
//                break;
//            case 'v':
//                p->verboseFlag = TRUE;
//                p->quietFlag = FALSE;
//                break;
//            case 'X':
//                p->boardType = 0;
//                p->boardMode = LOCAL;
//                break;
//            case 'x':
//                p->boardType = 1;
//                p->boardMode = REMOTE;
//                break;
//            case 'Y':
//                p->boardType = 2;
//                p->boardMode = LOCAL;
//                break;
//            case 'y':
//                p->boardType = 3;
//                p->boardMode = REMOTE;
//                break;
//            case 'Z':
//                p->showTotal = TRUE;
//                break;
//            case 'h':
//            case '?':
//                fprintf(stdout, "\n%s Version = %s\n", argv[0], version);
//                fprintf(stdout, "\nParameters:\n\n");
//                fprintf(stdout, "   -a                     :  List known SBCs\n");
//                fprintf(stdout, "   -B <reg mask>          :  Do built in self test (BIST). [Not really implemented].\n");
//                fprintf(stdout, "   -b <bus as integer>    :  I2C bus number as integer.\n");
//                fprintf(stdout, "   -C                     :  Read back cycle count registers before sampling.\n");
//                fprintf(stdout, "   -c <count>             :  Set cycle counts as integer (default 200).\n");
//                fprintf(stdout, "   -D <rate>              :  Set magnetometer sample rate [TMRC].\n");
//                fprintf(stdout, "   -d <count>             :  Set polling delay (default 1000000 uSec).\n");
//                fprintf(stdout, "   -H                     :  Hide raw measurments.\n");
//                fprintf(stdout, "   -j                     :  Format output as JSON.\n");
//                fprintf(stdout, "   -L [addr as integer]   :  Local temperature address (default 19 hex).\n");
//                fprintf(stdout, "   -l                     :  Read local temperature only.  [Not really implemented].\n");
//                fprintf(stdout, "   -M [addr as integer]   :  Magnetometer address (default 20 hex).\n");
//                fprintf(stdout, "   -m                     :  Read magnetometer only.\n");
//                fprintf(stdout, "   -P                     :  Show Parameters.\n");
//                fprintf(stdout, "   -q                     :  Quiet mode.                   [Not really implemented].\n");
//                fprintf(stdout, "   -r                     :  Read remote temperature only.\n");
//                fprintf(stdout, "   -R [addr as integer]   :  Remote temperature address (default 18 hex).\n");
//                fprintf(stdout, "   -s                     :  Return single reading.\n");
//                fprintf(stdout, "   -T                     :  Raw timestamp in milliseconds (default: UTC string).\n");
//                fprintf(stdout, "   -t                     :  Get/Set Continuous Measurement Mode Data Rate.\n");
//                fprintf(stdout, "   -V                     :  Display software version and exit.\n");
//                fprintf(stdout, "   -v                     :  Verbose output.\n");
//                fprintf(stdout, "   -X                     :  Read Simple Magnetometer Board (SMSB).\n");
//                fprintf(stdout, "   -x                     :  Read board with extender (MSBx).\n");
//                fprintf(stdout, "   -Y                     :  Read Scotty's RPi Mag HAT standalone. [UNTESTED]\n");
//                fprintf(stdout, "   -y                     :  Read Scotty's RPi Mag HAT in extended mode. [UNTESTED]\n");
//                fprintf(stdout, "   -Z                     :  Show total field. sqrt((x*x) + (y*y) + (z*z))\n");
//                fprintf(stdout, "   -h or -?               :  Display this help.\n\n");
//                return 1;
//                break;
//            default:
//                fprintf(stdout, "?? getopt returned character code 0x%2X ??\n", c);
//                break;
//        }
//    }
//    if(optind < argc)
//    {
//        printf ("non-option ARGV-elements: ");
//        while (optind < argc)
//        {
//            printf ("%s ", argv[optind++]);
//        }
//        printf ("\n");
//    }
//    return 0;
//}
//
////------------------------------------------
//// readTemp()
////------------------------------------------
//int readTemp(pList *p,int devAddr)
//{
//    int temp = -99;
//    char data[2] = {0};
//    char reg[1] = {MCP9808_REG_AMBIENT_TEMP};
//
//    i2c_setAddress(p->i2c_fd, devAddr);
//    write(p->i2c_fd, reg, 1);
//    if(read(p->i2c_fd, data, 2) != 2)
//    {
//        printf("Error : Input/Output error \n");
//    }
//    else
//    {
//        // Convert the data to 13-bits
//        temp = ((data[0] & 0x1F) * 256 + data[1]);
//        if(temp > 4095)
//        {
//            temp -= 8192;
//        }
//    }
//    return temp;
//}
//
////------------------------------------------
//// readMag()
////------------------------------------------
//int readMag(pList *p, int32_t *XYZ)
//{
//    int rv = 0;
//    int bytes_read = 0;
//    short cmmMode = (CMMMODE_ALL);   // 71 d
//
//    i2c_setAddress(p->i2c_fd, p->magnetometerAddr);
//
//#if DEBUG
//    printf("Write RM3100I2C_CMM.  Mode: 0x%x. (AKA Beacon)\n", cmmMode);
//#endif    
//    // Write command to  use polling.
//    i2c_write(p->i2c_fd, RM3100I2C_CMM, cmmMode);    // Start CMM on X, Y, Z
//#if DEBUG
//    printf("Waiting for DRDY...\n");
//#endif
//    // Check if DRDY went high and wait unit high before reading results
//    while((rv = (p->i2c_fd, i2c_read(p->i2c_fd, RM3100I2C_STATUS)) & RM3100I2C_READMASK) != RM3100I2C_READMASK)
//    {
//    }
//#if DEBUG
//    printf("Got DRDY...\n");
//#endif    
//    // Read the XYZ registers
//    if((bytes_read = i2c_readbuf(p->i2c_fd, RM3100I2C_XYZ, (unsigned char*) &mSamples, sizeof(mSamples)/sizeof(char))) != sizeof(mSamples)/sizeof(char))
//    {
//        perror("i2c transaction i2c_readbuf() failed.\n");
//    }
//#if DEBUG
//    printf("After i2c_readbuf()...\n");
//#endif    
//
//    XYZ[0] = ((signed char)mSamples[0]) * 256 * 256;
//    XYZ[0] |= mSamples[1] * 256;
//    XYZ[0] |= mSamples[2];
//    XYZ[1] = ((signed char)mSamples[3]) * 256 * 256;
//    XYZ[1] |= mSamples[4] * 256;
//    XYZ[1] |= mSamples[5];
//    XYZ[2] = ((signed char)mSamples[6]) * 256 * 256;
//    XYZ[2] |= mSamples[7] * 256;
//    XYZ[2] |= mSamples[8];
//
//    return bytes_read;
//}
//
//#define OUTBUFLEN 60
//
////------------------------------------------
////  main()
////------------------------------------------
//int main(int argc, char** argv)
//{
//    pList p;
//    char outStr[OUTBUFLEN] = "";
//    int32_t rXYZ[3];
//    int32_t xyz[3];
//    int temp = 0;
//    float cTemp = 0.0;
//    int rv = 0;
//    struct tm *utcTime = getUTC();
//
//    if((rv = getCommandLine(argc, argv, &p)) != 0)
//    {
//        return rv;
//    }
//    if(p.verboseFlag)
//    {
//        printf("\nUTC time: %s", asctime(utcTime));
//    }
//    if(p.showParameters)
//    {
//        showSettings(&p);
//    }
//
//    // Open I2C bus (only one at a time for now)    
//    i2c_open(&p);
//
//    // Setup the magnetometer.
//    setup_mag(&p);
//    mag_set_sample_rate(&p, 100);
//    // TMRC Reg mysteriously gets bolluxed up, probably setting CMM rates.  Brute force fix for now.
//    p.TMRCRate = 0x96;
//    setTMRCReg(&p);
//    printf("TMRS reg value: %i\n", getTMRCReg(&p));
//    if(p.readBackCCRegs)
//    {
//        readCycleCountRegs(&p);
//    }
//    // loop
//    while(1)
//    {
//        //  Read temp sensor.
//        if(!p.magnetometerOnly)
//        {
//            temp = readTemp(&p, MCP9808_I2CADDR_DEFAULT);
//            cTemp = temp * 0.0625;
//        }
// 
//        // Read Magnetometer.
//        if(!p.localTempOnly)
//        {
//            readMag(&p,rXYZ);
//            xyz[0] = (rXYZ[0] / p.x_gain);
//            xyz[1] = (rXYZ[1] / p.y_gain);
//            xyz[2] = (rXYZ[2] / p.z_gain);
//        }
//    
//        // Output the results.
//        if(!(p.jsonFlag))
//        {
//            if(p.tsMilliseconds)
//            {
//                fprintf(stdout, "Time Stamp: %ld, ", currentTimeMillis());
//            }
//            else
//            {
//                utcTime = getUTC();
//                strftime(outStr, OUTBUFLEN, "%d %b %Y %T", utcTime);                
//                fprintf(stdout, "Time: %s", outStr);
//            }
//            fprintf(stdout, ", Temp: %.2f",    cTemp);
//            fprintf(stdout, ", x: %i",           xyz[0]);
//            fprintf(stdout, ", y: %i",           xyz[1]);
//            fprintf(stdout, ", z: %i",           xyz[2]);
//            if(!p.hideRaw)
//            {
//                fprintf(stdout, ", rx: %i",           rXYZ[0]);
//                fprintf(stdout, ", ry: %i",           rXYZ[1]);
//                fprintf(stdout, ", rz: %i",           rXYZ[2]);
//            }
//            if(p.showTotal)
//            {
//                fprintf(stdout, ", Tm: %.0f",           sqrt((xyz[0] * xyz[0]) + (xyz[1] * xyz[1]) + (xyz[2] * xyz[2])));
//            }
//            fprintf(stdout, "\n");
//        }
//        else
//        {
//            fprintf(stdout, "{ ");
//            if(p.tsMilliseconds)
//            {
//                fprintf(stdout, "\"%ld\", ",  currentTimeMillis());
//            }
//            else
//            {
//                utcTime = getUTC();
//                strftime(outStr, OUTBUFLEN, "%d %b %Y %T", utcTime);        // RFC 2822: "%a, %d %b %Y %T %z"      RFC 822: "%a, %d %b %y %T %z"  
//                fprintf(stdout, "ts:\"%s\", ", outStr);
//            }
//            fprintf(stdout, " lt:\"%.2f\"",  cTemp);
//            fprintf(stdout, ", x:\"%i\" ",    xyz[0]);
//            fprintf(stdout, ", y:\"%i\" ",    xyz[1]);
//            fprintf(stdout, ", z:\"%i\"",     xyz[2]);
//            if(!p.hideRaw)
//            {
//                fprintf(stdout, ", rx:\"%i\"",    rXYZ[0]);
//                fprintf(stdout, ", ry:\"%i\"",    rXYZ[1]);
//                fprintf(stdout, ", rz:\"%i\"",     rXYZ[2]);
//            }
//            if(p.showTotal)
//            {
//                fprintf(stdout, ", Tm: \"%.0f\"",  sqrt((xyz[0] * xyz[0]) + (xyz[1] * xyz[1]) + (xyz[2] * xyz[2])));
//            }
//           fprintf(stdout, " }\n");
//        }
//        fflush(stdout);
//        if(p.singleRead)
//        {
//            break;
//        }
//        // wait p.outDelay (1000 ms default) for next poll.
//        usleep(p.outDelay);
//    }
//    i2c_close(p.i2c_fd);
//    return 0;
//}
//
