# CFMT

fmt implemented in C.

See, you can print things like this in C/C++:

```c
cfmt_println("{} {}", "This is", "cfmt");
cfmt_println("{} {} {}", 0.47, (short)10, (long long)100);
```

Or better formated:

```c
cfmt_println("{:10} {:10}", "This is", "cfmt");
cfmt_println("{:10.1e} {:10x} {:10}", 0.47, (short)10, (long long)100);
```

## Motivation

I'v been suffered from "%x", "%lx", "%llx" for a really long time. And I decided to end th
is.

Say you have a `struct A { int val; }`, and originally, you use `printf("%d", a.val);`. And you changed `int val` to `long long val`, now you have to change all "%d" to "%lld". It's annoying.

I do hate std::cout. The performance, and the way format output.

```c++
float a = 123.777;
// If you want a formated output, you have to put more function in cout.
std::cout << setprecision(5) << a;
```

## How to use this?(Usage)

### Before use cfmt

Download cfmt, try to use your compiler to compile test.c:

```
gcc test.c cfmt.c -o test FLAGS_OF_YOUR_PROJECT
./test
```

Or MSVC:

```
cl test.c cfmt.c /std:c11 FLAGS_OF_YOUR_PROJECT
.\test.exe
```

If it is compiled successfully, and runs without any error, then it is OK to use cfmt.

### Compile

Take cfmt.c and cfmt.h to your project, and include cfmt.h, compile cfmt.c. e.g.:

```shell
gcc cfmt.c -c
g++ cfmt.o test.cc -o test
```

```shell
gcc test.c -o test
```

If it is MSVC:

```bat
cl /std:c11 test.c cfmt.c
```

Or MSVC C++:

```
cl cfmt.c /c /std:c11
cl cfmt.o test.cc
```

### How to format print a struct?

Well, this need more work. Anyway, currently, this is not supported. But you can try this:

```c
const char *convert_to_string(struct A *a) {
  // Here you `malloc`ed, and convert `a` to string, then return the string
}
int main() {
  struct A a;
  const char *str = convert_to_string(&a);
  cfmt_println("{}", str);
  free(str);
}
```

And in C++, you can print std::string like this:

```c
cfmt_println("{}", str.c_str());
```

cfmt is a wrapper of printf, so it cannot do what printf cannot do.

### Wrap cfmt?

cfmt is macros. You can only use macro to wrap it.

```c
#define Log(fmt, ...) cfmt_fprint(log_file, "Log {}:{}:" fmt "\n", __func__, __LINE__, ##_
_VA_ARGS__)
```

## API references

```c
void cfmt_fprint(FILE *fp, const char *fmt, ...)
void cfmt_println(const char *fmt, ...)
void cfmt_print(const char *fmt, ...)
const char *cfmt_format(const char *fmt, ...)
```

You DO NOT own the string returned by `cfmt_format`, DON'T `free()` the ptr!

```c
const char *str = cfmt_format("{}", 12);
// This might crashes
free(str);
```

arg `fmt`: [format string](https://man.archlinux.org/man/printf.3#Format_of_the_format_str
ing)

## More Example

See test.c and test.cc
