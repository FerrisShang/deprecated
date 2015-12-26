CC=gcc
STRIP=strip
#CC = mipsel-linux-gcc
#STRIP = mipsel-linux-strip


CFLAGS = -I/usr/include 
LDFLAGS = -L/usr/lib/ -pthread


TARGET = test
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))

all:$(TARGET)
$(TARGET):$(OBJS)
	$(CC) $? -o $@ $(LDFLAGS)
	rm *.o

