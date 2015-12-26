CC=mips-linux-gnu-gcc
STRIP=strip
#CC = mipsel-linux-gcc
#STRIP = mipsel-linux-strip


LDFLAGS = -lpthread


TARGET = test
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))

all:$(TARGET)
$(TARGET):$(OBJS)
	$(CC) $? -o $@ $(LDFLAGS)
	rm *.o

