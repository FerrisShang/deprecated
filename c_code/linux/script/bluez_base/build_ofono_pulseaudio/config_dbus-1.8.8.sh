./configure \
		--host=$HOST \
		--prefix=$PREFIX/usr \
		--sysconfdir=$PREFIX/etc \
		--disable-developer \
		--disable-ansi \
		--disable-verbose-mode \
		--disable-asserts \
		--disable-checks \
		--disable-xml-docs \
		--disable-doxygen-docs \
		--enable-abstract-sockets \
		--disable-selinux \
		--disable-libaudit \
		--disable-inotify \
		--disable-kqueue \
		--disable-console-owner-file \
		--disable-launchd \
		--disable-systemd \
		--disable-embedded-tests \
		--disable-modular-tests \
		--disable-tests \
		--disable-installed-tests \
		--enable-epoll \
		--disable-x11-autolaunch \
		--disable-Werror \
		--enable-stats \
		--localstatedir=/var \
		--with-dbus-user=dbus \
		--with-system-socket=/var/run/dbus/system_bus_socket \
		--with-system-pid-file=/var/run/messagebus.pid \
		CFLAGS="-I$PREFIX/usr/include -fdata-sections -ffunction-sections" \
		LDFLAGS="-L$PREFIX/usr/lib -Wl,--gc-sections" \

make -j
make install
