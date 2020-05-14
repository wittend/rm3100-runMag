# rm3100-runMag

For now just clone this project into your home directory on the Raspberry Pi or board with similar bus using:

    git clone https://github.com/wittend/rm3100-runMag.git

Then do:

    $ cd rm3100-runMag
    $ make
    

and if all goes well type:

    $ sudo ./runMag
    (most flavors of Linux will require sudo)
    

and now you should see some results!

## Example:

    dave@odroid:~/$ sudo ./runMag -j -Z
    
    { ts:"03 May 2020 23:56:55",  lt:"24.62", x:"14" , y:"-11" , z:"49", rx:"1109", ry:"-844", rz:"3707", Tm: "52" }
    { ts:"03 May 2020 23:56:59",  lt:"24.62", x:"14" , y:"-11" , z:"49", rx:"1111", ry:"-865", rz:"3712", Tm: "52" }
    { ts:"03 May 2020 23:57:03",  lt:"24.62", x:"14" , y:"-11" , z:"49", rx:"1105", ry:"-862", rz:"3706", Tm: "52" }
    ...
    

## Example:

    dave@odroid:~/$ sudo ./runMag -h
    
    ./runMag Version = 0.0.3
    
    Parameters:
    
       -a                     :  List known SBC I2C bus numbers (for use with -b).
       -B <reg mask>          :  Do built in self test (BIST).                  [Not really implemented].
       -b <bus as integer>    :  I2C bus number as integer.
       -C                     :  Read back cycle count registers before sampling.
       -c <count>             :  Set cycle counts as integer (default 200).
       -D <rate>              :  Set magnetometer sample rate (TMRC register).
       -d <count>             :  Set polling delay (default 1000000 uSec).
       -H                     :  Hide raw measurments.
       -j                     :  Format output as JSON.
       -L [addr as integer]   :  Local temperature address (default 19 hex).
       -l                     :  Read local temperature only.                   [testing].
       -M [addr as integer]   :  Magnetometer address (default 20 hex).
       -m                     :  Read magnetometer only.
       -P                     :  Show Parameters.
       -q                     :  Quiet mode.                                    [partial].
       -r                     :  Read remote temperature only                   [testing].
       -R [addr as integer]   :  Remote temperature address (default 18 hex).
       -s                     :  Return single reading.
       -T                     :  Raw timestamp in milliseconds (default: UTC string).
       -t                     :  Get/Set Continuous Measurement Mode Data Rate.
       -V                     :  Display software version and exit.
       -v                     :  Verbose output.
       -X                     :  Read Simple Magnetometer Board (SMSB).
       -x                     :  Read board with extender (MSBx).
       -Y                     :  Read Scotty's RPi Mag HAT standalone.          [UNTESTED]
       -y                     :  Read Scotty's RPi Mag HAT in extended mode.    [UNTESTED]
       -Z                     :  Show total field. sqrt((x*x) + (y*y) + (z*z))
       -h or -?               :  Display this help.


