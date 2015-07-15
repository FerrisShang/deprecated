#CC=mips-linux-gnu-gcc
CC=gcc

$CC -Wall lib_so.c -fPIC -shared -o lib_so.so 
$CC -Wall main.c plugin.c -o main -ldl
