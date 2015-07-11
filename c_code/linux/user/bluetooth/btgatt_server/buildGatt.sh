
#该脚本用于编译出蓝牙ble 心率demo
#为方便测试在main函数中添加了 le_addr(0);用于设置随机地址，
#而正常情况下随机地址应该在固件烧录后设置且不应该随便更改

cp Makefile.gatt Makefile
make
mips-linux-gnu-strip btgatt-server

#adb push $OUTPUT_NAME /
#scp $OUTPUT_NAME user@192.168.3.92:~/fshang/adb_halley/bt_tool/
