./configure \
		--prefix=/usr \
		--host=$HOST \
		--enable-shared \
		--disable-static \

make
make install DESTDIR=$PREFIX
