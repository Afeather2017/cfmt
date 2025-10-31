#include <cassert>
#include <cstring>

#include "cfmt.h"
const int v0 = 0, v1 = 1, v2 = 2, v3 = 3, v4 = 4;
int main() {
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

  cfmt_println("{}|", CH(0));
  cfmt_println("{} {}|", (unsigned char)'1', -(short)2);
  cfmt_println("{} {} {}|", (unsigned short)3, -4, 5u);
  cfmt_println("{} {} {} {}|", -6l, 7lu, -8ll, 9llu);
  cfmt_println("{} {} {} {} {}|", (float)10, 11., "12", &v1, &v2);
  cfmt_println("{:#x}|{:-#10o}|{:10.3}|{:+10.0g}|{:.3e}|{}|", 15, 16, -0.0017,
               18e8, 19e7, 20);
  return 0;
}
