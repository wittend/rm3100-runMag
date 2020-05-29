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
int openI2CBus(pList *p);
void closeI2CBus(int i2c_fd);
int setNOSReg(pList *p);
unsigned short setMagSampleRate(pList *p, unsigned short sample_rate);
unsigned short getMagSampleRate(pList *p);;
unsigned short getCCGainEquiv(unsigned short CCVal);
int startCMM(pList *p);
int getMagRev(pList *p);
int setup_mag(pList *p);
int runBIST(pList *p);
int getCMMReg(pList *p);
void setCMMReg(pList *p);
int getTMRCReg(pList *p);
void setTMRCReg(pList *p);
void setCycleCountRegs(pList *p);
void readCycleCountRegs(pList *p);

#endif // SWX3100RUNMag_h