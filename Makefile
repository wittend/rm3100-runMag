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
DEPS = device_defs.h i2c.h
SRCS = runMag.c i2c.c
OBJS = $(subst .c,.o,$(SRCS))
DOBJS = runMag.o i2c.o
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

debug: runMag.c $(DEPS) 
	$(CC) -c $(DEBUG) i2c.c  
	$(CC) -o $(TARGET) $(DEBUG) runMag.c i2c.o $(LIBS)

release: runMag.c $(DEPS)
	$(CC) -c $(CFLAGS) i2c.c  
	$(CC) -o $(TARGET) $(CFLAGS) runMag.c i2c.o $(LIBS)

clean:
	$(RM) $(OBJS) $(TARGET)

distclean: clean
	
.PHONY: clean distclean all debug release