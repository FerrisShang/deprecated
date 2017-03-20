CC=gcc
JSON_DIR=./../../sourceCode/
LIBJSON_PAG_NAME="json-c-0.11-20130402.tar.gz"
JSON_FOLDER_NAME=json-c-json-c-0.11-20130402/

if [ -f "$JSON_DIR/$LIBJSON_PAG_NAME" ]
then
tar xzf $JSON_DIR/$LIBJSON_PAG_NAME -C .
	 touch $JSON_FOLDER_NAME/config.h
	 touch $JSON_FOLDER_NAME/json_config.h
	 $CC \
		 ./main.c \
		 $JSON_FOLDER_NAME/*.c \
					   -I$JSON_FOLDER_NAME \
					   -DHAVE_STRINGS_H \
					   -DSTDC_HEADERS \
					   -DHAVE_STRDUP \
					   -DJSON_C_HAVE_INTTYPES_H \
					   -DHAVE_STRNCASECMP \
					   -DHAVE_SNPRINTF \
					   -DHAVE_SYS_TYPES_H \
					   -DHAVE_SYS_STAT_H \
					   -DHAVE_FCNTL_H \
					   -DHAVE_UNISTD_H \
					   -DHAVE_OPEN \
					   -DHAVE_STDARG_H \
					   -DHAVE_VSNPRINTF \

else
	 echo -e "\033[33;33m Json package not found\e[0m"
	 echo -e " Running wget_bluez5_43_packages.sh first"
fi
