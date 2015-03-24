output = test_mic_hp
$(output) : test_mic_hp.o
	  mips-linux-gnu-gcc -o $(output) test_mic_hp.c --static
clean:
	  rm -rf *.o $(output)
