#include "cfmt.h"
int main() {
  cfmt_println("");
  cfmt_println("{}|", CH(0));
  cfmt_println("{} {}|", (unsigned char)'1', -(short)2);
  cfmt_println("{} {} {}|", (unsigned short)3, -4, 5u);
  cfmt_println("{} {} {} {}|", -6l, 7lu, -8ll, 9llu);
  int v1 = 0;
  short v2 = 0;
  cfmt_println("{} {} {} {} {}|", (float)10, 11., "12", &v1, &v2);
  return 0;
}
