./configure \
		--host=$HOST \
		--prefix=$PREFIX/usr \
		CFLAGS="-Wno-unused-but-set-variable" \

sed -i 's/#define malloc rpl_malloc//g'   config.h
sed -i 's/#define realloc rpl_realloc//g' config.h

make -j
make install
