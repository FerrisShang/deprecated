output = get_hp_state 
$(output) : get_hp_state.o
	  mips-linux-gnu-gcc -o $(output) get_hp_state.c --static
clean:
	  rm -rf *.o $(output)
