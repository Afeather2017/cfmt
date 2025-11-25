#include <assert.h>
#include <string.h>
#include <time.h>

#include "cfmt.h"
const static int v0 = COUNT_ARGS() == 0;
const static int v1 = COUNT_ARGS(CH(0)) == 1;
static int v2 = COUNT_ARGS((unsigned char)'1', -(short)2) == 2;
static int v3 = COUNT_ARGS((unsigned short)3, -4, 5u) == 3;
static int v4 = COUNT_ARGS(-6l, 7lu, -8ll, -9llu) == 4;
static int v5 = COUNT_ARGS((float)10, 11., "12", &v0, &v1) == 5;
static int v12 = COUNT_ARGS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11) == 12;
static int v22 = COUNT_ARGS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                            15, 16, 17, 18, 19, 20, 21) == 22;
FILE* fp = NULL;
#include <time.h>
#define Log(fmt, ...)                                                      \
  do {                                                                     \
    time_t current_time = time(NULL);                                      \
    struct tm stm = *localtime(&current_time);                             \
    char time_string[1024];                                                \
    strftime(time_string, sizeof(time_string), "%d-%m-%Y %H-%M-%S", &stm); \
    cfmt_fprint(fp, "LOG {} {} {}: " fmt "\n", time_string, __LINE__,      \
                __func__, ##__VA_ARGS__);                                  \
  } while (0)
static void va_arg_recursive_vprocess_char(char* buf, int n,
                                           struct cfmt_valist* list) {
  if (n == 0) return;
  buf[0] = (char)va_arg(list->wrapped, int);
  va_arg_recursive_vprocess_char(buf + 1, n - 1, list);
}
static void va_arg_recursive_process_char(char* buf, int n, ...) {
  struct cfmt_valist list;
  va_start(list.wrapped, n);
  va_arg_recursive_vprocess_char(buf, n / 2, &list);
  va_arg_recursive_vprocess_char(buf + n / 2, n / 2 + (n & 1), &list);
  va_end(list.wrapped);
}
static void stdarg_promote_test(int n, ...) {
  struct cfmt_valist list;
  va_start(list.wrapped, n);
  assert('0' == va_arg(list.wrapped, int));
  assert(1 == va_arg(list.wrapped, int));
  assert(2 == va_arg(list.wrapped, int));
  assert(3 == va_arg(list.wrapped, long));
  assert(4 == va_arg(list.wrapped, long long));
  assert(5 == va_arg(list.wrapped, double));
  assert(6 == va_arg(list.wrapped, double));
  va_end(list.wrapped);
}
static void va_arg_test(void) {
  cfmt_print("testing stdarg...");
  {
    char array[10] = {};
    const char* s = "0123456";
    va_arg_recursive_process_char(array, 4, s[0], s[1], s[2], s[3]);
    assert(0 == strcmp(array, "0123"));
  }
  stdarg_promote_test(7, (char)'0', (short)1, (int)2, (long)3, (long long)4,
                      (float)5, (double)6);
  cfmt_println("success!");
}
int main() {
  fp = stdout;

  cfmt_print("testing ARG_COUNT...");
  assert(v0);
  assert(v1);
  assert(v2);
  assert(v3);
  assert(v4);
  assert(v5);
  assert(v12);
  assert(v22);
  cfmt_println("success!");

  cfmt_println("testing cfmt and print...");
  assert(0 == strcmp("cfmt", cfmt_format("cfmt")));
  assert(0 == strcmp("0", cfmt_format("{}", CH(0))));
  const char* result = NULL;
  result = cfmt_format("{} {}", (unsigned char)'1', -(short)2);
  assert(0 == strcmp("1 -2", result));
  result = cfmt_format("{} {} {}", (unsigned short)3, -4, 5u);
  assert(0 == strcmp("3 -4 5", result));
  assert(0 ==
         strcmp("-6 7 -8 9", cfmt_format("{} {} {} {}", -6l, 7lu, -8ll, 9llu)));

  result = cfmt_format("{:#x}|{:-#10o}|{:10.3}|{:+10.0g}|{:.3e}|{}", 15, 16,
                       -0.0017, 18e8, 19e7, 20);
  char buf[1024];
  sprintf(buf, "%#x|%-#10o|%10.3lf|%+10.0g|%.3e|%d", 15, 16, -0.0017, 18e8,
          19e7, 20);
  assert(0 == strcmp(result, buf));

  result = cfmt_format("{:*.*}", 10, 10, 0.0000000005);
  sprintf(buf, "%*.*lf", 10, 10, 0.0000000005);
  assert(0 == strcmp(result, buf));

  sprintf(buf, "%p|%p|%p|%p|%p", &v0, &v1, &v2, &v3, &v4);
  result = cfmt_format("{}|{}|{}|{}|{}", &v0, &v1, &v2, &v3, &v4);
  assert(0 == strcmp(buf, result));

  sprintf(buf, "%d|%d|%d|%d|%d", v0, v1, v2, v3, v4);
  result = cfmt_format("{}|{}|{}|{}|{}", v0, v1, v2, v3, v4);
  assert(0 == strcmp(buf, result));

  sprintf(buf, "{}");
  result = cfmt_format("{{}}");
  assert(0 == strcmp(buf, result));

  sprintf(buf, "{1}");
  result = cfmt_format("{{{}}", 1);
  assert(0 == strcmp(buf, result));

  result = cfmt_format("{}|{}", (void*)0x1234, (const void*)0x5678);
  assert(NULL != strstr(result, "1234"));
  assert(NULL != strstr(result, "5678"));

  result = cfmt_format("{}|{}", (int*)0x1234, (const long*)0x5678);
  assert(NULL != strstr(result, "1234"));
  assert(NULL != strstr(result, "5678"));

  cfmt_println("{}|", CH(0));
  cfmt_println("{} {}|", (unsigned char)'1', -(short)2);
  cfmt_println("{} {} {}|", (unsigned short)3, -4, 5u);
  cfmt_println("{} {} {} {}|", -6l, 7lu, -8ll, 9llu);
  cfmt_println("{} {} {} {} {}|", (float)10, 11., "12", &v1, &v2);
  cfmt_println("{:#x}|{:-#10o}|{:10.3}|{:+10.0g}|{:.3e}|{}|", 15, 16, -0.0017,
               18e8, 19e7, 20);
  cfmt_println("{} {}|", 1, 2, 3);

  // test 22 arguments
  cfmt_println("{} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}",
               0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
               19, 20, 21);

  Log("{}{}", "This is", " a log message");
  cfmt_println("success!");

  cfmt_println("Test cfmt_errno");
  result = cfmt_format("{:*.*}", 10, 10., 0.0000000005);
  assert(cfmt_last_errno() == kWrongType);
  cfmt_println("{}", cfmt_strerr());
  assert(cfmt_last_errno() == kNoError);

  result = cfmt_format("{:.*}", "a", "1000000000");
  assert(cfmt_last_errno() == kWrongType);
  cfmt_println("{}", cfmt_strerr());

  result = cfmt_format("{}", "a", "1000000000");
  assert(cfmt_last_errno() == kUnmatchedArgCount);
  cfmt_println("{}", cfmt_strerr());

  result = cfmt_format("111{}222");
  assert(cfmt_last_errno() == kUnmatchedArgCount);
  cfmt_println("{}", cfmt_strerr());

  result = cfmt_format("{ ");
  assert(cfmt_last_errno() == kWrongFormatSpec);
  cfmt_println("{}", cfmt_strerr());

  result = cfmt_format("{");
  assert(cfmt_last_errno() == kUnmatchedBrace);
  cfmt_println("{}", cfmt_strerr());

  result = cfmt_format("} ");
  assert(cfmt_last_errno() == kNoError);
  assert(0 == strcmp("", cfmt_strerr()));

  {
    struct unknow_type {
      int a, b;
    } v;
    cfmt_println("test format unknow type {}", &v);
    assert(cfmt_last_errno() == kUnknowType);
    cfmt_println("{}", cfmt_strerr());
  }

  // full the buffer.
  {
    static char large_buf[CFMT_OUTPUT_SIZE * 2];
    memset(large_buf, '0', sizeof(large_buf));
    cfmt_format("{}", large_buf);
    assert(cfmt_last_errno() == kBufferOverflow);
    cfmt_println("{}", cfmt_strerr());

    cfmt_format(large_buf);
    assert(cfmt_last_errno() == kBufferOverflow);
    cfmt_println("{}", cfmt_strerr());
  }

  cfmt_println("success!");

  {
    time_t current_time = time(NULL);
    struct tm stm = *localtime(&current_time);
    char a[400];
    strcpy(a, cfmt_format("{}", &stm));
    char b[400];
    strftime(b, sizeof(b), "%Y%m%d-%H%M%S", &stm);

    cfmt_print("test struct tm *:\nexpect {}, got {}\n", b, a);
    assert(strcmp(a, b) == 0);
    cfmt_println("success!");
  }
  cfmt_internal_test();
  va_arg_test();
  return 0;
}
