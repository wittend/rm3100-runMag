//=========================================================================
// cmdmgr.h
// 
// header file for commandline / configuration management for runMag utility.
// 
// Author:      David Witten, KD0EAG
// Date:        June 19, 2020
// License:     GPL 3.0
//=========================================================================
#ifndef SWX3100CMDMGR_h
#define SWX3100CMDMGR_h

#include "runMag.h"
#include "main.h"


//------------------------------------------
// Prototypes
//------------------------------------------
long currentTimeMillis();
struct tm *getUTC();
void listSBCs();
int buildLogFilePath(pList *p);
void showCountGainRelationship();
int readConfigFromFile(pList *p, char *cfgFile);
int saveConfigToFile(pList *p, char *cfgFile);
void showSettings(pList *p);
int getCommandLine(int argc, char** argv, pList *p);


#endif // SWX3100CMDMGR_h
