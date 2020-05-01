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

/************************/
/*     Enumerations     */
/************************/
/**
* @enum SensorPowerMode
*
* @brief Possible sensor power modes supported.
*
*/
typedef enum
{
    SensorPowerModePowerDown = 0,       
    SensorPowerModeSuspend = 1,       
    SensorPowerModeActive = 255,  
} SensorPowerMode;
 
/**
* @enum SensorStatus
*
* @brief Sensor status results.
*/
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
 
//------------------------------------------
// Parameter List
//------------------------------------------
typedef struct tag_pList
{
    int boardType;
    int doBist; 

    int cc_x;
    int cc_y;
    int cc_z;
 
    int x_gain;
    int y_gain;
    int z_gain;

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
    int verboseFlag;
    

    char *Version;
} pList;

// Device paths for different platforms
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

typedef enum
{
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
#define RM3100_MAG_SINGLE   0x00

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

// MCP9808 Bus Address.
//-----------------------------------
#define MCP9808_I2CADDR_DEFAULT         0x18       // alt: (selected by chip hardware 0x19 - 0x1F

// MCP9808 Registers
//-----------------------------------
#define MCP9808_REG_CONFIG              0x01
#define MCP9808_REG_UPPER_TEMP          0x02
#define MCP9808_REG_LOWER_TEMP          0x03
#define MCP9808_REG_CRIT_TEMP           0x04
#define MCP9808_REG_AMBIENT_TEMP        0x05
#define MCP9808_REG_MANUF_ID            0x06
#define MCP9808_REG_DEVICE_ID           0x07
#define MCP9808_REG_RESOLUTION          0x08

// Configuration register values.
//-----------------------------------
#define MCP9808_REG_CONFIG_SHUTDOWN     0x0100
#define MCP9808_REG_CONFIG_CRITLOCKED   0x0080
#define MCP9808_REG_CONFIG_WINLOCKED    0x0040
#define MCP9808_REG_CONFIG_INTCLR       0x0020
#define MCP9808_REG_CONFIG_ALERTSTAT    0x0010
#define MCP9808_REG_CONFIG_ALERTCTRL    0x0008
#define MCP9808_REG_CONFIG_ALERTSEL     0x0004
#define MCP9808_REG_CONFIG_ALERTPOL     0x0002
#define MCP9808_REG_CONFIG_ALERTMODE    0x0001

// Expected return values.
//-----------------------------------
#define MCP9808_MANID_EXPECTED          0x0054
#define MCP9808_DEVREV_EXPECTED         0x0400


#endif  //SWX3100DEFS_h
