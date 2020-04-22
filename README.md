# rm3100-simple-i2c

For now just clone this project into your home directory on the Raspberry Pi using:

git clone https://github.com/wittend/rm3100-simple-i2c.  

Then do:

$ cd rm3100-simple-i2c

$ make

and if all goes well type:

$ ./simplei2c

and now you should see the results!

Example:

dave@odroid:~/projects/SWx-C/i2c-c$ ./simplei2c -h

./simplei2c Version = 0.0.2

Parameters:

    -b <number as integer> :  I2C bus number as integer.
    -c <count>             :  Cycle count as integer (default 200).
    -j                     :  Format output as JSON.
    -l                     :  Read local temperature only.
    -L [addr as integer]   :  Local temperature address [default 19 hex].
    -m                     :  Read magnetometer only.
    -M [addr as integer]   :  Magnetometer address [default 20 hex].
    -q                     :  Quiet mode.
    -r                     :  Read remote temperature only.
    -R [addr as integer]   :  Remote temperature address [default 18 hex].
    -s                     :  Return single magnetometer reading.
    -S                     :  Read Simple Magnetometer Support Board.
    -v                     :  Verbose output.
    -V                     :  Display software version and exit.
    -X                     :  Read Board with Extender (default).
    -h or -?               :  Display this help.
