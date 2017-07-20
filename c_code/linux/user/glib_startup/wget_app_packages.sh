# The code packages are use for compile Glib 2.50.3

SOURCE_CODE_FOLDER=./app_packages
mkdir -p $SOURCE_CODE_FOLDER

ZLIB_1_2_11_NAME="zlib-1.2.11.tar.xz"
ZLIB_1_2_11_URL="https://zlib.net/zlib-1.2.11.tar.xz"

LIBFFI_3_2_1_NAME="libffi-3.2.1.tar.gz"
LIBFFI_3_2_1_URL="ftp://sourceware.org/pub/libffi/libffi-3.2.1.tar.gz"

GLIB_2_50_3_NAME="glib-2.50.3.tar.xz"
GLIB_2_50_3_URL="https://ftp.gnome.org/pub/gnome/sources/glib/2.50/glib-2.50.3.tar.xz"

CheckPackage () {
	if [ -f "$SOURCE_CODE_FOLDER/$1" ]
	then
		echo -e "\033[33;32m$1 package found\e[0m"
	else
		echo -e "\033[33;33m$1 package not found, download now\e[0m"
		wget -O $SOURCE_CODE_FOLDER/$1 $2
	fi
}

CheckPackage $ZLIB_1_2_11_NAME        $ZLIB_1_2_11_URL
CheckPackage $LIBFFI_3_2_1_NAME       $LIBFFI_3_2_1_URL
CheckPackage $GLIB_2_50_3_NAME        $GLIB_2_50_3_URL
