#-----------------------------------
# Simple Makefile for rm3100i2c
#-----------------------------------
CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g $(shell root-config --cflags)
LDFLAGS=-g $(shell root-config --ldflags)
LDLIBS=$(shell root-config --libs)
TARGET=i2c-mag-temp

SRCS=$(TARGET).c,i2c.c
OBJS=$(subst .c,.o,$(SRCS))

all: $(TARGET)

$(TARGET).o  : $(TARGET).c i2c_local.o device_defs.h i2c_local.h
	$(CC) $(CXXFLAGS) -c $(TARGET).c i2c_local.o

i2c_local.o  : i2c_local.c device_defs.h i2c_local.h
	$(CC) $(CXXFLAGS) i2c_local.c

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(OBJS) $(TARGET)

distclean: clean
	$(RM) $(TARGET)