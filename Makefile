#-----------------------------------
# Simple Makefile for rm3100i2c.
#-----------------------------------
CC=gcc
LD=gcc
CXX=g++
RM=rm -f
TARGET=simplei2c

SRCS=simplei2c.c
OBJS=$(subst .c,.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(TARGET).c device_defs.h
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET) 

clean:
	$(RM) $(OBJS) $(TARGET)

distclean: clean
	$(RM) $(TARGET)
	
.PHONY: clean distclean all