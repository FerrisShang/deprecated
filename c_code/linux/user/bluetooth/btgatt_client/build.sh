echo "building btgatt-client"
CC=mips-linux-gnu-gcc
SOURCE_FOLDER_GATTC=.
SOURCES_GATTC="\
$SOURCE_FOLDER_GATTC/tools/btgatt-client.c \
	   $SOURCE_FOLDER_GATTC/tools/mgmt.c \
	   $SOURCE_FOLDER_GATTC/src/ble_profile_uuid.c \
	   $SOURCE_FOLDER_GATTC/src/shared/gatt-client.c \
	   $SOURCE_FOLDER_GATTC/src/shared/gatt-helpers.c \
	   $SOURCE_FOLDER_GATTC/src/shared/mainloop.c \
	   $SOURCE_FOLDER_GATTC/src/shared/gatt-db.c \
	   $SOURCE_FOLDER_GATTC/src/shared/gatt-server.c \
	   $SOURCE_FOLDER_GATTC/src/shared/att.c \
	   $SOURCE_FOLDER_GATTC/src/shared/queue.c \
	   $SOURCE_FOLDER_GATTC/src/shared/util.c \
	   $SOURCE_FOLDER_GATTC/src/shared/crypto.c \
	   $SOURCE_FOLDER_GATTC/src/shared/io-mainloop.c \
	   $SOURCE_FOLDER_GATTC/src/shared/timeout-mainloop.c \
	   $SOURCE_FOLDER_GATTC/lib/bluetooth.c \
	   $SOURCE_FOLDER_GATTC/lib/uuid.c \
	   $SOURCE_FOLDER_GATTC/lib/hci.c \
	   "
CFLAGS_GATTC="-I$SOURCE_FOLDER_GATTC -I$SOURCE_FOLDER_GATTC/lib"
$CC $SOURCES_GATTC $CFLAGS_GATTC -o ./btgatt-client -Wall
