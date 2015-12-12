OUTPUT_NAME=gatttool
COMPILE_CROSS=mips-linux-gnu
PREFIX=/home/fshang/f/wicom_code/BlueZ/output/app/usr
CFLAG="-DVERSION="4.101" \
	   -I. \
	   -I./src \
	   -I./btio \
	   -I./gdbus \
	   -I./attrib \
	   -I./lib \
	   -I$PREFIX/include \
	   -I$PREFIX/include/dbus-1.0 \
	   -I$PREFIX/include/glib-2.0 \
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

SRC=" ./attrib/gattrib.c \
	  ./attrib/utils.c \
	  ./attrib/interactive.c \
	  ./attrib/gatt.c \
	  ./attrib/gatttool.c \
	  ./attrib/att.c \
	  ./lib/uuid.c \
	  ./lib/bluetooth.c \
	  ./lib/hci.c \
	  ./lib/sdp.c \
	  ./src/textfile.c \
	  ./src/log.c \
	  ./src/dbus-common.c \
	  ./src/glib-helper.c \
	  ./gdbus/object.c \
	  ./gdbus/polkit.c \
	  ./gdbus/mainloop.c \
	  ./gdbus/watch.c \
	  ./btio/btio.c \
"

$COMPILE_CROSS-gcc $CFLAG $LDFLAG $INC $LIBS $SRC -o $OUTPUT_NAME
