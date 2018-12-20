all: ex_iterators.o
%.o: %.c
	gcc -c -I. $<
clean:
	rm *.o
