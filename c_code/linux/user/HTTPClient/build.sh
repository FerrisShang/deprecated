CC=gcc

$CC \
		HTTPClientSample.c \
		API/*.c \
		-I. \
		-IAPI \
		-o sample \
		-Wall \

