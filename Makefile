#=========================================================================
# Simple Makefile for testMag
#
# Author:      David Witten, KD0EAG
# Date:        April 21, 2020
# License:     GPL 3.0
#=========================================================================
CC = gcc
LD = gcc
GPERF = gperf
CXX = g++
#DEPS = main.h MCP9808.h device_defs.h i2c.h testMag.h cmdmgr.h config.gperf cfghash.c  
DEPS = main.h MCP9808.h device_defs.h i2c.h testMag.h cmdmgr.h
#SRCS = main.c testMag.c i2c.c cmdmgr.c cfghash.c
SRCS = main.c testMag.c i2c.c cmdmgr.c
OBJS = $(subst .c,.o,$(SRCS))
#DOBJS = main.o testMag.o i2c.o cmdmgr.o cfghash.o 
DOBJS = main.o testMag.o i2c.o cmdmgr.o
LIBS = -lm
DEBUG = -g -Wall
CFLAGS = -I.
LDFLAGS =
TARGET_ARCH =
LOADLIBES =
LDLIBS =
GPERFFLAGS = --language=ANSI-C 

TARGET = testMag

RM = rm -f

all: release

#cfghash.c: config.gperf
#	$(GPERF) $(GPERFFLAGS) config.gperf > cfghash.c

# debug: testMag.c cfghash.c $(DEPS) 
debug: testMag.c $(DEPS) 
	$(CC) -c $(DEBUG) testMag.c  
	$(CC) -c $(DEBUG) cmdmgr.c  
	$(CC) -c $(DEBUG) i2c.c
	$(CC) -o $(TARGET) $(DEBUG) main.c testMag.o i2c.o cmdmgr.o $(LIBS)

#release: testMag.c cfghash.c $(DEPS)
release: testMag.c $(DEPS)
	$(CC) -c $(CFLAGS) testMag.c
	$(CC) -c $(CFLAGS) cmdmgr.c
	$(CC) -c $(CFLAGS) i2c.c  
	$(CC) -o $(TARGET) $(CFLAGS) main.c testMag.o i2c.o cmdmgr.o $(LIBS)

clean:
	$(RM) $(OBJS) $(TARGET) config.json

distclean: clean
	
.PHONY: clean distclean all debug release