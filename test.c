#include <assert.h>

#include "cfmt.c"
#include "cfmt.h"
const static int v0 = COUNT_ARGS() == 0;
const static int v1 = COUNT_ARGS(CH(0)) == 1;
static int v2 = COUNT_ARGS((unsigned char)'1', -(short)2) == 2;
static int v3 = COUNT_ARGS((unsigned short)3, -4, 5u) == 3;
static int v4 = COUNT_ARGS(-6l, 7lu, -8ll, -9llu) == 4;
static int v5 = COUNT_ARGS((float)10, 11., "12", &v0, &v1) == 5;
static int v12 = COUNT_ARGS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11) == 12;
void internal_function_test() {
  struct string_buffer buf = {};
  string_buffer_init(&buf);
  buf.capacity = 10;
  for (int i = 0; i < 10; i++) put_char_to_buf(&buf, '0' + i);
  assert(0 == strcmp(get_buf_as_c_string(&buf), "012345678"));

  string_buffer_init(&buf);
  buf.capacity = 10;
  concat_to_buf(&buf, "123456789abcdef");
  assert(0 == strcmp(get_buf_as_c_string(&buf), "123456789"));

  string_buffer_init(&buf);
  buf.capacity = 10;
  concat_to_buf(&buf, "1234567");
  concat_to_buf(&buf, "89abcdef");
  assert(0 == strcmp(get_buf_as_c_string(&buf), "123456789"));

  string_buffer_init(&buf);
  buf.capacity = 10;
  append_data_to_buf(&buf, "0123abcdefg", 4);
  assert(0 == strcmp(get_buf_as_c_string(&buf), "0123"));
  append_data_to_buf(&buf, "456789abcd", 6);
  assert(0 == strcmp(get_buf_as_c_string(&buf), "012345678"));
}
int main() {
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

  sprintf(buf, "%p|%p", &v0, &v1);
  result = cfmt_format("{}|{}", &v0, &v1);
  assert(0 == strcmp(buf, result));

  sprintf(buf, "%d|%d|%d|%d|%d", v0, v1, v2, v3, v4);
  result = cfmt_format("{}|{}|{}|{}|{}", v0, v1, v2, v3, v4);
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

  internal_function_test();
  return 0;
}
