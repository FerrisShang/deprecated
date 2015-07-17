#用于配置dbus1.8.8
PREFIX="/home/fshang/f/halley_linux3.10/sources/network/apps/output/app/usr"
./configure \
		--host=mips-linux-gnu \
		--sysconfdir=$PREFIX/../etc \
		--prefix=$PREFIX \
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
		CFLAGS="-I$PREFIX/include" LDFLAGS="-L$PREFIX/lib -fdata-sections -ffunction-sections" \
		LDFLAGS="-L$PREFIX/lib -Wl,--gc-sections" \
