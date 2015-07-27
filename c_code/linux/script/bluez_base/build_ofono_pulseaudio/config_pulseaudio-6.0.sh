./configure \
		--host=$HOST \
		--prefix=/usr \
		--sysconfdir=/etc \
		--disable-largefile \
		--disable-x11 \
		--disable-tests \
		--disable-gtk3 \
		--disable-gconf \
		--disable-avahi \
		--disable-jack \
		--disable-asyncns \
		--disable-tcpwrap \
		--disable-lirc \
		--disable-bluez5 \
		--disable-ipv6 \
		--disable-openssl \
		--disable-xen \
		--without-caps \
		CFLAGS="-I$PREFIX/usr/include " \
		LDFLAGS="-L$PREFIX/usr/lib -ldbus-1 -ljson-c -lsndfile " \
		DBUS_CFLAGS="-I$PREFIX/usr/lib/dbus-1.0/include -I$PREFIX/usr/include/dbus-1.0" \
		DBUS_LIBS="-L$PREFIX/usr/lib -ldbus-1 -lpthread -lrt" \
		BLUEZ_CFLAGS="-I$PREFIX/usr/include/bluetooth" \
		BLUEZ_LIBS="-L$PREFIX/usr/lib -lbluetooth" \
		LIBJSON_CFLAGS="-I$PREFIX/usr/include/json-c" \
		LIBJSON_LIBS="-L$PREFIX/usr/lib -ljson-c" \
		LIBSNDFILE_CFLAGS="-I$PREFIX/usr/include" \
		LIBSNDFILE_LIBS="-L$PREFIX/usr/lib -lsndfile" \
		SBC_CFLAGS="-I$PREFIX/usr/include" \
		SBC_LIBS="-L$PREFIX/usr/lib -lsbc" \

make -j
make install DESTDIR=$PREFIX
