.PHONY: clean
all: test_gcc test_gcc_c11 test_clang test_clang_c11 test_clang_cpp test_gcc_cpp
	./test_gcc 
	./test_gcc_c11 
	./test_clang 
	./test_clang_c11 
	./test_clang_cpp 
	./test_gcc_cpp
	echo pass!

test_gcc: test.c cfmt.h
	gcc test.c -o $@ -g

test_gcc_c11: test.c cfmt.h
	gcc test.c -o $@ -g -std=c11

test_clang: test.c cfmt.h
	clang test.c -o $@ -g

test_clang_c11: test.c cfmt.h
	clang test.c -o $@ -g -std=c11

test_clang_cpp: test.cc cfmt.h cfmt.c
	clang++ test.cc cfmt.c -o $@ -g

test_gcc_cpp: test.cc cfmt.h cfmt.c
	gcc cfmt.c -c -g
	g++ test.cc cfmt.o -o $@ -g

clean: 
	rm test_gcc test_gcc_c11 test_clang test_clang_c11 cfmt.o test_gcc_cpp test_clang_cpp

