#!/bin/bash

export CC=mips-linux-gnu-gcc
export COMPILE_CROSS=mips-linux-gnu
export HOST=mips-linux-gnu
export PREFIX=/


DIR_CURRENT=`pwd`
COMPILE_FOLDER=$DIR_CURRENT/../../../../script/bluez_base/bluez5.43/compileCode
OUTPUT_FOLDER=$DIR_CURRENT/../../../../script/bluez_base/bluez5.43/output
SOURCE_CODE_FOLDER=$DIR_CURRENT/../../../../sourceCode

BLUEZ_5_43_NAME="bluez-5.43.tar.xz"
EXPAT_2_1_0_NAME="expat-2.1.0.tar.gz"
DBUS_1_8_8_NAME="dbus-1.8.8.tar.gz"
ZLIB_1_2_8_NAME="zlib-1.2.8.tar.gz"
LIBFFI_3_1_NAME="libffi-3.1.tar.gz"
GLIB_2_40_0_NAME="glib-2.40.0.tar.xz"
TERMCAPE_NAME="termcap-1.3.1.tar.gz"
READLINE_NAME="readline-master.tar.gz"

if [ -f "$SOURCE_CODE_FOLDER/$BLUEZ_5_43_NAME" ]
then
#compile
#cd $COMPILE_FOLDER/bluez-5.43/
	 $CC   \
		 -I$OUTPUT_FOLDER/usr/local/include \
		 -I$OUTPUT_FOLDER/usr/include \
		 -I$OUTPUT_FOLDER/usr/include/glib-2.0 \
		 -I$OUTPUT_FOLDER/usr/include/dbus-1.0 \
		 -I$OUTPUT_FOLDER/usr/lib/glib-2.0/include \
		 -I$OUTPUT_FOLDER/usr/lib/dbus-1.0/include \
		 -L$OUTPUT_FOLDER/usr/lib \
		 -L$OUTPUT_FOLDER/usr/local/lib \
		 -I$COMPILE_FOLDER/bluez-5.43 \
		 -fdata-sections -ffunction-sections \
		 -DVERSION=5.43 \
		 -lz \
		 -lffi \
		 -lreadline \
		 -ltermcap \
		 -lglib-2.0 \
		 -ldbus-1 \
		 -rdynamic \
		 signal_recv.c \
		 -o signal_recv \
		 -Wall \

	 cd $DIR_CURRENT
else
	 echo "bluez5.43 package download failed, exit now"
fi
