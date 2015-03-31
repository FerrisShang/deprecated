cc = mips-linux-gnu-
ALL:
	$(cc)gcc socket.c -o socket
	$(cc)strip socket
#ad push socket /sbin
	scp2adb socket
clean:
	rm -f test socket
