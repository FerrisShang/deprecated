OUTPUT_NAME=hcitool
COMPILE_CROSS=mips-linux-gnu
CURRENT=`pwd`
PREFIX=/home/fshang/code/bluez5.31/output/app/usr
CFLAG="-DVERSION=\"5.31\" \
	   -I. \
	   -ffunction-sections -fdata-sections \
	   "
LDFLAG="-Wl,--gc-sections \
		"
LIBS=" \
	  "
SRC="src/oui.c \
	 tools/hcitool.c \
	 lib/hci.c \
	 lib/bluetooth.c \
	 "
$COMPILE_CROSS-gcc $CFLAG $LDFLAG $INC $LIBS $SRC -o $OUTPUT_NAME
$COMPILE_CROSS-strip $OUTPUT_NAME
