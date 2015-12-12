OUTPUT_NAME=gatttool
COMPILE_CROSS=mips-linux-gnu
CURRENT=`pwd`
PREFIX=/home/fshang/f/wicom_code/BlueZ/output/app/usr

search_str="lib\/bluetooth\.h"
search_file="attrib/gatt.c"
if res=`grep -n $search_str $search_file`
then
    echo $search_file "already changed."
else
    sed -i '/glib.h/a #include "lib/bluetooth.h"' $search_file
fi

CFLAG="-DVERSION="5.31" \
	   -I. \
	   -I./lib \
	   -I$PREFIX/include \
	   -I$PREFIX/include/dbus-1.0 \
	   -I$PREFIX/include/glib-2.0 \
	   -I$PREFIX/include/bluetooth \
	   -I$PREFIX/include/readline \
	   -I$PREFIX/lib/glib-2.0/include \
	   -I$PREFIX/lib/dbus-1.0/include \
	   "
LDFLAG="-L$PREFIX/lib \
		"
LIBS="-ldbus-1 \
	  -lglib-2.0 \
	  -lreadline \
	  -lncurses \
	  "
SRC="attrib/gatttool.c \
	 attrib/gatt.c \
	 src/dbus-common.c \
	 src/log.c \
	 src/textfile.c \
	 src/uuid-helper.c \
	 src/shared/att.c \
	 src/shared/crypto.c \
	 src/shared/io-glib.c \
	 src/shared/mainloop.c \
	 src/shared/mgmt.c \
	 src/shared/queue.c \
	 src/shared/timeout-glib.c \
	 src/shared/util.c \
	 client/display.c \
	 btio/btio.c \
	 gdbus/client.c \
	 gdbus/mainloop.c \
	 gdbus/object.c \
	 gdbus/polkit.c \
	 gdbus/watch.c \
	 attrib/att.c \
	 attrib/gattrib.c \
	 attrib/interactive.c \
	 attrib/utils.c \
	 lib/bluetooth.c \
	 lib/uuid.c \
	 lib/hci.c \
	 lib/sdp.c \
	 "
$COMPILE_CROSS-gcc $CFLAG $LDFLAG $INC $LIBS $SRC -o $OUTPUT_NAME

if res=`grep -n $search_str $search_file`
then
    sed -i /$search_str/d $search_file
fi
