test: *.c *.h makefile
	gcc *.c -o test
	gcc *.c -o test -std=c11
	clang *.c -o test
	clang *.c -o test -std=c11
