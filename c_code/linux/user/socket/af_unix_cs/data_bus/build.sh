CFLAGS="-g -I.."
CC=gcc
LDFLAGS="-pthread -fsanitize=address -fsanitize=leak"
#CC=mips-linux-gnu-gcc
#LDFLAGS="-pthread"

SOURCE_CODE="data_bus.c ../af_client.c ../af_server.c queue.c main.c"

$CC $SOURCE_CODE $CFLAGS $LDFLAGS
