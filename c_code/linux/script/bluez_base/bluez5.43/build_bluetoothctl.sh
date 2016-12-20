#!/bin/bash

export CC=mips-linux-gnu-gcc
export COMPILE_CROSS=mips-linux-gnu
export HOST=mips-linux-gnu
export PREFIX=/


DIR_CURRENT=`pwd`
COMPILE_FOLDER=$DIR_CURRENT/compileCode
OUTPUT_FOLDER=$DIR_CURRENT/output
SOURCE_CODE_FOLDER=$DIR_CURRENT/../../../sourceCode

BLUEZ_5_43_NAME="bluez-5.43.tar.xz"
EXPAT_2_1_0_NAME="expat-2.1.0.tar.gz"
DBUS_1_8_8_NAME="dbus-1.8.8.tar.gz"
ZLIB_1_2_8_NAME="zlib-1.2.8.tar.gz"
LIBFFI_3_1_NAME="libffi-3.1.tar.gz"
GLIB_2_40_0_NAME="glib-2.40.0.tar.xz"
TERMCAPE_NAME="termcap-1.3.1.tar.gz"
READLINE_NAME="readline-master.tar.gz"

BLUEZ_5_43_URL="http://www.kernel.org/pub/linux/bluetooth/bluez-5.43.tar.xz"

rm -rf $COMPILE_FOLDER $OUTPUT
mkdir -p $COMPILE_FOLDER $OUTPUT

if [ -f "$SOURCE_CODE_FOLDER/$BLUEZ_5_43_NAME" ]
then
	 echo "bluez5.43 package found"
else
	 echo "bluez5.43 package not found, download now"
	 wget -O $SOURCE_CODE_FOLDER/$BLUEZ_5_43_NAME $BLUEZ_5_43_URL
fi

if [ -f "$SOURCE_CODE_FOLDER/$BLUEZ_5_43_NAME" ]
then
#Extract files
	 echo "Extracting bluez-5.43.tar.xz"
	 tar xJf $SOURCE_CODE_FOLDER/$BLUEZ_5_43_NAME -C $COMPILE_FOLDER/
	 echo "Extracting expat-2.1.0.tar.gz"
	 tar xzf $SOURCE_CODE_FOLDER/$EXPAT_2_1_0_NAME -C $COMPILE_FOLDER/
	 echo "Extracting dbus-1.8.8.tar.gz"
	 tar xzf $SOURCE_CODE_FOLDER/$DBUS_1_8_8_NAME -C $COMPILE_FOLDER/
	 echo "Extracting zlib-1.2.8.tar.gz"
	 tar xzf $SOURCE_CODE_FOLDER/$ZLIB_1_2_8_NAME -C $COMPILE_FOLDER/
	 echo "Extracting libffi-3.1.tar.gz"
	 tar xzf $SOURCE_CODE_FOLDER/$LIBFFI_3_1_NAME -C $COMPILE_FOLDER/
	 echo "Extracting glib-2.40.0.tar.xz"
	 tar xJf $SOURCE_CODE_FOLDER/$GLIB_2_40_0_NAME -C $COMPILE_FOLDER/
	 echo "Extracting termcap-1.3.1.tar.gz"
	 tar xzf $SOURCE_CODE_FOLDER/$TERMCAPE_NAME -C $COMPILE_FOLDER/
	 echo "Extracting readline-master.tar.gz"
	 tar xzf $SOURCE_CODE_FOLDER/$READLINE_NAME -C $COMPILE_FOLDER/
#compile
	 cd $COMPILE_FOLDER/zlib-1.2.8/
		  ./configure
		  make -j
		  make install DESTDIR=$OUTPUT_FOLDER
	 cd $COMPILE_FOLDER/libffi-3.1/
		  ./configure \
			  --host=$HOST \
			  --prefix=/usr \
			  --sysconfdir=/etc \
			  --disable-debug
		  make -j
		  make install DESTDIR=$OUTPUT_FOLDER
	 cd $COMPILE_FOLDER/glib-2.40.0/
		  glib_cv_stack_grows=no \
		  glib_cv_uscore=no \
		  ac_cv_func_posix_getpwuid_r=yes \
		  ac_cv_func_posix_getgrgid_r=yes \
		  ac_cv_path_GLIB_COMPILE_RESOURCES=yes \
		  ./configure \
			   --prefix=/usr \
			   --host=$HOST \
			   --sysconfdir=/etc \
			   ZLIB_CFLAGS="-I$OUTPUT_FOLDER/usr/local/include" \
			   ZLIB_LIBS="-L$OUTPUT_FOLDER/usr/local/lib -lz" \
			   LIBFFI_CFLAGS="-I$OUTPUT_FOLDER/usr/include -I$OUTPUT_FOLDER/usr/lib/libffi-3.1/include/" \
			   LIBFFI_LIBS="-L$OUTPUT_FOLDER/usr/lib -lffi" \
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
		  make install DESTDIR=$OUTPUT_FOLDER
	 cd $COMPILE_FOLDER/expat-2.1.0/
		  ./configure \
			  --prefix=/usr \
			  --host=$HOST \
			  --enable-shared \
			  --disable-static
		  make -j
		  make install DESTDIR=$OUTPUT_FOLDER
	 cd $COMPILE_FOLDER/dbus-1.8.8/
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
			  CFLAGS="-I$OUTPUT_FOLDER/usr/include -fdata-sections -ffunction-sections" \
			  LDFLAGS="-L$OUTPUT_FOLDER/usr/lib -Wl,--gc-sections"
		  make -j
		  make install DESTDIR=$OUTPUT_FOLDER/
	 cd $COMPILE_FOLDER/termcap-1.3.1
		  ./configure --host=mips-linux-gnu
		  make -j
		  mkdir -p $OUTPUT_FOLDER/usr/lib/
		  cp libtermcap.a $OUTPUT_FOLDER/usr/lib/
	 cd $COMPILE_FOLDER/readline-master
		  ./configure --host=mips-linux-gnu --enable-shared --enable-static
		  make -j
		  make install DESTDIR=$OUTPUT_FOLDER

	 cd $COMPILE_FOLDER/bluez-5.43/
	 $CC   \
		 -I$OUTPUT_FOLDER/usr/local/include \
		 -I$OUTPUT_FOLDER/usr/include \
		 -I$OUTPUT_FOLDER/usr/include/glib-2.0 \
		 -I$OUTPUT_FOLDER/usr/include/dbus-1.0 \
		 -I$OUTPUT_FOLDER/usr/lib/glib-2.0/include \
		 -I$OUTPUT_FOLDER/usr/lib/dbus-1.0/include \
		 -L$OUTPUT_FOLDER/usr/lib \
		 -L$OUTPUT_FOLDER/usr/local/lib \
		 -I$COMPILE_FOLDER/bluez-5.43 \
		 -fdata-sections -ffunction-sections \
		 -DVERSION=5.43 \
		 -lz \
		 -lffi \
		 -lreadline \
		 -ltermcap \
		 -lglib-2.0 \
		 -ldbus-1 \
		 client/main.c \
		 client/gatt.c \
		 client/display.c \
		 client/advertising.c \
		 client/agent.c \
		 gdbus/client.c \
		 gdbus/watch.c \
		 gdbus/object.c \
		 gdbus/polkit.c \
		 gdbus/mainloop.c \
		 monitor/uuid.c \
		 -o bluetoothctl \

	 mkdir -p $OUTPUT_FOLDER/usr/bin/
	 cp bluetoothctl $OUTPUT_FOLDER/usr/bin/
	 echo "bluetoothctl compile finished."
	 cd $DIR_CURRENT
else
	 echo "bluez5.43 package download failed, exit now"
fi
