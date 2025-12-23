-- Notes for setting I2C up to work on RAS-Pi 3 & 4

Setting I2C up on the Raspberry Pi's is fairly simple.  I will assume that you will use the current default full Raspbian OS.  It uses the same image for all versions:

I suggest:
Raspbian Buster with desktop
Image with desktop based on Debian Buster
Version:February 2020
Release date:2020-02-13
Kernel version:4.19
Size:1136 MB
https://downloads.raspberrypi.org/raspbian_latest

When you have this zip file, you need to uncompress it with a zip64 compatible unzip tool:
my favorite for Windows is 7-Zip. for Mac, apparently The Unarchiver is equivalent. for Linux Unzip works.

THe instructions from the official Raspberry Pi site say:

balenaEtcher is a graphical SD card writing tool that works on Mac OS, Linux and Windows, and is the easiest option for most users. balenaEtcher also supports writing images directly from the zip file, without any unzipping required. To write your image with balenaEtcher:

Download the latest version of balenaEtcher and install it.
Connect an SD card reader with the SD card inside.
Open balenaEtcher and select from your hard drive the Raspberry Pi .img or .zip file you wish to write to the SD card.
Select the SD card you wish to write your image to.
Review your selections and click 'Flash!' to begin writing data to the SD card.
Note: for Linux users, zenity might need to be installed on your machine for balenaEtcher to be able to write the image on your SD card.

For more advanced control of this process, see our system-specific guides:

Linux
Mac OS
Windows
Chrome OS
There are other ways to do ths as well, but this should be safe.

Now you need to plug in monitor, keyboard, and mouse.  I prefer a wireless keyboard/mouse combo. If possible, connect a wired network connection at first, but a reliable WiFi environment should work.

Then power up the Pi and let it guide you through the setup.  Let it restart,

In the version that I just installed, you can select Menu > Preferences > Raspberry Pi Configuration and then you get an applet thing.  It you select the tab "Interfaces" you can turn I2C and I also Selected SSH.  You might also consider Serial Port and VNC.

Alternatively, from a terminal window/command prompt, you can type sudo raspi-config and make these change
Again,you should reboot the Pi.

Now open a terminal window and type ls /dev/i2c*.  You should see it respond with "/dev/i2c-1". This indicates that the system driver is loaded for I2C bus 1.  It will be different on different types of SBC.  I have one that lists 0 - 8.

Now you want to type the command "i2cdetect -y 1"

You should see something like this:

     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --                         

The dashes are unoccupied addresses (hexadecimal) on I2C bus 1

An example with some devices on the bus will look like:

     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- 37 -- -- 3a -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- 4a 4b -- -- -- --
50: 50 -- -- -- 54 -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- -- 
    
the numbers indicate the presence of devices at those addresses, for example at 0x37 (in C syntax)                    
(this is from my desktop Linux machine).

Now if you plug in the Qwiic cable and the Magnetometer unit (use either connector) You should see:
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- 18 -- -- -- -- -- -- --
20: 20 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --                         

On the board that I sent you there two devices.  The one at 0x18 is the MCP9808 precision temperature sensor.  The one at 0x20 is the magnetometer module.  It is the job of the software to request register values from these addresses and to format them into something we can read.  

That is enough for this moment, I will forward code and instructions for using it this afternoon.
