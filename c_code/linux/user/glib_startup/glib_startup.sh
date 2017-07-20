#!/bin/bash

export HOST=mips-linux-gnu
export CC=mips-linux-gnu-gcc
#export HOST=
#export CC=gcc
export PREFIX=/

set -e

DIR_CURRENT=`pwd`
COMPILE_FOLDER=$DIR_CURRENT/compileCode
OUTPUT_FOLDER=$DIR_CURRENT/output
SOURCE_CODE_FOLDER=$DIR_CURRENT/app_packages

ZLIB_1_2_11_NAME="zlib-1.2.11.tar.xz"
LIBFFI_3_2_1_NAME="libffi-3.2.1.tar.gz"
GLIB_2_50_3_NAME="glib-2.50.3.tar.xz"

./wget_app_packages.sh
rm -rf $COMPILE_FOLDER $OUTPUT_FOLDER
mkdir -p $COMPILE_FOLDER $OUTPUT_FOLDER

#Extract files
	 echo "Extracting $ZLIB_1_2_11_NAME"
	 tar xJf $SOURCE_CODE_FOLDER/$ZLIB_1_2_11_NAME -C $COMPILE_FOLDER/
	 echo "Extracting $LIBFFI_3_2_1_NAME"
	 tar xf $SOURCE_CODE_FOLDER/$LIBFFI_3_2_1_NAME -C $COMPILE_FOLDER/
	 echo "Extracting $GLIB_2_50_3_NAME"
	 tar xJf $SOURCE_CODE_FOLDER/$GLIB_2_50_3_NAME -C $COMPILE_FOLDER/

#compile
	 cd $COMPILE_FOLDER/zlib-1.2.11/
	 echo -e "\033[33;32mbuilding `pwd`\e[0m"
		  ./configure --static --prefix=/
		  make -j
		  make install DESTDIR=$OUTPUT_FOLDER
	 cd $COMPILE_FOLDER/libffi-3.2.1/
	 echo -e "\033[33;32mbuilding `pwd`\e[0m"
		  ./configure          \
			  --host=$HOST     \
			  --prefix=/       \
			  --disable-shared \
			  --enable-static  \
			  --with-sysroot="$OUTPUT_FOLDER" \
			  CFLAGS="-Wall -O2 -fdata-sections -ffunction-sections"
		  make -j
		  make install DESTDIR=$OUTPUT_FOLDER
	 cd $COMPILE_FOLDER/glib-2.50.3/
	 echo -e "\033[33;32mbuilding `pwd`\e[0m"
		  glib_cv_stack_grows=no      \
		  glib_cv_uscore=no           \
		  ./configure                 \
			   --prefix=/             \
			   --host=$HOST           \
			   --enable-static        \
			   --disable-shared       \
			   --disable-selinux      \
			   --disable-fam          \
			   --disable-xattr        \
			   --disable-libelf       \
			   --disable-libmount     \
			   --disable-gtk-doc      \
			   --disable-gtk-doc-html \
			   --disable-gtk-doc-pdf  \
			   --disable-man          \
			   --disable-dtrace       \
			   --disable-systemtap    \
			   --disable-coverage     \
			   --disable-Bsymbolic    \
			   --disable-znodelete    \
			   --without-pcre         \
			   --disable-installed-tests \
			   --disable-always-build-tests \
			   --with-sysroot=$OUTPUT_FOLDER \
			   LT_SYS_LIBRARY_PATH=$OUTPUT_FOLDER \
			   ZLIB_CFLAGS="-I$OUTPUT_FOLDER/include" \
			   ZLIB_LIBS="-L$OUTPUT_FOLDER/lib/ -lz" \
			   LIBFFI_CFLAGS="-I$OUTPUT_FOLDER/lib/libffi-3.2.1/include/" \
			   LIBFFI_LIBS="-L$OUTPUT_FOLDER/lib/ -lffi" \
			   CFLAGS="-Wall -O2 -fdata-sections -ffunction-sections"
		  make -j
		  make install DESTDIR=$OUTPUT_FOLDER
