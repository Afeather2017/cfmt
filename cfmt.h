#pragma once
#ifndef CFMT_H
#define CFMT_H
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
#include <stdarg.h>
#include <stdio.h>
struct string_buffer {
  int size;
  int capacity;
  char *data;
};
enum cfmt_typeid {
  kUnknow,
  kChar,
  kUChar,
  kShort,
  kUShort,
  kInt,
  kUInt,
  kLong,
  kULong,
  kLLong,
  kULLong,
  kFloat,
  kDouble,

  kCharP,
  kUCharP,
  kShortP,
  kUShortP,
  kIntP,
  kUIntP,
  kLongP,
  kULongP,
  kLLongP,
  kULLongP,
  kFloatP,
  kDoubleP,
  kVoidP,

  kStructTmP,

#ifdef __cplusplus
  kNonCopyableAndNonMovable,
  kStdString,
  kStdStringView,
#endif  // __cplusplus
};
enum cfmt_error_code {
  kNoError,
  kBufferOverflow,
  kWrongType,
  kUnknowType,
  kUnmatchedBrace,
};
// In MSVC, va_list is only a const char*. So arguments will not be popped
// during recursivly processing. We have to wrap it and use it with reference.
struct cfmt_valist {
  va_list wrapped;
};
typedef enum cfmt_error_code (*cfmt_formatter_func_t)(struct string_buffer *buf,
                                                      enum cfmt_typeid type_id,
                                                      const char *fmt,
                                                      struct cfmt_valist *list);
// clang-format off
void _cfmt_println(const char *fmt, int count,
                   enum cfmt_typeid types[], ...);
const char *_cfmt_format(const char *fmt, int count,
                         enum cfmt_typeid types[], ...);
void _cfmt_fprint(FILE *fp, const char *fmt, int count,
                  enum cfmt_typeid types[], ...);
enum cfmt_error_code last_cfmt_errno(void);
const char *cfmt_strerr(void);
// clang-format on
void cfmt_internal_test(void);
void cfmt_set_default_formatter(cfmt_formatter_func_t func);
#ifdef __cplusplus
}
#endif  // __cplusplus

#ifdef __cplusplus
// All of this type should be used with pointer, so return pointer by default
template <typename T>
auto cfmt_convert(const T &val) {
  return &val;
}
// clang-format off
// Built-in types
inline auto cfmt_convert(const char                arg) { return arg; }
inline auto cfmt_convert(const unsigned char       arg) { return arg; }
inline auto cfmt_convert(const short               arg) { return arg; }
inline auto cfmt_convert(const unsigned short      arg) { return arg; }
inline auto cfmt_convert(const int                 arg) { return arg; }
inline auto cfmt_convert(const unsigned int        arg) { return arg; }
inline auto cfmt_convert(const long                arg) { return arg; }
inline auto cfmt_convert(const unsigned long       arg) { return arg; }
inline auto cfmt_convert(const long long           arg) { return arg; }
inline auto cfmt_convert(const unsigned long long  arg) { return arg; }
inline auto cfmt_convert(const float               arg) { return arg; }
inline auto cfmt_convert(const double              arg) { return arg; }

inline auto cfmt_convert(const char               *arg) { return arg; }
inline auto cfmt_convert(const unsigned char      *arg) { return arg; }
inline auto cfmt_convert(const short              *arg) { return arg; }
inline auto cfmt_convert(const unsigned short     *arg) { return arg; }
inline auto cfmt_convert(const int                *arg) { return arg; }
inline auto cfmt_convert(const unsigned int       *arg) { return arg; }
inline auto cfmt_convert(const long               *arg) { return arg; }
inline auto cfmt_convert(const unsigned long      *arg) { return arg; }
inline auto cfmt_convert(const long long          *arg) { return arg; }
inline auto cfmt_convert(const unsigned long long *arg) { return arg; }
inline auto cfmt_convert(const float              *arg) { return arg; }
inline auto cfmt_convert(const double             *arg) { return arg; }

inline auto cfmt_to_typeid(const char                arg) { return kChar; }
inline auto cfmt_to_typeid(const unsigned char       arg) { return kUChar; }
inline auto cfmt_to_typeid(const short               arg) { return kShort; }
inline auto cfmt_to_typeid(const unsigned short      arg) { return kUShort; }
inline auto cfmt_to_typeid(const int                 arg) { return kInt; }
inline auto cfmt_to_typeid(const unsigned int        arg) { return kUInt; }
inline auto cfmt_to_typeid(const long                arg) { return kLong; }
inline auto cfmt_to_typeid(const unsigned long       arg) { return kULong; }
inline auto cfmt_to_typeid(const long long           arg) { return kLLong; }
inline auto cfmt_to_typeid(const unsigned long long  arg) { return kULLong; }
inline auto cfmt_to_typeid(const float               arg) { return kFloat; }
inline auto cfmt_to_typeid(const double              arg) { return kDouble; }

inline auto cfmt_to_typeid(const char               *arg) { return kCharP; }
inline auto cfmt_to_typeid(const unsigned char      *arg) { return kUCharP; }
inline auto cfmt_to_typeid(const short              *arg) { return kShortP; }
inline auto cfmt_to_typeid(const unsigned short     *arg) { return kUShortP; }
inline auto cfmt_to_typeid(const int                *arg) { return kIntP; }
inline auto cfmt_to_typeid(const unsigned int       *arg) { return kUIntP; }
inline auto cfmt_to_typeid(const long               *arg) { return kLongP; }
inline auto cfmt_to_typeid(const unsigned long      *arg) { return kULongP; }
inline auto cfmt_to_typeid(const long long          *arg) { return kLLongP; }
inline auto cfmt_to_typeid(const unsigned long long *arg) { return kULLongP; }
inline auto cfmt_to_typeid(const float              *arg) { return kFloatP; }
inline auto cfmt_to_typeid(const double             *arg) { return kDoubleP; }
// clang-format on

#include <string>
#include <string_view>
inline auto cfmt_to_typeid(const std::string &arg) { return kStdString; }
inline auto cfmt_to_typeid(const std::string_view &arg) {
  return kStdStringView;
}
struct NonCopyableNonMovable;
inline auto cfmt_to_typeid(const NonCopyableNonMovable &arg) {
  return kNonCopyableAndNonMovable;
}

template <typename... Args>
const char *cfmt_format_cpp(const char *fmt, int line_no, Args &&...args) {
  cfmt_typeid types[] = {static_cast<cfmt_typeid>(line_no),
                         cfmt_to_typeid(args)...};
  int count = sizeof(types) / sizeof(int) - 1;
  return _cfmt_format(fmt, count, types, cfmt_convert(args)...);
}
template <typename... Args>
void cfmt_fprint_cpp(FILE *fp, const char *fmt, int line_no, Args &&...args) {
  cfmt_typeid types[] = {static_cast<cfmt_typeid>(line_no),
                         cfmt_to_typeid(args)...};
  int count = sizeof(types) / sizeof(int) - 1;
  _cfmt_fprint(fp, fmt, count, types, cfmt_convert(args)...);
}
#define cfmt_println(fmt, ...) \
  cfmt_fprint_cpp(stdout, fmt "\n", __LINE__, ##__VA_ARGS__)
#define cfmt_print(fmt, ...) \
  cfmt_fprint_cpp(stdout, fmt, __LINE__, ##__VA_ARGS__)
#define cfmt_format(fmt, ...) cfmt_format_cpp(fmt, __LINE__, ##__VA_ARGS__)
#define cfmt_fprint(fp, fmt, ...) \
  cfmt_fprint_cpp(fp, fmt, __LINE__, ##__VA_ARGS__)

#else  // !__cplusplus

#if defined(_MSC_VER) && !defined(__STDC_VERSION__)
#error "We need c11 and _Generic for MSVC! Please use '/std:c11'"
#endif

#define __COUNT_ARGS(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12,    \
                     _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, N, ...) \
  N
#define EXPAND(...) __VA_ARGS__
#if defined(__STRICT_ANSI__)
#define COUNT_ARGS(...)                                                        \
  __COUNT_ARGS(0, ##__VA_ARGS__ __VA_OPT__(, ) 22, 21, 20, 19, 18, 17, 16, 15, \
               14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 2000000000)
#else
#define COUNT_ARGS(...)                                                      \
  __COUNT_ARGS(0, ##__VA_ARGS__, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, \
               11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 2000000000)
#endif  // defined(_MSC_VER)

#define TYPE_ID(v)                          \
  _Generic((v),                             \
      char: kChar,                          \
      unsigned char: kUChar,                \
      short: kShort,                        \
      unsigned short: kUShort,              \
      int: kInt,                            \
      unsigned int: kUInt,                  \
      long: kLong,                          \
      unsigned long: kULong,                \
      long long: kLLong,                    \
      unsigned long long: kULLong,          \
      float: kFloat,                        \
      double: kDouble,                      \
                                            \
      const char *: kCharP,                 \
      const unsigned char *: kUCharP,       \
      const short *: kShortP,               \
      const unsigned short *: kUShortP,     \
      const int *: kIntP,                   \
      const unsigned int *: kUIntP,         \
      const long *: kLongP,                 \
      const unsigned long *: kULongP,       \
      const long long *: kLLongP,           \
      const unsigned long long *: kULLongP, \
      const float *: kFloatP,               \
      const double *: kDoubleP,             \
      void *: kVoidP,                       \
                                            \
      char *: kCharP,                       \
      unsigned char *: kUCharP,             \
      short *: kShortP,                     \
      unsigned short *: kUShortP,           \
      int *: kIntP,                         \
      unsigned int *: kUIntP,               \
      long *: kLongP,                       \
      unsigned long *: kULongP,             \
      long long *: kLLongP,                 \
      unsigned long long *: kULLongP,       \
      float *: kFloatP,                     \
      double *: kDoubleP,                   \
      const void *: kVoidP,                 \
                                            \
      struct tm *: kStructTmP,              \
                                            \
      default: kUnknow)

#define TYPE_ID_22(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_21(__VA_ARGS__))
#define TYPE_ID_21(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_20(__VA_ARGS__))
#define TYPE_ID_20(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_19(__VA_ARGS__))
#define TYPE_ID_19(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_18(__VA_ARGS__))
#define TYPE_ID_18(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_17(__VA_ARGS__))
#define TYPE_ID_17(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_16(__VA_ARGS__))
#define TYPE_ID_16(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_15(__VA_ARGS__))
#define TYPE_ID_15(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_14(__VA_ARGS__))
#define TYPE_ID_14(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_13(__VA_ARGS__))
#define TYPE_ID_13(v, ...) EXPAND(TYPE_ID(v), TYPE_ID_12(__VA_ARGS__))
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
#define TYPE_ID_2000000000() abort()

#define MACRO_CONCAT(a, b) a##b
#define CONCAT(a, b) MACRO_CONCAT(a, b)

#define TYPE_ARRAY(COUNT, ...) \
  EXPAND((enum cfmt_typeid[]){__LINE__, CONCAT(TYPE_ID_, COUNT)(__VA_ARGS__)})

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
