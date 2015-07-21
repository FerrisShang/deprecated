PREFIX="/home/fshang/f/halley_linux3.10/sources/network/apps/output/app/usr"
./configure \
		--host=mips-linux-gnu \
		--enable-static \
		--enable-shared \
		--disable-udev \
		--disable-libtool-lock \
		--enable-debug \
		--enable-threads \
		--enable-test \
		--disable-tools \
		--disable-monitor \
		--disable-udev \
		--disable-cups \
		--enable-manpages \
		--enable-experimental \
		--without-dbusconfdir \
		--without-dbussystembusdir \
		--without-dbussessionbusdir \
		--without-udevdir \
		--without-systemdsystemunitdir \
		--without-systemduserunitdir \
		CFLAGS="-I$PREFIX/include -fdata-sections -ffunction-sections" \
		LDFLAGS="-L$PREFIX/lib -Wl,--gc-sections" \
		LIBS="-pthread -lrt -lical" \
		DBUS_CFLAGS="-I$PREFIX/lib/dbus-1.0/include -I$PREFIX/include/dbus-1.0" \
		DBUS_LIBS="-ldbus-1" \
		GLIB_CFLAGS="-I$PREFIX/lib/glib-2.0/include -I$PREFIX/include/glib-2.0" \
		GLIB_LIBS="-lglib-2.0" \
		ICAL_CFLAGS="-I$PREFIX/lib/libical" \
		ICAL_LIBS="-lical" \

