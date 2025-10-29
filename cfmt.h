#pragma once
#ifndef CFMT_H
#define CFMT_H
#define __COUNT_ARGS(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, N, ...) N
#define EXPAND(...) __VA_ARGS__
#define COUNT_ARGS(...) __COUNT_ARGS(0, ##__VA_ARGS__ __VA_OPT__(,) 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 2000000000)

#if defined(_MSC_VER) && !defined(__STDC_VERSION__)
#error "We need c11 and _Generic for MSVC!"
#endif

#define TYPE_ID(v) _Generic((v), \
  char: 0, unsigned char: 1,\
  short: 2, unsigned short: 3,\
  int: 4, unsigned int: 5,\
  long: 6, unsigned long: 7,\
  long long: 8, unsigned long long: 9,\
  float: 10, double: 11,\
  \
  char*: 50, unsigned char *: 51,\
  short*: 52, unsigned short*: 53,\
  int*: 54, unsigned int*: 55,\
  long*: 56, unsigned long*: 57,\
  long long*: 58, unsigned long long*: 59,\
  float*: 60, double*: 61,\
  \
  default: (int)2e9)

#define TYPE_ID_12(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_11(__VA_ARGS__))
#define TYPE_ID_11(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_10(__VA_ARGS__))
#define TYPE_ID_10(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_9(__VA_ARGS__))
#define TYPE_ID_9(v, ...)  EXPAND(TYPE_ID(v), TYPE_ID_8(__VA_ARGS__))
#define TYPE_ID_8(v, ...)  EXPAND(TYPE_ID(v), TYPE_ID_7(__VA_ARGS__))
#define TYPE_ID_7(v, ...)  EXPAND(TYPE_ID(v), TYPE_ID_6(__VA_ARGS__))
#define TYPE_ID_6(v, ...)  EXPAND(TYPE_ID(v), TYPE_ID_5(__VA_ARGS__))
#define TYPE_ID_5(v, ...)  EXPAND(TYPE_ID(v), TYPE_ID_4(__VA_ARGS__))
#define TYPE_ID_4(v, ...)  EXPAND(TYPE_ID(v), TYPE_ID_3(__VA_ARGS__))
#define TYPE_ID_3(v, ...)  EXPAND(TYPE_ID(v), TYPE_ID_2(__VA_ARGS__))
#define TYPE_ID_2(v, ...)  EXPAND(TYPE_ID(v), TYPE_ID_1(__VA_ARGS__))
#define TYPE_ID_1(v)       EXPAND(TYPE_ID(v))
#define TYPE_ID_0()
#define TYPE_ID_2000000000()

#define MACRO_CONCAT(a, b) a ## b
#define CONCAT(a, b) MACRO_CONCAT(a, b)

#define TYPE_ARRAY(COUNT, ...) EXPAND((int []){-1, CONCAT(TYPE_ID_,\
                                               COUNT)(__VA_ARGS__)})

#define __CFmtArgs(fmt, N, ...) fmt, \
                                N,\
                                TYPE_ARRAY(N, __VA_ARGS__), ##__VA_ARGS__

#define CFmtArgs(fmt, ...) __CFmtArgs(fmt,\
                                      COUNT_ARGS(__VA_ARGS__), ##__VA_ARGS__)
#include <stdio.h>
void _cfmt_println(const char *fmt, int count, int types[], ...);
const char *_cfmt_format(const char *fmt, int count, int types[], ...);
void _cfmt_fprint(FILE *fp, const char *fmt, int count, int types[], ...);
#define cfmt_print(fmt, ...) _cfmt_fprint(stdout,\
                                          CFmtArgs(fmt\
                                            , ##__VA_ARGS__))
#define cfmt_println(fmt, ...) _cfmt_fprint(stdout,\
                                            CFmtArgs(fmt "\n"\
                                              , ##__VA_ARGS__))
#define cfmt_fprint(fp, fmt, ...) _cfmt_fprint(fp,\
                                               CFmtArgs(fmt\
                                                 , ##__VA_ARGS__))
#define cfmt_format(fmt, ...) _cfmt_format(CFmtArgs(fmt\
                                            , ##__VA_ARGS__))
#define CH(x) (#x [0])
#endif // CFMT_H
