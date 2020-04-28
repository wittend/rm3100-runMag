# rm3100-runMag

For now just clone this project into your home directory on the Raspberry Pi or similar bus using:

    git clone https://github.com/wittend/rm3100-runMag.

Then do:

$ cd rm3100-runMag

    Kluge Alert:
    check line 64 in I2C to be sure that the constant (from device_defs.h) for your single board computer is used.
    e.g.:     char i2cFname[] = ODROIDN2_I2C_BUS;
    This will be fixed!

$ make

and if all goes well type:

    $ sudo ./runMag
    (most flavors of Linux will require sudo)

and now you should see some results!

## Example:

    dave@odroid:~/$ sudo ./runMag -h

    ./runMag Version = 0.0.2

    Parameters:

    -b <bus as integer>    :  I2C bus number as integer.
    -B <reg mask>          :  Do built in self test (BIST). [Not really implemented].
    -c <count>             :  Set cycle counts as integer (default 200).
    -C                     :  Read cycle count registers.   [Not really implemented].   
    -H                     :  Hide raw measurments.
    -j                     :  Format output as JSON.
    -l                     :  Read local temperature only.  [Not really implemented].
    -L [addr as integer]   :  Local temperature address (default 19 hex).
    -m                     :  Read magnetometer only.
    -M [addr as integer]   :  Magnetometer address (default 20 hex).
    -P                     :  Show Parameters.
    -q                     :  Quiet mode.                   [Not really implemented].
    -r                     :  Read remote temperature only.
    -R [addr as integer]   :  Remote temperature address (default 18 hex).
    -s                     :  Return single reading.
    -S                     :  Read Simple Magnetometer Support Board.
    -T                     :  Raw timestamp in milliseconds (default: UTC string).
    -v                     :  Verbose output.               [Not really implemented].
    -V                     :  Display software version and exit.
    -X                     :  Read board with extender (default).
    -h or -?               :  Display this help.
   
   
   
