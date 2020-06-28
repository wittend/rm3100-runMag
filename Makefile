#=========================================================================
# Simple Makefile for runMag
#
# Author:      David Witten, KD0EAG
# Date:        April 21, 2020
# License:     GPL 3.0
#=========================================================================
CC = gcc
LD = gcc
GPERF = gperf
CXX = g++
#DEPS = main.h MCP9808.h device_defs.h i2c.h runMag.h cmdmgr.h config.gperf cfghash.c  
DEPS = main.h MCP9808.h device_defs.h i2c.h runMag.h cmdmgr.h uthash/uthash.h
#SRCS = main.c runMag.c i2c.c cmdmgr.c cfghash.c
SRCS = main.c runMag.c i2c.c cmdmgr.c
OBJS = $(subst .c,.o,$(SRCS))
#DOBJS = main.o runMag.o i2c.o cmdmgr.o cfghash.o 
DOBJS = main.o runMag.o i2c.o cmdmgr.o
LIBS = -lm
DEBUG = -g -Wall
CFLAGS = -I.
LDFLAGS =
TARGET_ARCH =
LOADLIBES =
LDLIBS =
GPERFFLAGS = --language=ANSI-C 

TARGET = runMag

RM = rm -f

all: release

#cfghash.c: config.gperf
#	$(GPERF) $(GPERFFLAGS) config.gperf > cfghash.c

# debug: runMag.c cfghash.c $(DEPS) 
debug: runMag.c $(DEPS) 
	$(CC) -c $(DEBUG) runMag.c  
	$(CC) -c $(DEBUG) cmdmgr.c  
	$(CC) -c $(DEBUG) i2c.c
#	$(CC) -c $(DEBUG) cfghash.c
#	$(CC) -o $(TARGET) $(DEBUG) main.c runMag.o i2c.o cmdmgr.o cfghash.o $(LIBS)
	$(CC) -o $(TARGET) $(DEBUG) main.c runMag.o i2c.o cmdmgr.o $(LIBS)

#release: runMag.c cfghash.c $(DEPS)
release: runMag.c $(DEPS)
	$(CC) -c $(CFLAGS) runMag.c
	$(CC) -c $(CFLAGS) cmdmgr.c
	$(CC) -c $(CFLAGS) i2c.c  
#	$(CC) -c $(CFLAGS) cfghash.c
#	$(CC) -o $(TARGET) $(CFLAGS) main.c runMag.o i2c.o cmdmgr.o cfghash.o $(LIBS)
	$(CC) -o $(TARGET) $(CFLAGS) main.c runMag.o i2c.o cmdmgr.o $(LIBS)

clean:
#	$(RM) $(OBJS) $(TARGET) cfghash.c config.json
	$(RM) $(OBJS) $(TARGET) config.json

distclean: clean
	
.PHONY: clean distclean all debug release