./configure \
		--host=$HOST \
		--prefix=/usr \
		CFLAGS="-I$PREFIX/usr/include " \
		LDFLAGS="-L$PREFIX/usr/lib" \
		SNDFILE_CFLAGS="-I$PREFIX/usr/include" \
		SNDFILE_LIBS="-L$PREFIX/usr/lib -lsndfile" \

make -j
make install DESTDIR=$PREFIX
