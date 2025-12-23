

  Tuesday, 11/19/2019
  ===================


		RM3100 Magnetometer Support Board
		=================================

Updated: Tuesday, 11/19/2019 10:49:35 AM

Audience


This printed circuit board (PCB) was created to support the use of the RM3100 Geomagnetic Sensors produced by PNI Corporation.  It is intended to provide easy access to these sensors using the I2C protocol using small single board computers such as the Raspberry Pi, Arduino, and their variants.  The intended audience includes both individual experimenters and professional researchers interested in space weather, geomagnetic phenomena, and similar interests.

Design Considerations


This board  is intended be inexpensive yet versitile. It has two high level functions which may be used together or seperately.

- Support the connection to the magnetometer sensors with commonly used options.
- Allow extension of the I2C bus (in pairs) using off-the-shelf Cat 6 sheilded cable.  The intent is to reduce the noise floor between host SBCs and the actual sensors themelves.

Use Cases


- A single board may have a sensor mounted and be attached directly to an SBC via a short four wire cable (< 50 mm) with 3.3v levels (typically for benchtop testing).
- A local board may be used without a magnetometer as a sending unit.  A second board is connected to the first by a (TBD; < 30m) length of shielded Cat 6 cable.  The second unit is populated with the magnetometer sensors.  The first unit is signaled by 3.3v I2C from the host.  It is also provided with 5v by the host device which is used to power signalling on differential pair clock and data lines.  5v is also sent on two additional pairs from the local to the remote units. At the remote unit the 5v is down-regulated to 3.3v to power the sensors.
- In each of the other cases it should be possible to daisy-chain additional sensors on the same bus, within the limits of available current and bandwidth, so long as they can operate at 3.3v.

Includes I2c connector footprints for:

- SparkFun Qwiic/Adafruit Stemma-QT, 
- Seeed Grove or JST-PH
- Digilent PMOD (new I2C 12 pin.).

CAD and Schematics


[file:///home/dave/Projects/SWx/rm3100_w_PCA9615/print/basic-rm3100_3d_2019-11-16_v2.jpg]

schematic_04_2019-11-16.pdf (file:///home/dave/Projects/SWx/rm3100_w_PCA9615/print/schematic_04_2019-11-16.pdf)


Reference Links


- PNI Corporation: https://www.pnicorp.com/ 

- RM3100 Geomagnetic Sensor: https://www.pnicorp.com/rm3100/ 

- NXP PCA9615 I2C differential extender: https://www.nxp.com/docs/en/data-sheet/PCA9615.pdf 



- HamSci Space Weather Station Working Page: https://hamsci.org/space-weather-station-working-page 

- TangerineSDR Project: https://tangerinesdr.com/


