CC=mips-linux-gnu-gcc 

SOURCE_CODE="\
			 lescan.c \
			 lib/bluetooth.c \
			 lib/hci.c \
			"

CFLAGS="-I. -Ilib"
LDFLAGS=""

$CC $CFLAGS $LDFLAGS $SOURCE_CODE -o lescan
