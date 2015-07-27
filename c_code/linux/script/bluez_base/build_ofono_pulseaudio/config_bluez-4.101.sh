./configure \
		--prefix=/usr \
		--sysconfdir=/etc \
		--host=$HOST \
		CFLAGS="-I$PREFIX/usr/include -fdata-sections -ffunction-sections" \
		LDFLAGS="-L$PREFIX/usr/lib -Wl,--gc-sections" \
		DBUS_CFLAGS="-I$PREFIX/usr/lib/dbus-1.0/include -I$PREFIX/usr/include/dbus-1.0" \
		DBUS_LIBS="-L$PREFIX/usr/lib -ldbus-1 -lpthread -lrt" \
		GLIB_CFLAGS="-I$PREFIX/usr/lib/glib-2.0/include -I$PREFIX/usr/include/glib-2.0" \
		GLIB_LIBS="-L$PREFIX/usr/lib -lglib-2.0 -pthread -lrt" \
		--disable-dependency-tracking \
		--with-telephony=ofono \
		--disable-network \
		--enable-sap \
		--enable-serial \
		--enable-input \
		--enable-audio \
		--enable-service \
		--disable-health \
		--disable-pnat \
		--disable-gstreamer \
		--enable-alsa \
		--disable-usb \
		--disable-hidd \
		--disable-pand \
		--disable-dund \
		--disable-test \
		--enable-datafiles \
		--disable-debug \
		--disable-dbusoob \
		--disable-wiimote \
		--enable-gatt \

make -j
make install DESTDIR=$PREFIX

$HOST-gcc -I$PREFIX/usr/lib/dbus-1.0/include -I$PREFIX/usr/include/dbus-1.0 -L$PREFIX/usr/lib -ldbus-1 -lrt -lpthread -DHAVE_CONFIG_H -I. test/agent.c -o test/agent
cp test/agent $PREFIX/usr/bin
