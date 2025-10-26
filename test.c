#include "cfmt.h"
#include <assert.h>
const static int v0 = COUNT_ARGS() == 0;
const static int v1 = COUNT_ARGS("a") == 1;
const static int v2 = COUNT_ARGS("a", 1) == 2;
const static int v3 = COUNT_ARGS("a", 1, 2) == 3;
int main() {
  assert(v0);
  assert(v1);
  assert(v2);
  assert(v3);
  cfmt_println("{} {} {}|", "bbbb", 1, 2llu);
  cfmt_println("{} {}|", 1.1f, 2.3);
  cfmt_println("{} {}|", 1.1f + 2.3, "aa" "bb");
  puts(cfmt_format("cfmt"));
}
