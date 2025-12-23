

  Wednesday, 11/20/2019
  =====================


		From the FAQ sent by PNI Tech Support:
		======================================

WHAT IS CYCLE COUNT AND HOW TO CHANGE IT?

- The internal clock count of RM3100 MagI2C ASIC establishes the number of sensor oscillation cycles. The number of oscillation cycle is “Cycle Count”.
- Cycle Count as default value is 200 or 0xC8 in hex. Each axis can have different cycle count, to change, set CCX, CCY and/or CCZ registers. They are 16-bit registers.

WHAT IS THE RELATIONSHIP BETWEEN CYCLE COUNT AND GAIN?

- Cycle count and gain are linearly related. See the following table on “Cycle Count” and “Gain”.
- Cycle Count/Gain/Sensitivity RM3100 Measurement
- Cycle  Count  |  Gain  (LSB/uT) |  Sensitivity(nT/LSB)  |  in count | microTesla (uT)
- 50 20 50.000 3000 150.000
- 100 38 26.316 3000 78.947
- 200 75 13.333 3000 40.000
- 300 113 8.850 3000 26.549
- 400 150 6.667 3000 20.000

WHAT IS THE RELATIONSHIP BETWEEN GAIN AND SENSITIVITY?

- Sensitivity is the inverse of gain, Sensitivity = 1/Gain. See Q4 Table “Gain” and “Sensitivity”.

HOW TO READ OUT RM3100 MEASUREMENT ?

- RM3100 measurement is a 24-bit signed integer in count.
-  //read out measurement from register 0x24 to the char mSamples[9] array
-  rm3100_i2c_read(Data_REG0X24, (char*)&mSamples, sizeof(mSamples)/sizeof(char));
-  SInt32 XYZ[3];
-  XYZ[0] = ((signed char)mSamples[0]) * 256 * 256;
-  XYZ[0] |= mSamples[1] * 256;
-  XYZ[0] |= mSamples[2];
-  XYZ[1] = ((signed char)mSamples[3]) * 256 * 256;
-  XYZ[1] |= mSamples[4] * 256;
-  XYZ[1] |= mSamples[5];
-  XYZ[2] = ((signed char)mSamples[6]) * 256 * 256;
-  XYZ[2] |= mSamples[7] * 256;
-  XYZ[2] |= mSamples[8];

HOW TO CONVERT RM3100 OUTPUT TO MICROTESLA?

- Convert the output of RM3100 from count to microTesla, the equation is,
- magnetic_field_in_uT = (RM3100_measurement_in_count / Gain)
- Please pay attention to “Cycle Count” setting. The default is 200, if customer changes to
- different value, the “Gain” will change too. Example given in Q4 Table.

WHAT IS THE MAXIMUM SAMPLE OR DATA RATE?

- The Cycle Count establish the maximum sample rate. For instance, if the cycle count is set to
- 200 as default, then the maximum 3-axis sample rate is ~430 Hz. Higher sample rates can be
- obtained by reducing the number of cycle counts, but this also diminishes gain and sensitivity.
- When sample rate is fixed, lower cycle count will reduce sensor oscillation time and in turn to
- reduce power consumption.

HOW TO CHANGE SAMPLE OR DATA RATE?

RM3100 TMRC register controls the time between measurements in Continuous Measurement
Mode. Its default is 0x92, indicating an update rate of ~600 Hz. When the Cycle Count is 200,
the maximum rate established by the Cycle Count will override the TMRC setting, and the
actual update rate will be ~430 Hz
