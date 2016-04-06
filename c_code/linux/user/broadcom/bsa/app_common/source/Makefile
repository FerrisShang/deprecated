CC = mipsel-linux-gcc
STRIP = mipsel-linux-strip

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.c.o,$(SRCS))

CFLAGS += -I../include -fPIC
LDFLAGS = -shared 

all: libappcommon.so

libappcommon.so: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

%.c.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	-rm -f $(OBJS) libappcommon.so

.PHONY: all clean
