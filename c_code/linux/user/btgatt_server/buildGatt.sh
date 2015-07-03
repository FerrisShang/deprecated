OUTPUT_NAME=btgatt_server
COMPILE_CROSS=mips-linux-gnu
CURRENT=`pwd`
PREFIX=$CURRENT/../bluez/output/app/usr
CFLAG="-I. \
	   -I./lib \
	   -I$PREFIX/include \
	   -I$PREFIX/include/dbus-1.0 \
	   -I$PREFIX/include/glib-2.0 \
	   -I$PREFIX/include/libical \
	   -I$PREFIX/include/bluetooth \
	   -I$PREFIX/include/readline \
	   -I$PREFIX/lib/glib-2.0/include \
	   "
LDFLAG="-L$PREFIX/lib \
		"
LIBS="-ldbus-1 \
	  -lrt \
	  -lpthread \
	  -lglib-2.0 \
	  "
SRC="tools/btgatt-server.c \
	 src/shared/mainloop.c \
	 src/shared/gatt-db.c \
	 src/shared/gatt-server.c \
	 src/shared/timeout-glib.c \
	 src/shared/att.c \
	 src/shared/queue.c \
	 src/shared/util.c \
	 src/shared/crypto.c \
	 src/shared/io-mainloop.c \
	 lib/bluetooth.c \
	 lib/uuid.c \
	 lib/hci.c \
	 "
$COMPILE_CROSS-gcc $CFLAG $LDFLAG $INC $LIBS $SRC -o $OUTPUT_NAME
$COMPILE_CROSS-strip $OUTPUT_NAME

