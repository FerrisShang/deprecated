OUTPUT_NAME=btgatt-server
COMPILE_CROSS=mips-linux-gnu
CURRENT=`pwd`
PREFIX=/home/fshang/f/bluez/output/app/usr
CFLAG="-I. \
	   -fdata-sections -ffunction-sections
	   "
LDFLAG="-L$PREFIX/lib \
		-Wl,--gc-sections
		"
LIBS="-lrt \
	  -lpthread \
	  "
SRC="tools/btgatt-server.c \
	 src/shared/mainloop.c \
	 src/shared/gatt-db.c \
	 src/shared/gatt-server.c \
	 src/shared/att.c \
	 src/shared/queue.c \
	 src/shared/util.c \
	 src/shared/crypto.c \
	 src/shared/io-mainloop.c \
	 src/shared/timeout-mainloop.c \
	 lib/bluetooth.c \
	 lib/uuid.c \
	 lib/hci.c \
	 "
$COMPILE_CROSS-gcc $CFLAG $LDFLAG $INC $LIBS $SRC -o $OUTPUT_NAME
$COMPILE_CROSS-strip $OUTPUT_NAME
#adb push $OUTPUT_NAME /
#scp $OUTPUT_NAME user@192.168.3.92:~/fshang/adb_halley/bt_tool/
