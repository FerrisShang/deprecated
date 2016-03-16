#!/bin/bash

export DIR_CURRENT=`pwd`
export BT_DEPEND_DIR=$DIR_CURRENT/../bt_depend
export BLUEZ4_DIR=$DIR_CURRENT/../bluez
export OUTPUT_DIR=$DIR_CURRENT/tmp_output
export APP_DIR=$DIR_CURRENT/app_output
mkdir -p $OUTPUT_DIR
mkdir -p $APP_DIR

export CC=mips-linux-gnu-gcc
export CXX=mips-linux-gnu-g++
export AR=mips-linux-gnu-ar

export PREFIX=$OUTPUT_DIR
export HOST=mips-linux-gnu

function stamp_reset()
{
	$BT_DEPEND_DIR/stamp_reset.sh;
	if [ -f "Makefile" ] ; then
		make distclean;
	fi
}

# expat-2.1.0 
cd $BT_DEPEND_DIR/expat-2.1.0 
echo compiling $BT_DEPEND_DIR/expat-2.1.0
stamp_reset
./configure \
		--prefix=/usr \
		--host=$HOST \
		--enable-shared \
		--disable-static
make -j
make install DESTDIR=$PREFIX

# dbus-1.8.8
cd $BT_DEPEND_DIR/dbus-1.8.8
echo compiling $BT_DEPEND_DIR/dbus-1.8.8
stamp_reset
./configure \
		--host=$HOST \
		--prefix=/usr \
		--sysconfdir=/etc \
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
		CFLAGS="-I$PREFIX/usr/include" \
		LDFLAGS="-L$PREFIX/usr/lib"
make -j
make install DESTDIR=$PREFIX

# libffi-3.1
cd $BT_DEPEND_DIR/libffi-3.1
echo compiling $BT_DEPEND_DIR/libffi-3.1
stamp_reset
./configure \
		--host=$HOST \
		--prefix=/usr \
		--sysconfdir=/etc \
		--disable-debug
make -j
make install DESTDIR=$PREFIX

# zlib-1.2.8
cd $BT_DEPEND_DIR/zlib-1.2.8
echo compiling $BT_DEPEND_DIR/zlib-1.2.8
stamp_reset
./configure \
		--prefix=/usr
make -j
make install DESTDIR=$PREFIX

# glib-2.40.0
cd $BT_DEPEND_DIR/glib-2.40.0
echo compiling $BT_DEPEND_DIR/glib-2.40.0
stamp_reset
glib_cv_stack_grows=no \
glib_cv_uscore=no \
ac_cv_func_posix_getpwuid_r=yes \
ac_cv_func_posix_getgrgid_r=yes \
ac_cv_path_GLIB_COMPILE_RESOURCES=yes \
./configure \
	--host=$HOST \
	--prefix=/usr \
	--sysconfdir=/etc \
	ZLIB_CFLAGS="-I$PREFIX/usr/include" \
	ZLIB_LIBS="-L$PREFIX/usr/lib -lz" \
	LIBFFI_CFLAGS="-I$PREFIX/usr/include -I$PREFIX/usr/lib/libffi-3.1/include/" \
	LIBFFI_LIBS="-L$PREFIX/usr/lib -lffi" \
	--disable-dependency-tracking \
	--disable-debug \
	--disable-mem-pools \
	--disable-installed-tests \
	--disable-always-build-tests \
	--disable-largefile \
	--enable-static \
	--enable-shared \
	--enable-included-printf \
	--disable-selinux \
	--disable-fam \
	--disable-xattr \
	--disable-libelf \
	--disable-gtk-doc \
	--disable-gtk-doc-html \
	--disable-gtk-doc-pdf \
	--disable-man \
	--disable-dtrace \
	--disable-systemtap \
	--disable-coverage \
	--disable-Bsymbolic
make -j
make install DESTDIR=$PREFIX

# bluez-4.101
cd $BLUEZ4_DIR/bluez-4.101
echo compiling $BLUEZ4_DIR/bluez-4.101
stamp_reset
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
		--enable-static \
		--enable-pcmcia \
		--enable-sap \
		--enable-serial \
		--enable-input \
		--enable-audio \
		--enable-tool \
		--enable-test \
		--enable-service \
		--disable-alsa \
		--disable-usb \
		--disable-test \
		--enable-datafiles \
		--disable-debug \
		--disable-dbusoob \
		--disable-gatt
make -j
make install DESTDIR=$PREFIX

# alsa-lib-1.0.28
cd $BT_DEPEND_DIR/alsa-lib-1.0.28
echo compiling $BT_DEPEND_DIR/alsa-lib-1.0.28
stamp_reset
./configure \
		--host=$HOST \
		--prefix=/usr \
		--sysconfdir=/etc
make -j
make install DESTDIR=$PREFIX

# libsndfile-1.0.25
cd $BT_DEPEND_DIR/libsndfile-1.0.25
echo compiling $BT_DEPEND_DIR/libsndfile-1.0.25
stamp_reset
./configure \
		--host=$HOST \
		--prefix=/usr
make -j
make install DESTDIR=$PREFIX

# json-c-json-c-0.11-20130402
cd $BT_DEPEND_DIR/json-c-json-c-0.11-20130402
echo compiling $BT_DEPEND_DIR/json-c-json-c-0.11-20130402
stamp_reset
./autogen.sh
sed -i '/rpl_/d' configure
./configure \
		--host=$HOST \
		--prefix=/usr \
		--disable-oldname-compat
make -j
make install DESTDIR=$PREFIX

# libtool-2.4
cd $BT_DEPEND_DIR/libtool-2.4
echo compiling $BT_DEPEND_DIR/libtool-2.4
stamp_reset
./configure \
		--host=$HOST \
		--prefix=/usr
make -j
make install DESTDIR=$PREFIX

# sbc-1.3
cd $BT_DEPEND_DIR/sbc-1.3
echo compiling $BT_DEPEND_DIR/sbc-1.3
stamp_reset
./configure \
		--host=$HOST \
		--prefix=/usr \
		CFLAGS="-I$PREFIX/usr/include " \
		LDFLAGS="-L$PREFIX/usr/lib" \
		SNDFILE_CFLAGS="-I$PREFIX/usr/include" \
		SNDFILE_LIBS="-L$PREFIX/usr/lib -lsndfile"
make -j
make install DESTDIR=$PREFIX

# pulseaudio-5.0
cd $DIR_CURRENT/pulseaudio-5.0
echo compiling $DIR_CURRENT/pulseaudio-5.0
stamp_reset
./configure \
	--host=$HOST \
	--prefix=/usr \
	--sysconfdir=/etc \
	--enable-alsa \
	--enable-dbus \
	--enable-bluez4 \
	--enable-static --enable-shared \
	--disable-legacy-runtime-dir \
	--disable-legacy-database-entry-format \
	--enable-dbus \
	--disable-gtk2 \
	--enable-hal-compat \
	--disable-openssl \
	--disable-bluez5 \
	--disable-dependency-trackinga \
	--disable-nls \
	--disable-rpath          \
	--disable-neon-opt       \
	--disable-largefile      \
	--disable-x11            \
	--disable-tests          \
	--disable-samplerate     \
	--disable-oss-output     \
	--disable-oss-wrapper    \
	--disable-coreaudio-output \
	--disable-esound         \
	--disable-solaris        \
	--disable-waveout        \
	--disable-glib2          \
	--disable-gtk3           \
	--disable-gconf          \
	--disable-avahi          \
	--disable-jack           \
	--disable-asyncns        \
	--disable-tcpwrap        \
	--disable-lirc           \
	--disable-bluez5         \
	--disable-udev           \
	--disable-ipv6           \
	--disable-openssl        \
	--disable-xen            \
	--disable-orc            \
	--disable-systemd        \
	--disable-systemd-journal \
	--disable-manpages       \
	--disable-per-user-esound-socket  \
	--disable-mac-universal  \
	--disable-default-build-tests \
	--disable-legacy-database-entry-format \
	--without-caps     \
	--with-database=auto \
	--without-fftw    \
	--without-speex  \
	CFLAGS="-I$PREFIX/usr/include " \
	LDFLAGS="-L$PREFIX/usr/lib -ldbus-1 -ljson-c -lsndfile -lltdl -ldl" \
	DBUS_CFLAGS="-I$PREFIX/usr/lib/dbus-1.0/include -I$PREFIX/usr/include/dbus-1.0" \
	DBUS_LIBS="-L$PREFIX/usr/lib -ldbus-1 -lpthread -lrt" \
	LIBJSON_CFLAGS="-I$PREFIX/usr/include -I$PREFIX/usr/include/json-c" \
	LIBJSON_LIBS="-L$PREFIX/usr/lib -ljson-c" \
	LIBSNDFILE_CFLAGS="-I$PREFIX/usr/include" \
	LIBSNDFILE_LIBS="-L$PREFIX/usr/lib -lsndfile" \
	SBC_CFLAGS="-I$PREFIX/usr/include" \
	SBC_LIBS="-L$PREFIX/usr/lib -lsbc" \
	ASOUNDLIB_CFLAGS="-I$PREFIX/usr/include -I$PREFIX/usr/include/alsa" \
	ASOUNDLIB_LIBS="-L$PREFIX/usr/lib -lasound"
make -j
make install DESTDIR=$PREFIX

cd $DIR_CURRENT

mkdir -p $APP_DIR/usr/bin/
mkdir -p $APP_DIR/usr/lib/pulse-5.0/modules
cp $OUTPUT_DIR/usr/bin/pulseaudio \
	   $APP_DIR/usr/bin/pulseaudio
cp $OUTPUT_DIR/usr/lib/libjson-c.so.2 \
	   $APP_DIR/usr/lib/libjson-c.so.2
cp $OUTPUT_DIR/usr/lib/libjson-c.so.2.0.1 \
	   $APP_DIR/usr/lib/libjson-c.so.2.0.1
cp $OUTPUT_DIR/usr/lib/libsndfile.so.1 \
	   $APP_DIR/usr/lib/libsndfile.so.1
cp $OUTPUT_DIR/usr/lib/libsndfile.so.1.0.25 \
	   $APP_DIR/usr/lib/libsndfile.so.1.0.25
cp $OUTPUT_DIR/usr/lib/libpulsecore-5.0.so \
	   $APP_DIR/usr/lib/libpulsecore-5.0.so
cp $OUTPUT_DIR/usr/lib/libpulse.so.0 \
	   $APP_DIR/usr/lib/libpulse.so.0
cp $OUTPUT_DIR/usr/lib/libpulse.so.0.17.3 \
	   $APP_DIR/usr/lib/libpulse.so.0.17.3
cp $OUTPUT_DIR/usr/lib/libltdl.so.7 \
	   $APP_DIR/usr/lib/libltdl.so.7
cp $OUTPUT_DIR/usr/lib/libltdl.so.7.3.0 \
	   $APP_DIR/usr/lib/libltdl.so.7.3.0
cp $OUTPUT_DIR/usr/lib/libasound.so.2 \
	   $APP_DIR/usr/lib/libasound.so.2
cp $OUTPUT_DIR/usr/lib/libasound.so.2.0.0 \
	   $APP_DIR/usr/lib/libasound.so.2.0.0
cp $OUTPUT_DIR/usr/lib/libsbc.so.1 \
	   $APP_DIR/usr/lib/libsbc.so.1
cp $OUTPUT_DIR/usr/lib/libsbc.so.1.2.1 \
	   $APP_DIR/usr/lib/libsbc.so.1.2.1
cp $OUTPUT_DIR/usr/lib/pulseaudio/libpulsecommon-5.0.so \
	   $APP_DIR/usr/lib/pulseaudio/libpulsecommon-5.0.so
cp $OUTPUT_DIR/usr/lib/pulse-5.0/modules/module-alsa-sink.so \
	   $APP_DIR/usr/lib/pulse-5.0/modules/module-alsa-sink.so
cp $OUTPUT_DIR/usr/lib/pulse-5.0/modules/module-bluetooth-policy.so \
	   $APP_DIR/usr/lib/pulse-5.0/modules/module-bluetooth-policy.so
cp $OUTPUT_DIR/usr/lib/pulse-5.0/modules/module-bluetooth-discover.so \
	   $APP_DIR/usr/lib/pulse-5.0/modules/module-bluetooth-discover.so
cp $OUTPUT_DIR/usr/lib/pulse-5.0/modules/module-bluez4-discover.so \
	   $APP_DIR/usr/lib/pulse-5.0/modules/module-bluez4-discover.so
cp $OUTPUT_DIR/usr/lib/pulse-5.0/modules/module-bluez4-device.so \
	   $APP_DIR/usr/lib/pulse-5.0/modules/module-bluez4-device.so
cp $OUTPUT_DIR/usr/lib/pulse-5.0/modules/libbluez4-util.so \
	   $APP_DIR/usr/lib/pulse-5.0/modules/libbluez4-util.so
cp $OUTPUT_DIR/usr/lib/pulse-5.0/modules/libalsa-util.so \
	   $APP_DIR/usr/lib/pulse-5.0/modules/libalsa-util.so
cp $OUTPUT_DIR/usr/lib/pulse-5.0/modules/module-loopback.so \
	   $APP_DIR/usr/lib/pulse-5.0/modules/module-loopback.so
