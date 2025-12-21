#include <cassert>
#include <cstring>
#include <string>
#include <string_view>

#include "cfmt.h"

cfmt_typeid to_typeid(const std::string& s) { return kCharP; }
cfmt_typeid to_typeid(const std::string_view& sv) { return kStdStringView; }
// It's OK to use unmovable and uncopyable type
struct NonCopyableNonMovable {
  int a = 10;
  int b = 11;
  NonCopyableNonMovable() = default;
  NonCopyableNonMovable(const NonCopyableNonMovable&) = delete;
  NonCopyableNonMovable& operator=(const NonCopyableNonMovable&) = delete;
  NonCopyableNonMovable(NonCopyableNonMovable&&) = delete;
  NonCopyableNonMovable& operator=(NonCopyableNonMovable&&) = delete;
};
cfmt_typeid to_typeid(NonCopyableNonMovable& v) {
  return kNonCopyableAndNonMovable;
}

cfmt_error_code formatter_cpp(struct string_buffer* buf,
                              enum cfmt_typeid type_id, const char* fmt,
                              struct cfmt_valist* list) {
  char* start = buf->data + buf->size;
  int size = buf->capacity - buf->size;
  int ret = 0;
  if (type_id == kStdStringView) {
    auto* sv = va_arg(list->wrapped, std::string_view*);
    ret = snprintf(start, size, "%.*s", (int)sv->size(), sv->data());
  } else if (type_id == kNonCopyableAndNonMovable) {
    auto* ncnm = va_arg(list->wrapped, NonCopyableNonMovable*);
    ret = snprintf(start, size, "{%d,%d}", ncnm->a, ncnm->b);
  } else if (type_id == kStdString) {
    auto* sp = va_arg(list->wrapped, std::string*);
    ret = snprintf(start, size, "%s", sp->c_str());
  } else {
    return kUnknowType;
  }
  if (ret < 0) abort();
  if (ret >= size) return kBufferOverflow;
  buf->size += ret;
  return kNoError;
}
const int v0 = 0, v1 = 1, v2 = 2, v3 = 3, v4 = 4;
int main() {
  cfmt_set_default_formatter(formatter_cpp);
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
  bool bool_val = true;
  cfmt_println("bool_val={} !bool_val={} true={} false={} &bool_val={}",
               bool_val, !bool_val, true, false, &bool_val);
  std::string str = "std::string{'std::string'}";
  cfmt_println("{}", str);
  std::string_view sv = "std::string_view{'std::string_view'}";
  cfmt_println("{}", sv);
  NonCopyableNonMovable ncnm{};
  cfmt_println("{}", ncnm);
  cfmt_internal_test();
  return 0;
}
