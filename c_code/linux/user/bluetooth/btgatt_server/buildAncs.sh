cp Makefile.gatt Makefile
make
mips-linux-gnu-strip ancs_test

#adb push $OUTPUT_NAME /
#scp $OUTPUT_NAME user@192.168.3.92:~/fshang/adb_halley/bt_tool/
