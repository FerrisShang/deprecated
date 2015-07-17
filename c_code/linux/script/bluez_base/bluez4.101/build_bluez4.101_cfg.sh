PREFIX="/home/fshang/f/halley_linux3.10/sources/network/apps/output/app/usr"
./configure \
		--program-prefix=/usr \
		--host=mips-linux-gnu \
		--enable-static \
		--enable-shared \
		--disable-pie \
		--disable-network \
		--disable-sap \
		--disable-serial \
		--disable-input \
		--disable-audio \
		--enable-service \
		--enable-health \
		--enable-pnat \
		--enable-gstreamer \
		--disable-alsa \
		--disable-usb \
		--enable-tools \
		--disable-bccmd \
		--disable-pcmcia \
		--disable-hid2hci \
		--disable-dfutool \
		--disable-hidd \
		--disable-pand \
		--disable-dund \
		--disable-cups \
		--enable-test \
		--disable-datafiles \
		--disable-debug \
		--disable-maemo6 \
		--disable-dbusoob \
		--disable-wiimote \
		--disable-hal \
		--enable-gatt \
		CFLAGS="-I$PREFIX/include -fdata-sections -ffunction-sections" \
		LDFLAGS="-L$PREFIX/lib -Wl,--gc-sections" \
		LIBS="-pthread -lrt" \
		DBUS_CFLAGS="-I$PREFIX/lib/dbus-1.0/include -I$PREFIX/include/dbus-1.0" \
		DBUS_LIBS="-ldbus-1" \
		GLIB_CFLAGS="-I$PREFIX/lib/glib-2.0/include -I$PREFIX/include/glib-2.0" \
		GLIB_LIBS="-lglib-2.0 " \

