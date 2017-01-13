#!/bin/bash

HOST=mips-linux-gnu
CC=$HOST-gcc
STRIP=$HOST-strip


DIR_CURRENT=`pwd`
OUTPUT_FOLDER=$DIR_CURRENT/../../../../script/bluez_base/bluez5.43/output

$CC   \
		-I$OUTPUT_FOLDER/usr/include/dbus-1.0 \
		-I$OUTPUT_FOLDER/usr/lib/dbus-1.0/include \
		-L$OUTPUT_FOLDER/usr/lib \
		-ldbus-1 \
		agent_bluez5.43.c \
		-o agent_bluez5.43

$STRIP agent_bluez5.43

