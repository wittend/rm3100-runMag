//=========================================================================
// cmdmgr.c
//
// commandline / configuration management routines for runMag utility.
//
// Author:      David Witten, KD0EAG
// Date:        June 19, 2020
// License:     GPL 3.0
//=========================================================================
#include "main.h"
//#include "jsmn/jsmn.h"
//#include "uthash/uthash.h"
#include "cmdmgr.h"

extern char version[];
//extern char outputPipeName[MAXPATHBUFLEN];
//extern char inputPipeName[MAXPATHBUFLEN];
extern char outFilePath[MAXPATHBUFLEN];
extern char workFilePath[MAXPATHBUFLEN];
extern char rollOverTime[UTCBUFLEN];
extern char sitePrefixString[SITEPREFIXLEN];

//-------------------------------------------
//  Informative list of known SBC defaults,
//-------------------------------------------
static struct busDev busDevs[] =
{
    /* Device Path,     SBCString,          enum Value          Bus Number*/
//    {OTHER_BUS,         sOTHER_BUS,          eOTHER_BUS,         -1},
    {KHADAS_EDGE_I2C3,  sKHADAS_EDGE_BUS3,   eKHADAS_EDGE_I2C3,  3},
    {KHADAS_VIM3_I2C3,  sVIM3_I2C_BUS3,      eVIM3_I2C_BUS3,     3},
    {KHADAS_VIM3_I2C4,  sVIM3_I2C_BUS4,      eVIM3_I2C_BUS4,     4},
    {NV_XAVIER_I2C_BUS, sNV_XAVIER_I2C_BUS,  eNV_XAVIER_I2C_BUS, 8},
    {NV_NANO_I2C_BUS,   sNV_NANO_I2C_BUS,    eNV_NANO_I2C_BUS,   1},
    {ODROIDC0_I2C_BUS,  sODROIDC0_I2C_BUS,   eODROIDC0_I2C_BUS,  1},
    {ODROIDC1_I2C_BUS,  sODROIDC1_I2C_BUS,   eODROIDC1_I2C_BUS,  1},
    {ODROIDN2_I2C_BUS,  sODROIDN2_I2C_BUS,   eODROIDC2_I2C_BUS,  1},
    {ODROIDC4_I2C_BUS,  sODROIDC4_I2C_BUS,   eODROIDC4_I2C_BUS,  2},
    {ODROIDC4_I2C_BUS3, sODROIDC4_I2C_BUS3,  eODROIDC4_I2C_BUS3, 3},
    {ODROIDN2_I2C_BUS,  sODROIDN2_I2C_BUS,   eODROIDN2_I2C_BUS,  2},
    {ODROIDN2_I2C_BUS3, sODROIDN2_I2C_BUS3,  eODROIDN2_I2C_BUS3, 3},
    {ODROIDN2PLUS_I2C_BUS0, sODROIDN2PLUS_I2C_BUS0,  eODROIDN2PLUS_I2C_BUS0,  0},
    {ODROIDN2PLUS_I2C_BUS1, sODROIDN2PLUS_I2C_BUS1,  eODROIDN2PLUS_I2C_BUS1, 1},
    {RASPI_I2C_BUS,     sRASPI_I2C_BUS,      eRASPI_I2C_BUS,     1},
    {0,                 0,                  -1,                 -1}
};

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
    if(now == -1)
    {
        puts("The time() function failed");
    }
    struct tm *ptm = gmtime(&now);
    if(ptm == NULL)
    {
        puts("The gmtime() function failed");
    }
    return ptm;
}

//------------------------------------------
// showCountGainRelationship()
//------------------------------------------
void showCountGainRelationship()
{
    // Probably a bit of silliness.
    fprintf(stdout, "    -----------------------------------------------------------------------\n");
    fprintf(stdout, "    |    Cycle Count/Gain/Sensitivity        |     RM3100 Measurement     |\n");
    fprintf(stdout, "    |---------------------------------------------------------------------|\n");
    fprintf(stdout, "    | Cycle |   Gain   |                     |                            |\n");
    fprintf(stdout, "    | Count | (LSB/uT) | Sensitivity(nT/LSB) | in count | microTesla (uT) |\n");
    fprintf(stdout, "    |---------------------------------------------------------------------|\n");
    fprintf(stdout, "    |   50  |    20    |      50.000         |   3000   |     150.000     |\n");
    fprintf(stdout, "    |  100  |    38    |      26.316         |   3000   |      78.947     |\n");
    fprintf(stdout, "    |  200  |    75    |      13.333         |   3000   |      40.000     |\n");
    fprintf(stdout, "    |  300  |   113    |       8.850         |   3000   |      26.549     |\n");
    fprintf(stdout, "    |  400  |   150    |       6.667         |   3000   |      20.000     |\n");
    fprintf(stdout, "    -----------------------------------------------------------------------\n");
    fprintf(stdout, "From: RM3100_FAQ_R02.pdf\n\n");
    exit(0);
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
    fprintf(stdout, "\n\n");
    exit(0);
}

//------------------------------------------
//  buildLogFilePath()
//------------------------------------------
int buildLogFilePath(pList *p)
{
    int rv = 0;
    char utcStr[UTCBUFLEN] = "";
    struct tm *utcTime = getUTC();

    p->outputFilePath = workFilePath;
    strcpy(p->outputFilePath, outFilePath);

    const char ch = '/';

    if(p->outputFilePath[strlen(p->outputFilePath) - 1] != ch)
    {
        printf("Adding '/' to: %s\n", p->outputFilePath);
        strcat(p->outputFilePath, "/");
    }
    strcat(p->outputFilePath, p->sitePrefix);
    strcat(p->outputFilePath, utcStr);
    strcat(p->outputFilePath, "-");
    utcTime = getUTC();
    strftime(utcStr, UTCBUFLEN, "%Y%m%d-runmag.log", utcTime);        // RFC 2822: "%a, %d %b %Y %T %z"      RFC 822: "%a, %d %b %y %T %z"
    //strftime(utcStr, UTCBUFLEN, "%Y%m%d-%H%M-runmag.log", utcTime);        // RFC 2822: "%a, %d %b %Y %T %z"      RFC 822: "%a, %d %b %y %T %z"
    strcat(p->outputFilePath, utcStr);
    //printf("p->outputFilePath: %s\n", p->outputFilePath);
    return rv;
}

////------------------------------------------
//// readConfigFromFile()
////------------------------------------------
//int readConfigFromFile(pList *p, char *cfgFile)
//{
//    int rv = 0;
//    struct stat fs;
//    FILE *fd = NULL;
//    char jsonstr[JSONBUFLEN] = "";
//
//    if(lstat(cfgFile, &fs) == -1)
//    {
//        perror("lstat");
//        exit(EXIT_FAILURE);
//    }
//    if((fd = fopen(cfgFile, "r")) != NULL)
//    {
//        if(fread(jsonstr, fs.st_size, 1, fd))
//        {
//            printf("\nConfig read from file: %s\n\n", cfgFile);
//#if _DEBUG
//            printf(jsonstr);
//#endif
//        }
//        else
//        {
//            perror("reading config file");
//            exit(EXIT_FAILURE);
//        }
//    }
//    jsmn_parser parser;
//    jsmntok_t t[JSONBUFTOKENCOUNT]; /* We expect no more than JSONBUFTOKENCOUNT JSON tokens */
//
//    jsmn_init(&parser);
//    rv = jsmn_parse(&parser, jsonstr, strlen(jsonstr), t, JSONBUFTOKENCOUNT);
//#if _DEBUG
//    printf("\n");
//    switch(rv)
//    {
//        case JSMN_ERROR_INVAL:
//            printf("njsmn_parse() returns JSMN_ERROR_INVAL.\nBad token, JSON string is corrupted.\n");
//            break;
//        case JSMN_ERROR_NOMEM:
//            printf("jsmn_parse() returns JSMN_ERROR_NOMEM.\nNot enough tokens, JSON string is too large\n");
//            break;
//        case JSMN_ERROR_PART:
//            printf("jsmn_parse() returns JSMN_ERROR_PART.\nJSON string is too short, expecting more JSON data\n");
//            break;
//        default:
//            printf("jsmn_parse() returns %i tokens.\n", rv);
//            for(int i =  0; i < rv; i++)
//            {
//                char js[65] = "";
//                int type = t[i].type;
//
//                printf("t[%i] type: ", i);
//                switch(type)
//                {
//                    case JSMN_UNDEFINED:        // = 0,
//                        printf("JSMN_UNDEFINED.\n");
//                        break;
//                    case JSMN_OBJECT:           // = 1,
//                        printf("JSMN_OBJECT.\n");
//                        break;
//                    case JSMN_ARRAY:            // = 2,
//                        printf("JSMN_ARRAY.\n");
//                        break;
//                    case JSMN_STRING:           // = 3,
//                        printf("JSMN_STRING.  Value: ");
//                        strncpy(js, jsonstr + t[i].start, t[i].end - t[i].start);
////                        js[t[i].end] = 0;
//                        printf("%s\n", js);
//                        break;
//                    case JSMN_PRIMITIVE:        // = 4
//                        printf("JSMN_PRIMITIVE. Value: ");
//                        strncpy(js, jsonstr + t[i].start, t[i].end - t[i].start);
////                        js[t[i].end] = 0;
//                        printf("%s\n", js);
//                        break;
//                }
//            }
//            printf("\n");
//            break;
//    }
//    printf("\n");
//#endif
//    return rv;
//}

////------------------------------------------
//// saveConfigToFile()
////------------------------------------------
//int saveConfigToFile(pList *p, char *cfgFile)
//{
//    int rv = 0;
//    FILE *fd = NULL;
//    char js[256] = "";
//    char jsonstr[JSONBUFLEN] = "";
//
//    sprintf(js, "[\n  {");
//    strcat(jsonstr, js);
//
//    //char *Version;
//    sprintf(js, "\n    \"swVersion\": \"%s\",", version);
//    strcat(jsonstr, js);
//
//    sprintf(js, "\n    \"SBCType\": %i,", p->SBCType);
//    strcat(jsonstr, js);
//    sprintf(js, "\n    \"boardType\": %i,", p->boardType);
//    strcat(jsonstr, js);
//    sprintf(js, "\n    \"boardMode\": %i,", p->boardMode);
//    strcat(jsonstr, js);
//    sprintf(js, "\n    \"doBistMask\": %i,", p->doBistMask);
//    strcat(jsonstr, js);
//
////    int cc_x;
//    sprintf(js, "\n    \"cc_x\": %i,", p->cc_x);
//    strcat(jsonstr, js);
////    int cc_y;
//    sprintf(js, "\n    \"cc_y\": %i,", p->cc_y);
//    strcat(jsonstr, js);
////    int cc_z;
//    sprintf(js, "\n    \"cc_z\": %i,", p->cc_z);
//    strcat(jsonstr, js);
//
////    int x_gain;
//    sprintf(js, "\n    \"x_gain\": %i,", p->x_gain);
//    strcat(jsonstr, js);
////    int y_gain;
//    sprintf(js, "\n    \"y_gain\": %i,", p->y_gain);
//    strcat(jsonstr, js);
////    int z_gain;
//    sprintf(js, "\n    \"z_gain\": %i,", p->z_gain);
//    strcat(jsonstr, js);
//
////    int TMRCRate;
//    sprintf(js, "\n    \"TMRCRate\": %i,", p->TMRCRate);
//    strcat(jsonstr, js);
////    int mSampleRate;
//    sprintf(js, "\n    \"mSampleRate\": %i,", p->mSampleRate);
//    strcat(jsonstr, js);
//
////    int samplingMode;
//    sprintf(js, "\n    \"samplingMode\": %i,", p->samplingMode);
//    strcat(jsonstr, js);
//
//    //int NOSRegValue;
//    sprintf(js, "\n    \"NOSRegValue\": %i,", p->NOSRegValue);
//    strcat(jsonstr, js);
//
//    //int readBackCCRegs;
//    sprintf(js, "\n    \"readBackCCRegs\": %i,", p->readBackCCRegs);
//    strcat(jsonstr, js);
//
//    ////int magRevId;
//    //sprintf(js, "\n  magRevId: %i,", p->magRevId);
//    //strcat(jsonstr, js);
//
//    //int hideRaw;
//    sprintf(js, "\n    \"hideRaw\": %i,", p->hideRaw);
//    strcat(jsonstr, js);
//    //int i2cBusNumber;
//    sprintf(js, "\n    \"i2cBusNumber\": %i,", p->i2cBusNumber);
//    strcat(jsonstr, js);
//
//    // int i2c_fd;
//    //sprintf(js, "\n  samplingMode: %i,", p->samplingMode);
//    //strcat(jsonstr, js);
//
//    // int jsonFlag;
//    sprintf(js, "\n    \"jsonFlag\": %i,", p->jsonFlag);
//    strcat(jsonstr, js);
//
//    // int localTempOnly;
//    sprintf(js, "\n    \"localTempOnly\": %i,", p->localTempOnly);
//    strcat(jsonstr, js);
//    //int localTempAddr;
//    sprintf(js, "\n    \"localTempAddr\": %i,", p->localTempAddr);
//    strcat(jsonstr, js);
//
//    //int magnetometerOnly;
//    sprintf(js, "\n    \"magnetometerOnly\": %i,", p->magnetometerOnly);
//    strcat(jsonstr, js);
//    //int magnetometerAddr;
//    sprintf(js, "\n    \"magnetometerAddr\": %i,", p->magnetometerAddr);
//    strcat(jsonstr, js);
//
//    //int remoteTempOnly;
//    sprintf(js, "\n    \"remoteTempOnly\": %i,", p->remoteTempOnly);
//    strcat(jsonstr, js);
//    //int remoteTempAddr;
//    sprintf(js, "\n    \"remoteTempAddr\": %i,", p->remoteTempAddr);
//    strcat(jsonstr, js);
//
//    //int outDelay;
//    sprintf(js, "\n    \"outDelay\": %i,", p->outDelay);
//    strcat(jsonstr, js);
//    //int quietFlag;
//    sprintf(js, "\n    \"quietFlag\": %i,", p->quietFlag);
//    strcat(jsonstr, js);
//    //int showParameters;
//    sprintf(js, "\n    \"showParameters\": %i,", p->showParameters);
//    strcat(jsonstr, js);
//    //int singleRead;
//    sprintf(js, "\n    \"singleRead\": %i,", p->singleRead);
//    strcat(jsonstr, js);
//    //int tsMilliseconds;
//    sprintf(js, "\n    \"tsMilliseconds\": %i,", p->tsMilliseconds);
//    strcat(jsonstr, js);
//    //int verboseFlag;
//    sprintf(js, "\n    \"verboseFlag\": %i,", p->verboseFlag);
//    strcat(jsonstr, js);
//    //int showTotal;
//    sprintf(js, "\n    \"showTotal\": %i", p->showTotal);
//    strcat(jsonstr, js);
//
//    sprintf(js, "\n  }\n]\n");
//    strcat(jsonstr, js);
//
//#if _DEBUG
//    printf("%s", jsonstr);
//#endif
//
//    if((fd = fopen(cfgFile, "w")) != NULL)
//    {
//        if(fwrite(jsonstr, strlen(jsonstr), 1, fd))
//        {
//            printf("\nSaved config to file: %s\n\n", cfgFile);
//        }
//        else
//        {
//            perror("writing config file");
//        }
//        fclose(fd);
//    }


//------------------------------------------
// setOutputFilePath()
//------------------------------------------
int setLogRollOver(pList *p, char *rollTime)
{
    int rv = 0;

    strncpy(rollOverTime, rollTime, strlen(rollTime));
    p->logOutputTime = rollOverTime;
    return rv;
}

//------------------------------------------
// setOutputFilePath()
//------------------------------------------
int setOutputFilePath(pList *p, char *outPath)
{
    int rv = 0;

    if(strlen(outPath) > MAXPATHBUFLEN - 1)
    {
        fprintf(stderr, "\nOutput path length exceeds maximum allowed length (%i)\n", MAXPATHBUFLEN - 1);
        rv =  1;
    }
    else
    {
        strncpy(outFilePath, outPath, strlen(outPath));
        p->outputFilePath = outFilePath;
    }
    return rv;
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
#if(0)    
    if(!p->magRevId)
    {
        getMagRev(p);
    }
#endif    
    fprintf(stdout, "   Magnetometer revision ID detected:          %i (dec)\n",    p->magRevId);
    fprintf(stdout, "   Log output path:                            %s\n",          p->buildLogPath ? "TRUE" : "FALSE");
    fprintf(stdout, "   Log output:                                 %s\n",          p->logOutput ? "TRUE" : "FALSE");
    //fprintf(stdout, "   Log Rollover time:                          %s\n",          p->logOutputTime);
    fprintf(stdout, "   Log site prefix string:                     %s\n",          p->sitePrefix);
    fprintf(stdout, "   Output file path:                           %s\n",          p->outputFilePath);
#if (USE_PIPES)
    fprintf(stdout, "   Log output to pipes:                        %s\n",          p->useOutputPipe ? "TRUE" : "FALSE");
    fprintf(stdout, "   Input file path:                            %s\n",          p->pipeInPath);
    fprintf(stdout, "   Output file path:                           %s\n",          p->pipeOutPath);
#endif
    fprintf(stdout, "   I2C bus number as integer:                  %i (dec)\n",    p->i2cBusNumber);
    fprintf(stdout, "   I2C bus path as string:                     %s\n",          pathStr);
    fprintf(stdout, "   Built in self test (BIST) value:            %02X (hex)\n",  p->doBistMask);
    fprintf(stdout, "   NOS Register value:                         %02X (hex)\n",  p->NOSRegValue);
    fprintf(stdout, "   Post DRDY delay:                            %i (dec)\n",    p->DRDYdelay);
    fprintf(stdout, "   Device sampling mode:                       %s\n",          p->samplingMode     ? "CONTINUOUS" : "POLL");
    fprintf(stdout, "   Cycle counts by vector:                     X: %3i (dec), Y: %3i (dec), Z: %3i (dec)\n", p->cc_x, p->cc_y, p->cc_z);
    fprintf(stdout, "   Gain by vector:                             X: %3i (dec), Y: %3i (dec), Z: %3i (dec)\n", p->x_gain, p->y_gain, p->z_gain);
    fprintf(stdout, "   Read back CC Regs after set:                %s\n",          p->readBackCCRegs   ? "TRUE" : "FALSE");
    fprintf(stdout, "   Software Loop Delay (uSec):                 %i (dec uSec)\n",    p->outDelay);
    fprintf(stdout, "   CMM sample rate:                            %2X (hex)\n",   p->CMMSampleRate);
    fprintf(stdout, "   TMRC reg value:                             %2X (hex)\n",   p->TMRCRate);
    fprintf(stdout, "   Format output as JSON:                      %s\n",          p->jsonFlag         ? "TRUE" : "FALSE");
    fprintf(stdout, "   Read local temperature only:                %s\n",          p->localTempOnly    ? "TRUE" : "FALSE");
    fprintf(stdout, "   Read remote temperature only:               %s\n",          p->remoteTempOnly   ? "TRUE" : "FALSE");
    fprintf(stdout, "   Read magnetometer only:                     %s\n",          p->magnetometerOnly ? "TRUE" : "FALSE");
    fprintf(stdout, "   Local temperature address:                  %02X (hex)\n",  p->localTempAddr);
    fprintf(stdout, "   Remote temperature address:                 %02X (hex)\n",  p->remoteTempAddr);
    fprintf(stdout, "   Magnetometer address:                       %02X {hex)\n",  p->magnetometerAddr);
    fprintf(stdout, "   Show parameters:                            %s\n",          p->showParameters   ? "TRUE" : "FALSE");
    fprintf(stdout, "   Quiet mode:                                 %s\n",          p->quietFlag        ? "TRUE" : "FALSE");
    fprintf(stdout, "   Hide raw measurements:                      %s\n",          p->hideRaw          ? "TRUE" : "FALSE");
    fprintf(stdout, "   Return single magnetometer reading:         %s\n",          p->singleRead       ? "TRUE" : "FALSE");
    fprintf(stdout, "   Magnetometer configuation:                  %s\n", (p->boardMode == LOCAL) ? "Local standalone" : "Extended with remote");
    fprintf(stdout, "   Timestamp format:                           %s\n",          p->tsMilliseconds   ? "RAW"  : "UTCSTRING");
    fprintf(stdout, "   Verbose output:                             %s\n",          p->verboseFlag      ? "TRUE" : "FALSE");
    fprintf(stdout, "   Show total field:                           %s\n",          p->showTotal        ? "TRUE" : "FALSE");
    fprintf(stdout, "\n\n");
}

//------------------------------------------
// getCommandLine()
//------------------------------------------
int getCommandLine(int argc, char** argv, pList *p)
{
    int c;
    int NOSval = 0;
    int magAddr = 0;
    int lTmpAddr = 0;
    int rTmpAddr = 0;

    if(p != NULL)
    {
        memset(p, 0, sizeof(pList));
    }

    p->SBCType          = eRASPI_I2C_BUS;
    p->boardType        = 0;
    p->boardMode        = LOCAL;
    p->doBistMask       = FALSE;
    p->NOSRegValue      = 10;
    p->DRDYdelay        = 0;
    p->buildLogPath     = FALSE;

    p->cc_x             = CC_400;
    p->cc_y             = CC_400;
    p->cc_z             = CC_400;
    p->x_gain           = GAIN_150;
    p->y_gain           = GAIN_150;
    p->z_gain           = GAIN_150;

    p->samplingMode     = POLL;
    p->readBackCCRegs   = FALSE;
    p->CMMSampleRate    = 200;
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
    p->TMRCRate         = 0x96;
    p->verboseFlag      = FALSE;
    p->showTotal        = FALSE;
    p->outputFilePath   = outFilePath;
#if(USE_PIPES)
    p->useOutputPipe    = FALSE;
    p->pipeInPath       = inputPipeName;
    p->pipeOutPath      = outputPipeName;
#endif
    //p->logOutputTime    = rollOverTime;
    p->logOutput        = FALSE;
    p->Version          = version;

//#if (USE_PIPES)
//    while((c = getopt(argc, argv, "?aA:b:B:c:Cd:D:Ef:F:g:HhjklL:mM:O:PqrR:sS:Tt:U:YvVZ")) != -1)
//#else
//    while((c = getopt(argc, argv, "?aA:b:B:c:Cd:D:Ef:F:g:HhjklL:mM:O:PqrR:sS:Tt:U:vVZ")) != -1)
//#endif
#if (USE_PIPES)
    while((c = getopt(argc, argv, "?aA:b:B:c:CD:Ef:F:g:HhjklL:mM:O:PqrR:sS:Tt:YvVZ")) != -1)
#else
    while((c = getopt(argc, argv, "?aA:b:B:c:CD:Ef:F:g:HhjklL:mM:O:PqrR:sS:Tt:vVZ")) != -1)
#endif
    {
        //int this_option_optind = optind ? optind : 1;
        switch(c)
        {
            case 'a':
                listSBCs();
                break;
            case 'A':
                // fprintf(stdout, "\nThe -A option is intended to allow setting a value in the NOS register.\n\n");
                sscanf(optarg, "%u", &NOSval);
                if(NOSval <= 1)
                {
                    if(p->verboseFlag)
                    {
                        fprintf(stderr, "Error: %u :: NOS input value must be >= 1. Forcing -A input to 10 \n", NOSval);
                    }
                    (p->NOSRegValue = 10);
                }
                else
                {
                    (p->NOSRegValue = NOSval);
                }
                if(p->verboseFlag)
                {
                    fprintf(stderr, "p->NOSRegValue:: %u\n", p->NOSRegValue);
                }
                // setNOSReg(p);
                break;
            case 'b':
                p->i2cBusNumber = atoi(optarg);
                break;
            case 'B':
                p->doBistMask = atoi(optarg);
                // printf("Not implemented yet.");
                break;
            case 'c':
                p->cc_x = p->cc_y = p->cc_z = atoi(optarg);
                if((p->cc_x > CC_800) || (p->cc_x <= 0))
                {
                    fprintf(stderr, "\n ERROR Invalid: cycle count > 400 (dec) or cycle count  <= 0.\n\n");
                    exit(1);
                }
                p->x_gain = p->y_gain = p->z_gain = getCCGainEquiv(p->cc_x);
                break;
            case 'C':
                p->readBackCCRegs = TRUE;
                break;
//            case 'd':
//                p->outDelay = atoi(optarg) * 1000;
//                break;
            case 'D':
                p->CMMSampleRate = atoi(optarg);
                break;
            case 'E':
                showCountGainRelationship();
                break;
            case 'f':
//                readConfigFromFile(p, optarg);
                break;
            case 'F':
//                saveConfigToFile(p, optarg);
                break;
            case 'g':
                p->samplingMode = atoi(optarg);
                break;
            case 'H':
                p->hideRaw = TRUE;
                break;
            case 'j':
                p->jsonFlag = TRUE;
                break;
            case 'k':
                p->logOutput = TRUE;
                p->buildLogPath = TRUE;
                break;
            //case 'K':
            //    setLogRollOver(p, optarg);
            //    p->buildLogPath = TRUE;
            //    break;
            case 'l':
                p->localTempOnly = TRUE;
                break;
            case 'L':
                sscanf(optarg, "%x", &lTmpAddr);
                p->localTempAddr = lTmpAddr;
                break;
            case 'm':
                p->magnetometerOnly = TRUE;
                break;
            case 'M':
                //p->magnetometerAddr = atoi(optarg);
                sscanf(optarg, "%x", &magAddr);
                p->magnetometerAddr = magAddr;
                break;
            case 'O':
                if(strlen(optarg) < MAXPATHBUFLEN)
                {
                    setOutputFilePath(p, optarg);
                    p->buildLogPath = TRUE;
                }
                else
                {
                    fprintf(stderr, "\nSite Prefix must be less than %i characters.\n", (MAXPATHBUFLEN - SITEPREFIXLEN) - UTCBUFLEN);
                    exit(1);
                }
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
                sscanf(optarg, "%x", &rTmpAddr);
                p->remoteTempAddr = rTmpAddr;
                break;
            case 's':
                p->singleRead = TRUE;
                break;
            case 'S':
                if(strlen(optarg) < SITEPREFIXLEN)
                {
                    strncpy(sitePrefixString, optarg, SITEPREFIXLEN);
                    p->sitePrefix = sitePrefixString;
                    p->buildLogPath = TRUE;
                }
                else
                {
                    fprintf(stderr, "\nSite Prefix must be less than %i\n", SITEPREFIXLEN);
                    exit(1);
                }
                break;
            case 'T':
                p->tsMilliseconds = TRUE;
                break;
            case 't':
                p->TMRCRate = atoi(optarg);
                break;
//            case 'U':
//                p->DRDYdelay = atoi(optarg) * 1000;
//                break;
            case 'V':
                fprintf(stdout, "\nVersion: %s\n", p->Version);
                exit(0);
                break;
            case 'v':
                p->verboseFlag = TRUE;
                p->quietFlag = FALSE;
                break;
#if (USE_PIPES)
            case 'Y':
                p->useOutputPipe = TRUE;
                break;
#endif
            case 'Z':
                p->showTotal = TRUE;
                break;
            case 'h':
            case '?':
                fprintf(stdout, "\n%s Version = %s\n", argv[0], version);
                fprintf(stdout, "\nParameters:\n\n");
                fprintf(stdout, "   -a                     :  List known SBC I2C bus numbers.       [ use with -b ]\n");
                fprintf(stdout, "   -A                     :  Set NOS (0x0A) register value.        [ Don't use unless you know what you are doing ]\n");
                fprintf(stdout, "   -B <reg mask>          :  Do built in self test (BIST).         [ Not implemented ]\n");
                fprintf(stdout, "   -b <bus as integer>    :  I2C bus number as integer.\n");
                fprintf(stdout, "   -C                     :  Read back cycle count registers before sampling.\n");
                fprintf(stdout, "   -c <count>             :  Set cycle counts as integer.          [ default 200 decimal]\n");
                fprintf(stdout, "   -D <rate>              :  Set magnetometer sample rate.         [ TMRC reg 96 hex default ].\n");
//                fprintf(stdout, "   -d <delay as ms>       :  Output delay.                         [ 1000 ms default ]\n");
                fprintf(stdout, "   -E                     :  Show cycle count/gain/sensitivity relationship.\n");
                fprintf(stdout, "   -f <filename>          :  Read configuration from file (JSON).  [ Not implemented ]\n");
                fprintf(stdout, "   -F <filename>          :  Write configuration to file (JSON).   [ Not implemented ]\n");
                fprintf(stdout, "   -g <mode>              :  Device sampling mode.                 [ POLL=0 (default), CONTINUOUS=1 ]\n");
                fprintf(stdout, "   -H                     :  Hide raw measurments.\n");
                fprintf(stdout, "   -j                     :  Format output as JSON.\n");
                fprintf(stdout, "   -k                     :  Create and roll log files.            [ 00:00 UTC default ]\n");
                //fprintf(stdout, "   -K <time string>       :  Rotate log time.                      [ if non-default - UTC ]\n");
                fprintf(stdout, "   -L <addr as integer>   :  Local temperature address.            [ default 19 hex ]\n");
                fprintf(stdout, "   -l                     :  Read local temperature only.\n");
                fprintf(stdout, "   -M <addr as integer>   :  Magnetometer address.                 [ default 20 hex ]\n");
                fprintf(stdout, "   -m                     :  Read magnetometer only.\n");
                fprintf(stdout, "   -O <filename>          :  Output file path.                     [ Must be valid path with write permissions ]\n");
                fprintf(stdout, "   -P                     :  Show Parameters.\n");
                fprintf(stdout, "   -q                     :  Quiet mode.                           [ partial ]\n");
                fprintf(stdout, "   -v                     :  Verbose output.\n");
                fprintf(stdout, "   -R <addr as integer>   :  Remote temperature address.           [ default 18 hex ]\n");
                fprintf(stdout, "   -r                     :  Read remote temperature only.\n");
                fprintf(stdout, "   -s                     :  Return single reading.                [ Do one measurement loop only ]\n");
                fprintf(stdout, "   -S                     :  Site prefix string for log files.     [ 32 char max. Do not use /\'\"* etc. Try callsign! ]\n");
                //fprintf(stdout, "   -t                     :  Set CMM Data Rate.                    [ 96 hex default ]\n");
                fprintf(stdout, "   -T                     :  Raw timestamp in milliseconds.        [ default: UTC string ]\n");
//                fprintf(stdout, "   -U <delay as ms>       :  Delay in mSec before DRDY.            [ default: 0 ]\n");
                fprintf(stdout, "   -V                     :  Display software version and exit.\n");
#if(USE_PIPES)
                fprintf(stdout, "   -Y                     :  Use WebSockets.                       [ default False].\n");
#endif
                fprintf(stdout, "   -Z                     :  Show total field.                     [ sqrt((x*x) + (y*y) + (z*z)) ]\n");
                fprintf(stdout, "   -h or -?               :  Display this help.\n\n");
                return 1;
                break;
            default:
                fprintf(stdout, "\n?? getopt returned character code 0x%2X ??\n", c);
                break;
        }
    }
    if(optind < argc)
    {
        printf("non-option ARGV-elements: ");
        while(optind < argc)
        {
            printf("%s ", argv[optind++]);
        }
        printf("\n");
    }
    return 0;
}
