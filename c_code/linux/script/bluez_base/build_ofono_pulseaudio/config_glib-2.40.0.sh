
glib_cv_stack_grows=no \
glib_cv_uscore=no \
ac_cv_func_posix_getpwuid_r=yes \
ac_cv_func_posix_getgrgid_r=yes \
ac_cv_path_GLIB_COMPILE_RESOURCES=yes \
./configure \
	--prefix=$PREFIX/usr \
	--host=$HOST \
	--sysconfdir=$PREFIX/etc \
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
	--disable-Bsymbolic \
	
make -j
make install
