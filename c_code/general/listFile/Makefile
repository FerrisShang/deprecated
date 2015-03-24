output = listFiles

$(output) : list_file.o strHandle.o main.o 
	 gcc -o $(output) main.c list_file.c strHandle.c
#list_file.o : list_file.h
#strHandle.o : strHandle.h
#main.o : main.c

clean:
	 @rm -rf *.o a.out $(output)
