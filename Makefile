#-----------------------------------
# Simple Makefile for rm3100i2c.
#-----------------------------------
CC = gcc
LD = gcc
CXX = g++
DEPS = device_defs.h i2c.h
SRCS = simplei2c.c i2c.c
OBJS = $(subst .c,.o,$(SRCS))
DOBJS = simplei2c.o i2c.o
LIBS = -lm
DEBUG = -g 
CFLAGS = -I.
LDFLAGS =
TARGET_ARCH =
LOADLIBES =
LDLIBS = 

TARGET = simplei2c

RM = rm -f

all: release

debug: simplei2c.c $(DEPS) 
	$(CC) -c $(DEBUG) i2c.c  
	$(CC) -o $(TARGET) $(DEBUG) simplei2c.c i2c.o $(LIBS)

release: simplei2c.c $(DEPS)
	$(CC) -c $(CFLAGS) i2c.c  
	$(CC) -o $(TARGET) $(CFLAGS) simplei2c.c i2c.o $(LIBS)

clean:
	$(RM) $(OBJS) $(TARGET)

distclean: clean
	
.PHONY: clean distclean all debug release