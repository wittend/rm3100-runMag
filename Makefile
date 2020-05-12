#=========================================================================
# Simple Makefile for runMag
#
# Author:      David Witten, KD0EAG
# Date:        April 21, 2020
# License:     GPL 3.0
#=========================================================================
CC = gcc
LD = gcc
CXX = g++
DEPS = main.h MCP9808.h device_defs.h i2c.h runMag.h
SRCS = main.c runMag.c i2c.c
OBJS = $(subst .c,.o,$(SRCS))
DOBJS = main.o runMag.o i2c.o
LIBS = -lm
DEBUG = -g 
CFLAGS = -I.
LDFLAGS =
TARGET_ARCH =
LOADLIBES =
LDLIBS = 

TARGET = runMag

RM = rm -f

all: release

debug: main.c $(DEPS) 
	$(CC) -c $(DEBUG) runMag.c  
	$(CC) -c $(DEBUG) i2c.c  
	$(CC) -o $(TARGET) $(DEBUG) main.c runMag.c i2c.o $(LIBS)

release: runMag.c $(DEPS)
	$(CC) -c $(DEBUG) runMag.c
	$(CC) -c $(CFLAGS) i2c.c  
	$(CC) -o $(TARGET) $(CFLAGS) main.c runMag.c i2c.o $(LIBS)

clean:
	$(RM) $(OBJS) $(TARGET)

distclean: clean
	
.PHONY: clean distclean all debug release