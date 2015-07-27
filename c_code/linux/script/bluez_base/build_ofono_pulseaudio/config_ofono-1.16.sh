./configure \
		--host=$HOST \
		--prefix=$PREFIX/usr \
		--sysconfdir=$PREFIX/etc \
		CFLAGS="-I$PREFIX/usr/include -fdata-sections -ffunction-sections" \
		LDFLAGS="-L$PREFIX/usr/lib -Wl,--gc-sections" \
		DBUS_CFLAGS="-I$PREFIX/usr/lib/dbus-1.0/include -I$PREFIX/usr/include/dbus-1.0" \
		DBUS_LIBS="-L$PREFIX/usr/lib -ldbus-1 -lpthread -lrt" \
		GLIB_CFLAGS="-I$PREFIX/usr/lib/glib-2.0/include -I$PREFIX/usr/include/glib-2.0" \
		GLIB_LIBS="-L$PREFIX/usr/lib -lglib-2.0 -pthread -lrt" \
		BLUEZ_CFLAGS="-I$PREFIX/usr/include/bluetooth" \
		BLUEZ_LIBS="-L$PREFIX/usr/lib -lbluetooth" \
		--enable-static \
		--enable-bluetooth \
		--enable-bluez4 \
		--disable-debug \
		--enable-tools \
		--disable-udev \

make -j
make install
