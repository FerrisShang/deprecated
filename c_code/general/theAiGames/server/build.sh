gcc demo_client.c -pthread -o demo_client
gcc demo_server.c stdio_open.c -lpthread -o demo_server
