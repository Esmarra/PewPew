test: test.o
	gcc -Wall -D_REENTRANT test.o -o test

test.o: test.c
	gcc -Wall -c  test.c -o test.o
clean :
	rm test *.o
