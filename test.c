#include <assert.h>
#include <string.h>

#include "cfmt.h"
const static int v0 = COUNT_ARGS() == 0;
const static int v1 = COUNT_ARGS(CH(0)) == 1;
static int v2 = COUNT_ARGS((unsigned char)'1', -(short)2) == 2;
static int v3 = COUNT_ARGS((unsigned short)3, -4, 5u) == 3;
static int v4 = COUNT_ARGS(-6l, 7lu, -8ll, -9llu) == 4;
static int v5 = COUNT_ARGS((float)10, 11., "12", &v0, &v1) == 5;
static int v12 = COUNT_ARGS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11) == 12;
FILE *fp = NULL;
#define Log(fmt, ...)                                                    \
  do {                                                                   \
    time_t current_time = time(NULL);                                    \
    auto tm = localtime(&current_time);                                  \
    char time_string[1024];                                              \
    strftime(time_string, sizeof(time_string), "%d-%m-%Y %H-%M-%S", tm); \
    cfmt_fprint(fp, "LOG {} {} {}: " fmt "\n", time_string, __LINE__,    \
                __func__, ##__VA_ARGS__);                                \
  } while (0)
int main() {
  fp = stdout;
  assert(v0);
  assert(v1);
  assert(v2);
  assert(v3);
  assert(v4);
  assert(v5);
  assert(v12);
  assert(0 == strcmp("cfmt", cfmt_format("cfmt")));
  assert(0 == strcmp("0", cfmt_format("{}", CH(0))));
  const char *result = NULL;
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

  result = cfmt_format("{:*.*}", 10, 10., 0.0000000005);

  sprintf(buf, "%p|%p|%p|%p|%p", &v0, &v1, &v2, &v3, &v4);
  result = cfmt_format("{}|{}|{}|{}|{}", &v0, &v1, &v2, &v3, &v4);
  assert(0 == strcmp(buf, result));

  sprintf(buf, "%d|%d|%d|%d|%d", v0, v1, v2, v3, v4);
  result = cfmt_format("{}|{}|{}|{}|{}", v0, v1, v2, v3, v4);
  assert(0 == strcmp(buf, result));

  sprintf(buf, "{}");
  result = cfmt_format("{{}}");
  assert(0 == strcmp(buf, result));

  result = cfmt_format("{}|{}", (void *)0x1234, (const void *)0x5678);
  assert(NULL != strstr(result, "1234"));
  assert(NULL != strstr(result, "5678"));

  result = cfmt_format("{}|{}", (int *)0x1234, (const long *)0x5678);
  assert(NULL != strstr(result, "1234"));
  assert(NULL != strstr(result, "5678"));

  cfmt_println("{}|", CH(0));
  cfmt_println("{} {}|", (unsigned char)'1', -(short)2);
  cfmt_println("{} {} {}|", (unsigned short)3, -4, 5u);
  cfmt_println("{} {} {} {}|", -6l, 7lu, -8ll, 9llu);
  cfmt_println("{} {} {} {} {}|", (float)10, 11., "12", &v1, &v2);
  cfmt_println("{:#x}|{:-#10o}|{:10.3}|{:+10.0g}|{:.3e}|{}|", 15, 16, -0.0017,
               18e8, 19e7, 20);
  cfmt_println("{}|");
  cfmt_println("{} {}|", 1, 2, 3);

  Log("{}{}", "This is", " a log message");
  cfmt_internal_test();
  return 0;
}
