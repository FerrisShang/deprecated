#!/bin/bash
set -e
export HOST_CONFIG="--host=mips-linux-gnu"
export DIR_CURRENT=`pwd`
export SOURCE_DIR=$DIR_CURRENT/../sourceCode/
export OUTPUT_DIR=$DIR_CURRENT/output
export PREFIX=/usr

echo "Extract source code"
cd $SOURCE_DIR
rm -rf expat-2.1.0
rm -rf dbus-1.8.8
rm -rf $OUTPUT_DIR
tar xf expat-2.1.0.tar.gz
tar xf dbus-1.8.8.tar.gz

echo "Compile source code"
# expat
cd $SOURCE_DIR/expat-2.1.0
./configure \
		--prefix=$PREFIX \
		$HOST_CONFIG \
		--enable-shared \
		--disable-static \

make -j
make install DESTDIR=$OUTPUT_DIR

# dbus
cd $SOURCE_DIR/dbus-1.8.8
./configure --prefix=$PREFIX \
			 $HOST_CONFIG \
			 --disable-dependency-tracking \
			 --enable-shared --disable-static \
			 --with-dbus-user=dbus \
			 --disable-tests \
			 --disable-asserts \
			 --enable-abstract-sockets \
			 --disable-selinux \
			 --disable-xml-docs \
			 --disable-doxygen-docs \
			 --localstatedir=/var \
			 --with-system-socket=/var/run/dbus/system_bus_socket \
			 --with-system-pid-file=/var/run/messagebus.pid \
			 --without-x \
			 --disable-systemd \
			 CFLAGS="-I$OUTPUT_DIR$PREFIX/include" \
			 LDFLAGS="-L$OUTPUT_DIR$PREFIX/lib" \

make -j
make install DESTDIR=$OUTPUT_DIR

cd $DIR_CURRENT
#mkdir -p /tmp/dbus
#mkdir -p /var/lib/dbus
#dbus-uuidgen > /var/lib/dbus/machine-id
#dbus-daemon --config-file=/etc/dbus-1/system.conf
