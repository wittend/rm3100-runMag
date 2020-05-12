//=========================================================================
// device_defs.h
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
#ifndef SWX3100DEFS_h
#define SWX3100DEFS_h

#define SIMPLEI2C_VERSION "0.0.3"

#define MAX_I2C_WRITE               32

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define LOCAL 0
#define REMOTE 1

//-------------------------------------------
// Enumerations 
//-------------------------------------------
typedef enum
{
    SensorPowerModePowerDown = 0,       
    SensorPowerModeSuspend = 1,       
    SensorPowerModeActive = 255,  
} SensorPowerMode;
 
//-------------------------------------------
// SensorStatus
//-------------------------------------------
typedef enum
{
    /* Valid Responses */
    SensorOK,                       /**< @brief Sensor responded with expected data. */
    SensorInitialized,              /**< @brief Sensor has been initialized. */
 
    /* Error Responses */
    SensorUnknownError,             /**< @brief An unknown error has occurred. */
    SensorErrorNonExistant,         /**< @brief Unable to communicate with sensor, sensor did not ACK. */
    SensorErrorUnexpectedDevice,    /**< @brief A different sensor was detected at the address. */
 
    SensorStatusPending = 255,      /**< @brief Reserved for internal used */
} SensorStatus;
 
#define CMMMODE_START 1
#define CMMMODE_DRDM  4     // Dont Use
#define CMMMODE_CMX   16
#define CMMMODE_CMY   32
#define CMMMODE_CMZ   64

#define CMMMODE_ALL   (CMMMODE_START | CMMMODE_CMX | CMMMODE_CMY | CMMMODE_CMZ)

//-------------------------------------------
// Device paths for different platforms
//-------------------------------------------
#define OTHER_BUS           ""
#define RASPI_I2C_BUS       "/dev/i2c-1"
#define ODROIDC0_I2C_BUS    "/dev/i2c-1"
#define ODROIDC1_I2C_BUS    "/dev/i2c-1"
#define ODROIDC2_I2C_BUS    "/dev/i2c-1"
#define ODROIDC4_I2C_BUS    "/dev/i2c-2"
#define ODROIDC4_I2C_BUS3   "/dev/i2c-3"
#define ODROIDN2_I2C_BUS    "/dev/i2c-2"
#define ODROIDN2_I2C_BUS3   "/dev/i2c-3"
#define NV_XAVIER_I2C_BUS   "/dev/i2c-8"
#define NV_NANO_I2C_BUS     "/dev/i2c-1"

//-------------------------------------------
// Known SBC device platforms 
//-------------------------------------------
#define sOTHER_BUS           "Other           "
#define sRASPI_I2C_BUS       "Rasoberry Pi 3/4"
#define sODROIDC0_I2C_BUS    "Odroid CO bus 1 "
#define sODROIDC1_I2C_BUS    "Odroid C1 bus 1 "
#define sODROIDC2_I2C_BUS    "Odroid C2 bus 1 "
#define sODROIDC4_I2C_BUS    "Odroid C4 bus 1 "
#define sODROIDC4_I2C_BUS3   "Odroid C4 bus 3 "
#define sODROIDN2_I2C_BUS    "Odroid N2 bus 2 "
#define sODROIDN2_I2C_BUS3   "Odroid N2 bus 3 "
#define sNV_XAVIER_I2C_BUS   "NV Xavier bus 8 "
#define sNV_NANO_I2C_BUS     "NV Nano bus 1   "

//-------------------------------------------
// Device paths for different platforms
//-------------------------------------------
typedef enum
{
//    eOTHER_BUS = -1,
    eRASPI_I2C_BUS = 0,
    eODROIDC0_I2C_BUS,
    eODROIDC1_I2C_BUS,
    eODROIDC2_I2C_BUS,
    eODROIDC4_I2C_BUS,
    eODROIDC4_I2C_BUS3,
    eODROIDN2_I2C_BUS,
    eODROIDN2_I2C_BUS3,
    eNV_XAVIER_I2C_BUS,
    eNV_NANO_I2C_BUS,
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

//-------------------------------------------
// 
//-------------------------------------------
static struct busDev busDevs[] =
{
    /* Device Path,     SBCString,          enum Value          Bus Number*/
//    {OTHER_BUS,         sOTHER_BUS,          eOTHER_BUS,         -1},
    {RASPI_I2C_BUS,     sRASPI_I2C_BUS,      eRASPI_I2C_BUS,     1},
    {ODROIDC0_I2C_BUS,  sODROIDC0_I2C_BUS,   eODROIDC0_I2C_BUS,  1},
    {ODROIDC1_I2C_BUS,  sODROIDC1_I2C_BUS,   eODROIDC1_I2C_BUS,  1},
    {ODROIDN2_I2C_BUS,  sODROIDN2_I2C_BUS,   eODROIDC2_I2C_BUS,  1},
    {ODROIDC4_I2C_BUS,  sODROIDC4_I2C_BUS,   eODROIDC4_I2C_BUS,  2},
    {ODROIDC4_I2C_BUS3, sODROIDC4_I2C_BUS3,  eODROIDC4_I2C_BUS3, 3},
    {ODROIDN2_I2C_BUS,  sODROIDN2_I2C_BUS,   eODROIDN2_I2C_BUS,  2},
    {ODROIDN2_I2C_BUS3, sODROIDN2_I2C_BUS3,  eODROIDN2_I2C_BUS3, 3},
    {NV_XAVIER_I2C_BUS, sNV_XAVIER_I2C_BUS,  eNV_XAVIER_I2C_BUS, 8},
    {NV_NANO_I2C_BUS,   sNV_NANO_I2C_BUS,    eNV_NANO_I2C_BUS,   1},
    {0,                 0,                  -1,                 -1}
};


//-------------------------------------------
// I2C bus speed options available
//-------------------------------------------
#define I2C_LOWSPEED            10000
#define I2C_STANDARD            100000
#define I2C_FASTMODE            1000000
#define I2C_HIGHSPEED           3400000

//-------------------------------------------
// Cycle Count values (16 bit)
//-------------------------------------------
#define CC_50       0x32        // 50  decimal
#define CC_100      0x64        // 100 decimal
#define CC_200      0xC8        // 200 decimal (default)
#define CC_300      0x12C       // 300 decimal
#define CC_400      0x190       // 400 decimal

#define CCP0        0xC8        // 200 Cycle Count
#define CCP1        0x00
#define NOS         0x01        // Number of Samples for averaging
#define TMRC        0x04        // Default rate 125 Hz 

//-------------------------------------------
// Gain values (decimal)
//-------------------------------------------
#define GAIN_20     20
#define GAIN_38     38
#define GAIN_75     75
#define GAIN_113    113
#define GAIN_150    150

//-------------------------------------------
// Address of the device
//-------------------------------------------
#define RM3100_I2C_ADDRESS              0x20
#define RM3100_I2C_ADDRESS_7bit         0x20
#define RM3100_I2C_ADDRESS_8bit         0x20 << 1 // MBED uses 8 bit address

//-------------------------------------------
// Value of the MagI2C Version expected
//-------------------------------------------
#define RM3100_VER_EXPECTED     0x22

//-------------------------------------------
//  From:   Table 5-1: MagI2C Register Map
//          "RM3100 & RM2100 Sensor Suite User Manual"
//          PNI Sensor Corporation
//          Doc 1017252 R07
//          Page 28 of 43
//-------------------------------------------
//  Name  |  Register #(Hex) | R/W      | Default (Hex) | Payload Format | Description
//-------------------------------------------------------------------------------
// POLL   |         00       |  RW      | 00            |     [7:0]      | Polls for a Single Measurement (aka MAG)
#define RM3100_MAG_POLL     0x00

// CMM    |         01       |  RW      | 00            |     [7:0]      | Initiates Continuous Measurement Mode (aka 'Beacon')
#define RM3100I2C_CMM       0x01

// CCX    |      04 – 05     |  RW      | 00C8          |    UInt16      | Cycle Count Register – X Axis
// Set values with  i2cset:  "sudo i2cset -y 1 0x20 0x04 0x01 0x90 0x01 0x90 0x01 0x90 0x0A i"
// Read values with i2cdump: "sudo i2cdump -r 0x04-0x0a 2 0x20"
#define RM3100I2C_CCX_1     0x04
#define RM3100I2C_CCX_0     0x05

// CCY    |      06 – 07     |  RW      | 00C8          |    UInt16      | Cycle Count Register – Y Axis
#define RM3100I2C_CCY_1     0x06
#define RM3100I2C_CCY_0     0x07

// CCZ    |      08 – 09     |  RW      | 00C8          |    Uint16      | Cycle Count Register – Z Axis
#define RM3100I2C_CCZ_1     0x08
#define RM3100I2C_CCZ_0     0x09

// NOS ?
#define RM3100I2C_NOS       0x0A

// TMRC   |      0B          |  RW      | 96            |     [7:0]      | Sets Continuous Measurement Mode Data Rate
#define RM3100I2C_TMRC      0x0B

// MX     |      24 – 26     |  R       | 000000        |    Uint24      | Measurement Results – X Axis
#define RM3100I2C_XYZ       0x24

#define RM3100I2C_MX        0x24
#define RM3100I2C_MX_2      0x24
#define RM3100I2C_MX_1      0x25
#define RM3100I2C_MX_0      0x26

// MY     |      27 – 29     |  R       | 000000        |    Uint24      | Measurement Results – Y Axis
#define RM3100I2C_MY        0x27
#define RM3100I2C_MY_2      0x27
#define RM3100I2C_MY_1      0x28
#define RM3100I2C_MY_0      0x29

// MZ     |      2A – 2C     |  R       | 000000        |    Uint24      | Measurement Results – Z Axis
#define RM3100I2C_MZ        0x2A
#define RM3100I2C_MZ_2      0x2A
#define RM3100I2C_MZ_1      0x2B
#define RM3100I2C_MZ_0      0x2C

// BIST   |      33          |  RW      | 00            |    [7:0]       | Built-In Self Test
#define RM3100I2C_BIST_WR   0x33

// STATUS |      34          |  R       | 00            |    [7:0]       | Status of DRDY
#define RM3100I2C_STATUS    0x34

// HSHAKE |      35          |  RW      | 1B            |    [7:0]       | Handshake Register
#define RM3100I2C_HSHAKE    0x35

// REVID  |      36          |  R       |       --      |    Unit8       | MagI2C Revision Identification
#define RM3100I2C_REVID     0x36

#define RM3100I2C_READMASK  0x80

#define RM3100I2C_POLLX     0x10
#define RM3100I2C_POLLY     0x20
#define RM3100I2C_POLLZ     0x40  
#define RM3100I2C_POLLXYZ   0x70        // POLLX & POLLY && POLLZ

#define CALIBRATION_TIMEOUT 5000        // timeout in milliseconds
#define DEG_PER_RAD (180.0/3.14159265358979)


#endif  //SWX3100DEFS_h
