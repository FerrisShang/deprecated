OUTPUT_NAME=gatttool
COMPILE_CROSS=mips-linux-gnu
CURRENT=`pwd`
PREFIX=/home/fshang/f/bluez/output/app/usr
CFLAG="-DHAVE_CONFIG_H \
	   -DPLUGINDIR=\"/etc/bluetooth/plugins\" \
	   -I. \
	   -I./lib \
	   -I$PREFIX/include \
	   -I$PREFIX/include/dbus-1.0 \
	   -I$PREFIX/include/glib-2.0 \
	   -I$PREFIX/include/libical \
	   -I$PREFIX/include/bluetooth \
	   -I$PREFIX/include/readline \
	   -I$PREFIX/lib/glib-2.0/include \
	   -I$PREFIX/lib/dbus-1.0/include \
	   "
LDFLAG="-L$PREFIX/lib \
		"
LIBS="-ldbus-1 \
	  -lrt \
	  -lpthread \
	  -lglib-2.0 \
	  -lreadline \
	  -lncurses \
	  "
SRC="src/adapter.c \
	 attrib/gatttool.c \
	 src/advertising.c \
	 src/agent.c \
	 src/attrib-server.c \
	 src/dbus-common.c \
	 src/device.c \
	 src/eir.c \
	 src/error.c \
	 src/gatt-client.c \
	 src/gatt-database.c \
	 src/log.c \
	 src/main.c \
	 src/oui.c \
	 src/profile.c \
	 src/rfkill.c \
	 src/sdp-client.c \
	 src/sdpd-database.c \
	 src/sdpd-request.c \
	 src/sdpd-server.c \
	 src/sdpd-service.c \
	 src/sdp-xml.c \
	 src/service.c \
	 src/storage.c \
	 src/systemd.c \
	 src/textfile.c \
	 src/uuid-helper.c \
	 src/shared/ad.c \
	 src/shared/att.c \
	 src/shared/btsnoop.c \
	 src/shared/crypto.c \
	 src/shared/ecc.c \
	 src/shared/gap.c \
	 src/shared/gatt-client.c \
	 src/shared/gatt-db.c \
	 src/shared/gatt-helpers.c \
	 src/shared/gatt-server.c \
	 src/shared/hci.c \
	 src/shared/hci-crypto.c \
	 src/shared/hfp.c \
	 src/shared/io-glib.c \
	 src/shared/mainloop.c \
	 src/shared/mgmt.c \
	 src/shared/pcap.c \
	 src/shared/queue.c \
	 src/shared/ringbuf.c \
	 src/shared/timeout-glib.c \
	 src/shared/uhid.c \
	 src/shared/util.c \
	 client/display.c \
	 peripheral/gatt.c \
	 btio/btio.c \
	 gdbus/client.c \
	 gdbus/mainloop.c \
	 gdbus/object.c \
	 gdbus/polkit.c \
	 gdbus/watch.c \
	 attrib/att.c \
	 attrib/gatt.c \
	 attrib/gattrib.c \
	 attrib/gatt-service.c \
	 attrib/interactive.c \
	 attrib/utils.c \
	 lib/bluetooth.c \
	 lib/uuid.c \
	 lib/hci.c \
	 lib/sdp.c \
	 "
$COMPILE_CROSS-gcc $CFLAG $LDFLAG $INC $LIBS $SRC -o $OUTPUT_NAME
#编译时需要删除src/main.c中的 main函数
