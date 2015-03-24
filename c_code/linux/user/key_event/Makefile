output = key_event 
$(output) : key_event.o
	mips-linux-gnu-gcc -o $(output) key_event.c --static
	mips-linux-gnu-strip $(output)
	scp92 $(output) adb_halley/$(output)
clean:
	rm -rf *.o $(output)
