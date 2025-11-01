.PHONY: clean
FLAGS=-g -Wall
all: test_gcc test_gcc_c11 test_clang test_clang_c11 test_clang_cpp test_gcc_cpp
	./test_gcc 
	./test_gcc_c11 
	./test_clang 
	./test_clang_c11 
	./test_clang_cpp 
	./test_gcc_cpp
	echo pass!

test_gcc: test.c cfmt.h cfmt.c makefile
	gcc cfmt.c test.c -o $@ $(FLAGS)

test_gcc_c11: test.c cfmt.h cfmt.c makefile
	gcc test.c cfmt.c -o $@ -std=c11 $(FLAGS)

test_clang: test.c cfmt.h cfmt.c makefile
	clang test.c cfmt.c -o $@ $(FLAGS)

test_clang_c11: test.c cfmt.h cfmt.c makefile
	clang test.c cfmt.c -o $@ -std=c11 $(FLAGS)

test_clang_cpp: test.cc cfmt.h cfmt.c makefile
	clang cfmt.c -c $(FLAGS)
	clang++ cfmt.o test.cc -o $@ $(FLAGS)

test_gcc_cpp: test.cc cfmt.h cfmt.c makefile
	gcc cfmt.c -c $(FLAGS)
	g++ test.cc cfmt.o -o $@ $(FLAGS)

clean: 
	rm test_gcc test_gcc_c11 test_clang test_clang_c11 cfmt.o test_gcc_cpp test_clang_cpp

