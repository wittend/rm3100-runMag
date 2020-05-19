//=========================================================================
// main.c
// 
// A command line interface for the RM3100 3-axis magnetometer from PNI Sensor Corp.
// Derived in part from several sources:
//      HTTPS://github.com/miguelrasteiro/RM3100.X
//      Jeremiah Mattison / Rm3100: https://os.mbed.com/users/fwrawx/code/Rm3100/
//      HTTPS://github.com/shannon-jia/rm3100
//      Song Qiang <songqiang1304521@gmail.com (Linux driver):
//          Linux kernel driver: HTTPS://github.com/torvalds/linux/tree/v5.3/drivers/iio/magnetometer
// 
// Author:      David Witten, KD0EAG
// Date:        May 12, 2020
// License:     GPL 3.0
//=========================================================================
#include "device_defs.h"
#include "i2c.h"
#include "MCP9808.h"
#include "runMag.h"
#include "main.h"

//------------------------------------------
// Static variables 
//------------------------------------------
char version[] = SIMPLEI2C_VERSION;
static char  mSamples[9];

#define DEBUG 0
#define UTCBUFLEN 64

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
// getUTC()
//------------------------------------------
struct tm *getUTC()
{
    time_t now = time(&now);
    if (now == -1)
    {
        puts("The time() function failed");
    }
    struct tm *ptm = gmtime(&now);
    if (ptm == NULL)
    {
        puts("The gmtime() function failed");
    }    
    return ptm;
}


//------------------------------------------
// listSBCs()
//------------------------------------------
void listSBCs()
{
    int i = 0;
    fprintf(stdout, "\nList of some known single board computer types\n  For default distibutions of Linux.\n  Remember, these may be remapped (or not mapped at all) by the device tree.\n  (use -b to specify the bus number required\n\n");
    fprintf(stdout, " Index        SBC Name                        Path      Bus Number \n");
    while(busDevs[i].devPath != NULL)
    {
        fprintf(stdout, "   %2i      %s             %s        %i\n", busDevs[i].enumVal, busDevs[i].SBCString, busDevs[i].devPath, busDevs[i].busNumber);
        i++;
    }
    fprintf(stdout, "\n");
    exit(0);
}

//------------------------------------------
// showSettings()
//------------------------------------------
void showSettings(pList *p)
{
    char pathStr[128] = "";
    snprintf(pathStr, sizeof(pathStr), "/dev/i2c-%i", p->i2cBusNumber);
    
    fprintf(stdout, "\nVersion = %s\n", version);
    fprintf(stdout, "\nCurrent Parameters:\n\n");
    //fprintf(stdout, "   Single Board Computer type:                 %i\n",      p->SBCType);
    //fprintf(stdout, "   Single Board Computer type as string:       %s\n",      p->SBCType == -1 ? "UNKNOWN - using default." : busDevs[p->SBCType].SBCString);
    //fprintf(stdout, "   I2C bus number as integer:                  %i\n",      p->SBCType == -1 ? 0 : busDevs[p->SBCType].busNumber);
    //fprintf(stdout, "   I2C bus path as string:                     %s\n",      p->SBCType == -1 ? "/dev/i2c-0" : busDevs[p->SBCType].devPath);
    fprintf(stdout, "   I2C bus number as integer:                  %i\n",      p->i2cBusNumber);
    fprintf(stdout, "   I2C bus path as string:                     %s\n",      pathStr);
    fprintf(stdout, "   Built in self test (BIST) value:            %2X hex\n", p->doBistMask);
    fprintf(stdout, "   Cycle count X as integer:                   %i\n",      p->cc_x);
    fprintf(stdout, "   Cycle count Y as integer:                   %i\n",      p->cc_y);
    fprintf(stdout, "   Cycle count Z as integer:                   %i\n",      p->cc_z);
    fprintf(stdout, "   X gain (depends on X CC):                   %i\n",      p->x_gain);
    fprintf(stdout, "   Y gain (depends on Y CC):                   %i\n",      p->y_gain);
    fprintf(stdout, "   Z gain (depends on Z CC):                   %i\n",      p->z_gain);
    fprintf(stdout, "   Read back CC Regs after set:                %s\n",      p->readBackCCRegs   ? "TRUE" : "FALSE" );
    fprintf(stdout, "   Polling Loop Delay (uSec):                  %i\n",      p->outDelay);
    fprintf(stdout, "   Magnetometer sample rate:                   %i\n",      p->mSampleRate);
    fprintf(stdout, "   CMM magnetometer sample rate (TMRC reg):    %i\n",      p->TMRCRate);
    fprintf(stdout, "   Format output as JSON:                      %s\n",      p->jsonFlag         ? "TRUE" : "FALSE" );
    fprintf(stdout, "   Read local temperature only:                %s\n",      p->localTempOnly    ? "TRUE" : "FALSE");
    fprintf(stdout, "   Read remote temperature only:               %s\n",      p->remoteTempOnly   ? "TRUE" : "FALSE");
    fprintf(stdout, "   Read magnetometer only:                     %s\n",      p->magnetometerOnly ? "TRUE" : "FALSE");
    fprintf(stdout, "   Local temperature address:                  %2X hex\n", p->localTempAddr);
    fprintf(stdout, "   Remote temperature address:                 %2X hex\n", p->remoteTempAddr);
    fprintf(stdout, "   Magnetometer address:                       %2X hex\n", p->magnetometerAddr);
    fprintf(stdout, "   Show parameters:                            %s\n",      p->showParameters   ? "TRUE" : "FALSE" );
    fprintf(stdout, "   Quiet mode:                                 %s\n",      p->quietFlag        ? "TRUE" : "FALSE" );
    fprintf(stdout, "   Hide raw measurements:                      %s\n",      p->hideRaw          ? "TRUE" : "FALSE" );
    fprintf(stdout, "   Return single magnetometer reading:         %s\n",      p->singleRead       ? "TRUE" : "FALSE");
    fprintf(stdout, "   Read Simple Magnetometer Board (SMSB):      %i\n",      p->boardType);
    fprintf(stdout, "   Read Board with Extender (MSBx:             %i\n",      p->boardType);
    fprintf(stdout, "   Timestamp format:                           %s\n",      p->tsMilliseconds   ? "RAW"  : "UTCSTRING");
    fprintf(stdout, "   Verbose output:                             %s\n",      p->verboseFlag      ? "TRUE" : "FALSE");
    fprintf(stdout, "   Show total field:                           %s\n",      p->showTotal        ? "TRUE" : "FALSE");
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
    
    p->SBCType          = eRASPI_I2C_BUS;
    p->boardType        = 0;
    p->boardMode        = LOCAL;
    p->doBistMask       = FALSE;

    p->cc_x             = CC_200;
    p->cc_y             = CC_200;
    p->cc_z             = CC_200;
    p->x_gain           = GAIN_75;
    p->y_gain           = GAIN_75;
    p->z_gain           = GAIN_75;
    
    p->readBackCCRegs   = FALSE;
    p->mSampleRate      = 100;
    p->hideRaw          = FALSE;
    //p->i2cBusNumber     = 1;
    p->i2cBusNumber     = busDevs[eRASPI_I2C_BUS].busNumber;
    p->i2c_fd           = 0;
    p->jsonFlag         = FALSE;

    p->localTempOnly    = FALSE;
    p->localTempAddr    = MCP9808_LCL_I2CADDR_DEFAULT;
    p->remoteTempOnly   = FALSE;
    p->remoteTempAddr   = MCP9808_RMT_I2CADDR_DEFAULT;  
    p->magnetometerOnly = FALSE;
    p->magnetometerAddr = RM3100_I2C_ADDRESS;
    p->outDelay         = 1000000;
    p->quietFlag        = TRUE;
    p->showParameters   = FALSE;
    p->singleRead       = FALSE;
    p->tsMilliseconds   = FALSE;
    p->TMRCRate         = 96;
    p->verboseFlag      = FALSE;
    p->showTotal        = FALSE;
    p->Version          = version;
   
    while((c = getopt(argc, argv, "?ab:B:c:Cd:D:HhjlL:mM:o:PqrR:sSTt:XxYyvVZ")) != -1)
    {
        int this_option_optind = optind ? optind : 1;
        switch (c)
        {
            case 'a':
                listSBCs();
                break;
            case 'b':
                p->i2cBusNumber = atoi(optarg);
                //p->SBCType = -1;
                break;
            case 'B':
                p->doBistMask = atoi(optarg);
                // printf("Not implemented yet.");
                break;
            case 'c':
                p->cc_x = p->cc_y = p->cc_z = atoi(optarg);
                break;
            case 'C':
                p->readBackCCRegs = TRUE;
                break;
            case 'd':
                p->outDelay = atoi(optarg);
                break;
            case 'D':
                p->mSampleRate = atoi(optarg);
                break;
            case 'H':
                p->hideRaw = TRUE;
                break;
            case 'j':
                p->jsonFlag = TRUE;
                break;
            case 'l':
                p->localTempOnly = TRUE;
                break;
            case 'L':
                p->localTempAddr = atoi(optarg);
                break;
            case 'm':
                p->magnetometerOnly = TRUE;
                break;
            case 'M':
                p->magnetometerAddr = atoi(optarg);
                break;
            case 'o':
                p->outDelay = atoi(optarg) * 1000;
                break;
            case 'P':
                p->showParameters = TRUE;
                break;
            case 'q':
                p->quietFlag = TRUE;
                p->verboseFlag = FALSE;
                break;
            case 'r':
                p->remoteTempOnly = TRUE;
                break;
            case 'R':
                p->remoteTempAddr = atoi(optarg);
                break;
            case 's':
                p->singleRead = TRUE;
                break;
            case 'S':
                //p->boardType = 1;
                break;
            case 'T':
                p->tsMilliseconds = TRUE;
                break;
            case 't':
                p->TMRCRate = atoi(optarg);
                break;
            case 'V':
                return 1;
                break;
            case 'v':
                p->verboseFlag = TRUE;
                p->quietFlag = FALSE;
                break;
            case 'X':
                p->boardType = 0;
                p->boardMode = LOCAL;
                break;
            case 'x':
                p->boardType = 1;
                p->boardMode = REMOTE;
                break;
            case 'Y':
                p->boardType = 2;
                p->boardMode = LOCAL;
                break;
            case 'y':
                p->boardType = 3;
                p->boardMode = REMOTE;
                break;
            case 'Z':
                p->showTotal = TRUE;
                break;
            case 'h':
            case '?':
                fprintf(stdout, "\n%s Version = %s\n", argv[0], version);
                fprintf(stdout, "\nParameters:\n\n");
                fprintf(stdout, "   -a                     :  List known SBC I2C bus numbers (use with -b).\n");
                fprintf(stdout, "   -B <reg mask>          :  Do built in self test (BIST).             [Not yet implemented].\n");
                fprintf(stdout, "   -b <bus as integer>    :  I2C bus number as integer.\n");
                fprintf(stdout, "   -C                     :  Read back cycle count registers before sampling.\n");
                fprintf(stdout, "   -c <count>             :  Set cycle counts as integer  (default 200).\n");
                fprintf(stdout, "   -D <rate>              :  Set magnetometer sample rate (TMRC reg).\n");
                fprintf(stdout, "   -d <count>             :  Set polling delay (default 1000000 uSec).\n");
                fprintf(stdout, "   -H                     :  Hide raw measurments.\n");
                fprintf(stdout, "   -j                     :  Format output as JSON.\n");
                fprintf(stdout, "   -L [addr as integer]   :  Local temperature address (default 19 hex).\n");
                fprintf(stdout, "   -l                     :  Read local temperature only.               [testing].\n");
                fprintf(stdout, "   -M [addr as integer]   :  Magnetometer address (default 20 hex).\n");
                fprintf(stdout, "   -m                     :  Read magnetometer only.\n");
                fprintf(stdout, "   -P                     :  Show Parameters.\n");
                fprintf(stdout, "   -q                     :  Quiet mode.                                [partial].\n");
                fprintf(stdout, "   -v                     :  Verbose output.\n");
                fprintf(stdout, "   -r                     :  Read remote temperature only.\n");
                fprintf(stdout, "   -R [addr as integer]   :  Remote temperature address (default 18 hex).\n");
                fprintf(stdout, "   -s                     :  Return single reading.\n");
                fprintf(stdout, "   -T                     :  Raw timestamp in milliseconds (default: UTC string).\n");
                fprintf(stdout, "   -t                     :  Get/Set Continuous Measurement Mode Data Rate.\n");
                fprintf(stdout, "   -V                     :  Display software version and exit.\n");
                fprintf(stdout, "   -X                     :  Read Simple Magnetometer Board (SMSB).\n");
                fprintf(stdout, "   -x                     :  Read board with extender (MSBx).\n");
                fprintf(stdout, "   -Y                     :  Read Scotty's RPi Mag HAT standalone.       [UNTESTED]\n");
                fprintf(stdout, "   -y                     :  Read Scotty's RPi Mag HAT in extended mode. [UNTESTED]\n");
                fprintf(stdout, "   -Z                     :  Show total field. sqrt((x*x) + (y*y) + (z*z))\n");
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
// readTemp()
//------------------------------------------
int readTemp(pList *p, int devAddr)
{
    int temp = -9999;
    char data[2] = {0};
    char reg[1] = {MCP9808_REG_AMBIENT_TEMP};

    i2c_setAddress(p->i2c_fd, devAddr);
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
// readMag()
//------------------------------------------
int readMag(pList *p, int devAddr, int32_t *XYZ)
{
    int rv = 0;
    int bytes_read = 0;
    short cmmMode = (CMMMODE_ALL);   // 71 d

    i2c_setAddress(p->i2c_fd, devAddr);
    // Write command to  use polling.
    i2c_write(p->i2c_fd, RM3100I2C_CMM, cmmMode);    // Start CMM on X, Y, Z
    // Check if DRDY went high and wait unit high before reading results
    while((rv = (p->i2c_fd, i2c_read(p->i2c_fd, RM3100I2C_STATUS)) & RM3100I2C_READMASK) != RM3100I2C_READMASK)
    {
    }
    // Read the XYZ registers
    if((bytes_read = i2c_readbuf(p->i2c_fd, RM3100I2C_XYZ, (unsigned char*) &mSamples, sizeof(mSamples)/sizeof(char))) != sizeof(mSamples)/sizeof(char))
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
//  main()
//------------------------------------------
int main(int argc, char** argv)
{
    pList p;
    char utcStr[UTCBUFLEN] = "";
    int32_t rXYZ[3];
    int32_t xyz[3];
    int temp = 0;
    float lcTemp = 0.0;
    float rcTemp = 0.0;
    int rv = 0;
    struct tm *utcTime = getUTC();

    if((rv = getCommandLine(argc, argv, &p)) != 0)
    {
        return rv;
    }
    // BIST - Built In Self Test
    if(p.doBistMask)
    {
        fprintf(stdout, "\nBIST using mask: %2X returns: %2X\n", p.doBistMask, runBIST(&p));
        exit(0);
    }
    // if Verbose == TRUE
    if(p.verboseFlag)
    {
        fprintf(stdout,"\nUTC time: %s", asctime(utcTime));
    }
    // Show initial (command line) parameters
    if(p.showParameters)
    {
        showSettings(&p);
    }
    // Open I2C bus (only one at a time for now)    
    openI2CBus(&p);
    // Setup the magnetometer.
    setup_mag(&p);
    mag_set_sample_rate(&p, 100);
    // TMRC Reg mysteriously gets bollixed up, probably setting CMM rates.  Brute force fix for now.
    p.TMRCRate = p.TMRCRate;
    setTMRCReg(&p);
#if DEBUG
//    fprintf(stdout,"TMRC reg value: %i\n", getTMRCReg(&p));
#endif    
    if(p.readBackCCRegs)
    {
        readCycleCountRegs(&p);
    }
    // loop
    while(1)
    {
        //  Read temp sensor.
        if(!p.magnetometerOnly)
        {
            if(p.remoteTempOnly)
            {
                temp = readTemp(&p, p.remoteTempAddr);
#if DEBUG
                fprintf(stdout,"REMOTE :: readTemp(&p, %2x) returns: %i\n", p.remoteTempAddr, temp);
#endif    
                rcTemp = temp * 0.0625;
            }
            else if(p.localTempOnly)
            {
                temp = readTemp(&p, p.localTempAddr);
#if DEBUG
                fprintf(stdout,"LOCAL :: readTemp(&p, %2x) returns: %i\n", p.localTempAddr, temp);
#endif    
                lcTemp = temp * 0.0625;
            }
            else
            {
                temp = readTemp(&p, p.remoteTempAddr);
#if DEBUG
                fprintf(stdout,"REMOTE :: readTemp(&p, %2x) returns: %i\n", p.remoteTempAddr, temp);
#endif    
                rcTemp = temp * 0.0625;
                temp = readTemp(&p, p.localTempAddr);
#if DEBUG
                fprintf(stdout,"LOCAL :: readTemp(&p, %2x) returns: %i\n", p.localTempAddr, temp);
#endif    
                lcTemp = temp * 0.0625;
            }
        }
        // Read Magnetometer.
        if((!p.localTempOnly) || (!p.remoteTempOnly))
        {
            readMag(&p, p.magnetometerAddr, rXYZ);
            xyz[0] = (rXYZ[0] / p.x_gain);
            xyz[1] = (rXYZ[1] / p.y_gain);
            xyz[2] = (rXYZ[2] / p.z_gain);
        }
        // Output the results.
        if(!(p.jsonFlag))
        {
            if(p.tsMilliseconds)
            {
                fprintf(stdout, "Time Stamp: %ld, ", currentTimeMillis());
            }
            else
            {
                utcTime = getUTC();
                strftime(utcStr, UTCBUFLEN, "%d %b %Y %T", utcTime);                
                fprintf(stdout, "Time: %s", utcStr);
            }
            if(p.remoteTempOnly)
            {
                fprintf(stdout, ", rTemp: %.2f", rcTemp);
            }
            else if(p.localTempOnly)
            {
                fprintf(stdout, ", lTemp: %.2f", lcTemp);
            }
            else
            {
                fprintf(stdout, ", rTemp: %.2f", rcTemp);
                fprintf(stdout, ", lTemp: %.2f", lcTemp);
            }
            fprintf(stdout, ", x: %i", xyz[0]);
            fprintf(stdout, ", y: %i", xyz[1]);
            fprintf(stdout, ", z: %i", xyz[2]);
            if(!p.hideRaw)
            {
                fprintf(stdout, ", rx: %i", rXYZ[0]);
                fprintf(stdout, ", ry: %i", rXYZ[1]);
                fprintf(stdout, ", rz: %i", rXYZ[2]);
            }
            if(p.showTotal)
            {
                fprintf(stdout, ", Tm: %.0f", sqrt((xyz[0] * xyz[0]) + (xyz[1] * xyz[1]) + (xyz[2] * xyz[2])));
            }
            fprintf(stdout, "\n");
        }
        else
        {
            fprintf(stdout, "{ ");
            if(p.tsMilliseconds)
            {
                fprintf(stdout, "\"%ld\", ",  currentTimeMillis());
            }
            else
            {
                utcTime = getUTC();
                strftime(utcStr, UTCBUFLEN, "%d %b %Y %T", utcTime);        // RFC 2822: "%a, %d %b %Y %T %z"      RFC 822: "%a, %d %b %y %T %z"  
                fprintf(stdout, "ts:\"%s\", ", utcStr);
            }
            if(p.remoteTempOnly)
            {
                fprintf(stdout, " rt:\"%.2f\"",  rcTemp);
           }
            else if(p.localTempOnly)
            {
                fprintf(stdout, " lt:\"%.2f\"",  lcTemp);
            }
            else
            {
                fprintf(stdout, " rt:\"%.2f\"",  rcTemp);
                fprintf(stdout, " lt:\"%.2f\"",  lcTemp);
            }
            fprintf(stdout, ", x:\"%i\"", xyz[0]);
            fprintf(stdout, ", y:\"%i\"", xyz[1]);
            fprintf(stdout, ", z:\"%i\"", xyz[2]);
            if(!p.hideRaw)
            {
                fprintf(stdout, ", rx:\"%i\"", rXYZ[0]);
                fprintf(stdout, ", ry:\"%i\"", rXYZ[1]);
                fprintf(stdout, ", rz:\"%i\"", rXYZ[2]);
            }
            if(p.showTotal)
            {
                fprintf(stdout, ", Tm: \"%.0f\"",  sqrt((xyz[0] * xyz[0]) + (xyz[1] * xyz[1]) + (xyz[2] * xyz[2])));
            }
           fprintf(stdout, " }\n");
        }
        fflush(stdout);
        if(p.singleRead)
        {
            break;
        }
        // wait p.outDelay (1000 ms default) for next poll.
        usleep(p.outDelay);
    }
    closeI2CBus(p.i2c_fd);
    return 0;
}

