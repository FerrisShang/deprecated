#!/bin/sh
RANDISK_NAME=rootfs_wifi.cpio.gz
TARGET_FILE_NAME="fshang@192.168.3.4:~/halley-linux310/sources/kernel/linux-3.10.14/arch/mips/xburst/soc-4775/chip-m150/halley/common/rootfs_wifi.cpio.gz"
CP=scp

cd ramdisk
find . | cpio -o -H newc | gzip > ../$RANDISK_NAME
cd ../ 

$CP $RANDISK_NAME $TARGET_FILE_NAME
