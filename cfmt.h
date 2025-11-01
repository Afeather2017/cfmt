#pragma once
#ifndef CFMT_H
#define CFMT_H
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
#include <stdio.h>
void _cfmt_println(const char *fmt, int count, int types[], ...);
const char *_cfmt_format(const char *fmt, int count, int types[], ...);
void _cfmt_fprint(FILE *fp, const char *fmt, int count, int types[], ...);
void cfmt_internal_test(void);
#ifdef __cplusplus
}
#endif  // __cplusplus

#ifdef __cplusplus
// clang-format off
inline int to_type_id(const char                 arg) { return 0u; }
inline int to_type_id(const unsigned char        arg) { return 1u; }
inline int to_type_id(const short                arg) { return 2u; }
inline int to_type_id(const unsigned short       arg) { return 3u; }
inline int to_type_id(const int                  arg) { return 4u; }
inline int to_type_id(const unsigned int         arg) { return 5u; }
inline int to_type_id(const long                 arg) { return 6u; }
inline int to_type_id(const unsigned long        arg) { return 7u; }
inline int to_type_id(const long long            arg) { return 8u; }
inline int to_type_id(const unsigned long long   arg) { return 9u; }
inline int to_type_id(const float                arg) { return 10; }
inline int to_type_id(const double               arg) { return 11; }

inline int to_type_id(const char                *arg) { return 50; }
inline int to_type_id(const unsigned char       *arg) { return 51; }
inline int to_type_id(const short               *arg) { return 52; }
inline int to_type_id(const unsigned short      *arg) { return 53; }
inline int to_type_id(const int                 *arg) { return 54; }
inline int to_type_id(const unsigned int        *arg) { return 55; }
inline int to_type_id(const long                *arg) { return 56; }
inline int to_type_id(const unsigned long       *arg) { return 57; }
inline int to_type_id(const long long           *arg) { return 58; }
inline int to_type_id(const unsigned long long  *arg) { return 59; }
inline int to_type_id(const float               *arg) { return 50; }
inline int to_type_id(const double              *arg) { return 51; }
// clang-format on
template <typename... Args>
const char *cfmt_format_cpp(const char *fmt, int line_no, Args... args) {
  int types[] = {line_no, to_type_id(args)...};
  return _cfmt_format(fmt, sizeof(types) / sizeof(int) - 1, types, args...);
}
template <typename... Args>
void cfmt_fprint_cpp(FILE *fp, const char *fmt, int line_no, Args... args) {
  int types[] = {line_no, to_type_id(args)...};
  _cfmt_fprint(fp, fmt, sizeof(types) / sizeof(int) - 1, types, args...);
}
#define cfmt_println(fmt, ...) \
  cfmt_fprint_cpp(stdout, fmt "\n", __LINE__, ##__VA_ARGS__)
#define cfmt_print(fmt, ...) \
  cfmt_fprint_cpp(stdout, fmt, __LINE__, ##__VA_ARGS__)
#define cfmt_format(fmt, ...) cfmt_format_cpp(fmt, __LINE__, ##__VA_ARGS__)
#define cfmt_fprint(fp, fmt, ...) \
  cfmt_fprint_cpp(fp, fmt, __LINE__, ##__VA_ARGS__)
#else
#if defined(_MSC_VER) && !defined(__STDC_VERSION__)
#error "We need c11 and _Generic for MSVC! Please use '/std:c11'"
#endif

#define __COUNT_ARGS(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, N, \
                     ...)                                                      \
  N
#define EXPAND(...) __VA_ARGS__
#if defined(__STRICT_ANSI__)
#define COUNT_ARGS(...)                                                      \
  __COUNT_ARGS(0, ##__VA_ARGS__ __VA_OPT__(, ) 12, 11, 10, 9, 8, 7, 6, 5, 4, \
               3, 2, 1, 0, 2000000000)
#else
#define COUNT_ARGS(...)                                                    \
  __COUNT_ARGS(0, ##__VA_ARGS__, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, \
               2000000000)
#endif  // defined(_MSC_VER)

#define TYPE_ID(v)                    \
  _Generic((v),                       \
      char: 0,                        \
      unsigned char: 1,               \
      short: 2,                       \
      unsigned short: 3,              \
      int: 4,                         \
      unsigned int: 5,                \
      long: 6,                        \
      unsigned long: 7,               \
      long long: 8,                   \
      unsigned long long: 9,          \
      float: 10,                      \
      double: 11,                     \
                                      \
      const char *: 50,               \
      const unsigned char *: 51,      \
      const short *: 52,              \
      const unsigned short *: 53,     \
      const int *: 54,                \
      const unsigned int *: 55,       \
      const long *: 56,               \
      const unsigned long *: 57,      \
      const long long *: 58,          \
      const unsigned long long *: 59, \
      const float *: 60,              \
      const double *: 61,             \
      void *: 62,                     \
                                      \
      char *: 50,                     \
      unsigned char *: 51,            \
      short *: 52,                    \
      unsigned short *: 53,           \
      int *: 54,                      \
      unsigned int *: 55,             \
      long *: 56,                     \
      unsigned long *: 57,            \
      long long *: 58,                \
      unsigned long long *: 59,       \
      float *: 60,                    \
      double *: 61,                   \
      const void *: 62,               \
                                      \
      default: (int)2e9)

#define TYPE_ID_12(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_11(__VA_ARGS__))
#define TYPE_ID_11(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_10(__VA_ARGS__))
#define TYPE_ID_10(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_9(__VA_ARGS__))
#define TYPE_ID_9(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_8(__VA_ARGS__))
#define TYPE_ID_8(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_7(__VA_ARGS__))
#define TYPE_ID_7(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_6(__VA_ARGS__))
#define TYPE_ID_6(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_5(__VA_ARGS__))
#define TYPE_ID_5(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_4(__VA_ARGS__))
#define TYPE_ID_4(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_3(__VA_ARGS__))
#define TYPE_ID_3(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_2(__VA_ARGS__))
#define TYPE_ID_2(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_1(__VA_ARGS__))
#define TYPE_ID_1(v) EXPAND(TYPE_ID(v))
#define TYPE_ID_0()
#define TYPE_ID_2000000000()

#define MACRO_CONCAT(a, b) a##b
#define CONCAT(a, b) MACRO_CONCAT(a, b)

#define TYPE_ARRAY(COUNT, ...) \
  EXPAND((int[]){__LINE__, CONCAT(TYPE_ID_, COUNT)(__VA_ARGS__)})

#define __CFmtArgs(fmt, N, ...) \
  fmt, N, TYPE_ARRAY(N, __VA_ARGS__), ##__VA_ARGS__

#define CFmtArgs(fmt, ...) \
  __CFmtArgs(fmt, COUNT_ARGS(__VA_ARGS__), ##__VA_ARGS__)
#define cfmt_print(fmt, ...) _cfmt_fprint(stdout, CFmtArgs(fmt, ##__VA_ARGS__))
#define cfmt_println(fmt, ...) \
  _cfmt_fprint(stdout, CFmtArgs(fmt "\n", ##__VA_ARGS__))
#define cfmt_fprint(fp, fmt, ...) _cfmt_fprint(fp, CFmtArgs(fmt, ##__VA_ARGS__))
#define cfmt_format(fmt, ...) _cfmt_format(CFmtArgs(fmt, ##__VA_ARGS__))
#endif  // __cplusplus
#define CH(x) (#x[0])
#endif  // CFMT_H
