//=========================================================================
// main.h
// 
// An interface for the RM3100 3-axis magnetometer from PNI Sensor Corp.
// 
// Author:      David Witten, KD0EAG
// Date:        April 21, 2020
// License:     GPL 3.0
//=========================================================================
#ifndef SWX3100RUNMag_h
#define SWX3100RUNMag_h

#include "device_defs.h"
#include "i2c.h"
#include "MCP9808.h"
#include "runMag.h"
#include "main.h"

//------------------------------------------
// Prototypes
//------------------------------------------
int setup_mag(pList *p);
long currentTimeMillis();
int getTMRCReg(pList *p);
void setTMRCReg(pList *p);
int getCMMReg(pList *p);
void setCMMReg(pList *p);
unsigned short mag_set_sample_rate(pList *p, unsigned short sample_rate);
int readMag(pList *p, int32_t *XYZ);
void readCycleCountRegs(pList *p);
void setCycleCountRegs(pList *p);
void showSettings(pList *p);

#endif // SWX3100RUNMag_h