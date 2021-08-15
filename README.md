# rm3100-runMag

This is a program intended to assist in testing the PNI RM3100 geomagnetic sensor.  It is written in simple C. It is intended for use on boards such as the Raspberry Pi 4, Odroid, Nvidia Nano and their kin. It uses Linux
OS resources to open, read, write, and close the device through the appropriate I2C bus.  It does not support SPI.  It assumes that the I2C kernel drivers are installed, and that device names such as /dev/i2c-1,
/dev/i2c-2, can be listed using ls.  (The exact numbers vary depending on the device used).

The current pre-release code is 0.0.17 (alpha2)

Just clone this project into your home directory on the Raspberry Pi or board with similar bus using:

    git clone https://github.com/wittend/rm3100-runMag.git

Then do:

    $ cd rm3100-runMag
    $ make


and if all goes well type:

    $ sudo ./runMag
    (most flavors of Linux will require sudo)


and now you should see some results!

## Example on Raspberry Pi 3/4, using logging:

This creates a logfile in the directory ./logs with the format 'kd0eag-20200624-runmag.log'.
Here 'kd0eag' is used as a site ID and '20200624' is the UTC 'YYYYMMDD' format of date.
This file will be closed at the end of the current UTC day and a new one opened named with the new day number.
Logging will continue to the new file uninterrupted.

    dave@raspi-3: ~/projects/rm3100-runMag $ ./runMag -kPS kd0eag



## Example on Odroid N2, output JSON, show totalized magnetic field.

    dave@odroid:~/$ sudo ./runMag -j -Z -b 2

    { ts:"03 May 2020 23:56:55",  lt:"24.62", x:"14" , y:"-11" , z:"49", rx:"1109", ry:"-844", rz:"3707", Tm: "52" }
    { ts:"03 May 2020 23:56:59",  lt:"24.62", x:"14" , y:"-11" , z:"49", rx:"1111", ry:"-865", rz:"3712", Tm: "52" }
    { ts:"03 May 2020 23:57:03",  lt:"24.62", x:"14" , y:"-11" , z:"49", rx:"1105", ry:"-862", rz:"3706", Tm: "52" }
    ...

    Tm : (sqrt((x*x) + (y*y) + (z*z)))


## Example output using -h or -? option:

    dave@raspi-3:~/projects/rm3100-runMag $/rm3100-runMag$ ./runMag -h

./runMag Version = 0.0.17 (alpha2)

Parameters:

   -a                     :  List known SBC I2C bus numbers.       [ use with -b ]
   -A                     :  Set NOS (0x0A) register value.        [ Don't use unless you know what you are doing ]
   -B <reg mask>          :  Do built in self test (BIST).         [ Not implemented ]
   -b <bus as integer>    :  I2C bus number as integer.
   -C                     :  Read back cycle count registers before sampling.
   -c <count>             :  Set cycle counts as integer.          [ default 200 decimal]
   -D <rate>              :  Set magnetometer sample rate.         [ TMRC reg 96 hex default ].
   -d <delay as ms>       :  Output delay.                         [ 1000 ms default ]
   -E                     :  Show cycle count/gain/sensitivity relationship.
   -f <filename>          :  Read configuration from file (JSON).  [ Not implemented ]
   -F <filename>          :  Write configuration to file (JSON).   [ Not implemented ]
   -g <mode>              :  Device sampling mode.                 [ POLL=0 (default), CONTINUOUS=1 ]
   -H                     :  Hide raw measurments.
   -j                     :  Format output as JSON.
   -k                     :  Create and roll log files.            [ 00:00 UTC default ]
   -L <addr as integer>   :  Local temperature address.            [ default 19 hex ]
   -l                     :  Read local temperature only.
   -M <addr as integer>   :  Magnetometer address.                 [ default 20 hex ]
   -m                     :  Read magnetometer only.
   -O <filename>          :  Output file path.                     [ Must be valid path with write permissions ]
   -P                     :  Show Parameters.
   -q                     :  Quiet mode.                           [ partial ]
   -v                     :  Verbose output.
   -R <addr as integer>   :  Remote temperature address.           [ default 18 hex ]
   -r                     :  Read remote temperature only.
   -s                     :  Return single reading.                [ Do one measurement loop only ]
   -S                     :  Site prefix string for log files.     [ 32 char max. Do not use /'"* etc. Try callsign! ]
   -T                     :  Raw timestamp in milliseconds.        [ default: UTC string ]
   -U <delay as ms>       :  Delay in mSec before DRDY.            [ default: 0 ]
   -V                     :  Display software version and exit.
   -Z                     :  Show total field.                     [ sqrt((x*x) + (y*y) + (z*z)) ]
   -h or -?               :  Display this help.


## Example output using the -E option:

    dave@odroid:~/projects/rm3100-runMag$ sudo ./runMag -E
        -----------------------------------------------------------------------
        |    Cycle Count/Gain/Sensitivity        |     RM3100 Measurement     |
        |---------------------------------------------------------------------|
        | Cycle |   Gain   |                     |                            |
        | Count | (LSB/uT) | Sensitivity(nT/LSB) | in count | microTesla (uT) |
        |---------------------------------------------------------------------|
        |   50  |    20    |      50.000         |   3000   |     150.000     |
        |  100  |    38    |      26.316         |   3000   |      78.947     |
        |  200  |    75    |      13.333         |   3000   |      40.000     |
        |  300  |   113    |       8.850         |   3000   |      26.549     |
        |  400  |   150    |       6.667         |   3000   |      20.000     |
        -----------------------------------------------------------------------
    From: RM3100_FAQ_R02.pdf


## Example output using the -P option:

    dave@raspi-3:~/projects/rm3100-runMag $ ./runMag -Psl

        Version = 0.0.7

        Current Parameters:

            Magnetometer revision ID detected:          34 (dec)
            Log output path:                            TRUE
            Log output:                                 TRUE
            Log site prefix string:                     kd0eag
            Output file path:                           ./logs/kd0eag-20200624-runmag.log
            I2C bus number as integer:                  1 (dec)
            I2C bus path as string:                     /dev/i2c-1
            Built in self test (BIST) value:            00 (hex)
            NOS Register value:                         00 (hex)
            Device sampling mode:                       POLL
            Cycle counts by vector:                     X: 200 (dec), Y: 200 (dec), Z: 200 (dec)
            Gain by vector:                             X:  75 (dec), Y:  75 (dec), Z:  75 (dec)
            Read back CC Regs after set:                FALSE
            Software Loop Delay (uSec):                 1000000 (dec)
            Magnetometer sample rate:                   200 (dec)
            CMM magnetometer sample rate (TMRC reg):    150 (dec)
            Format output as JSON:                      TRUE
            Read local temperature only:                TRUE
            Read remote temperature only:               FALSE
            Read magnetometer only:                     FALSE
            Local temperature address:                  18 (hex)
            Remote temperature address:                 19 (hex)
            Magnetometer address:                       20 {hex)
            Show parameters:                            TRUE
            Quiet mode:                                 TRUE
            Hide raw measurements:                      FALSE
            Return single magnetometer reading:         FALSE
            Read Simple Magnetometer Board (SMSB):      TRUE
            Read Board with Extender (MSBx):            FALSE
            Magnetometer configuation:                  Local standalone
            Timestamp format:                           UTCSTRING
            Verbose output:                             FALSE
            Show total field:                           FALSE


     Time: 12 Jun 2020 04:29:12, lTemp: 23.88, x: -8.160, y: 6.573, z: 54.813, rx: -612, ry: 493, rz: 4111
