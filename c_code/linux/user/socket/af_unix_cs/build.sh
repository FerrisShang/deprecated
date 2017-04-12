CC=gcc
CFLAGS="-g"
LDFLAGS="-pthread -fsanitize=address -fsanitize=leak"

SERVER_CODE="server_test.c af_server.c"
CLIENT_CODE="client_test.c af_client.c"

$CC $SERVER_CODE $CFLAGS $LDFLAGS -o server
$CC $CLIENT_CODE $CFLAGS $LDFLAGS -o client
