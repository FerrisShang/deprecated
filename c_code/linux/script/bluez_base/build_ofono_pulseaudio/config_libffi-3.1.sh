./configure \
		--host=$HOST \
		--prefix=$PREFIX/usr \
		--sysconfdir=$PREFIX/etc \
		--disable-debug \
		
make -j
make install
