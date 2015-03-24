mips-linux-gnu-gcc -D_LINUX -g -Wall -march=mips32r2 -Wa,-mmxu -lm -lpthread -ldl -lrt main.c -Llibs  -lshootimage -lcjpeg -o shootimg #--static
mips-linux-gnu-strip shootimg
scp shootimg user@192.168.3.92:~/fshang/adb_halley/
