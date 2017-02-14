#!/bin/bash

export CC=mips-linux-gnu-gcc
export COMPILE_CROSS=mips-linux-gnu
export HOST=mips-linux-gnu
export PREFIX=/


DIR_CURRENT=`pwd`
COMPILE_FOLDER=$DIR_CURRENT/../../../../script/bluez_base/bluez5.43/compileCode
OUTPUT_FOLDER=$DIR_CURRENT/../../../../script/bluez_base/bluez5.43/output
SOURCE_CODE_FOLDER=$DIR_CURRENT/../../../../sourceCode

if [ -d "$OUTPUT_FOLDER" ]
then
#compile
#cd $COMPILE_FOLDER/bluez-5.43/
	 $CC   \
		 -I$OUTPUT_FOLDER/usr/include/dbus-1.0 \
		 -I$OUTPUT_FOLDER/usr/lib/dbus-1.0/include \
		 -L$OUTPUT_FOLDER/usr/lib \
		 -ldbus-1 \
		 signal_recv.c \
		 -o signal_recv \
		 -Wall \

	 cd $DIR_CURRENT
else
	 echo $OUTPUT_FOLDER
	 echo "output folder not exsit, exit now"
fi
