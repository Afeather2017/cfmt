#include "cfmt.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
struct string_buffer {
  int size;
  int capacity;
  char *data;
};
// In MSVC, va_list is only a const char*. So arguments will not be popped
// during recursivly process. We have to wrap it and use it with reference.
static void string_buffer_init(struct string_buffer *buf, char *data, int len) {
  buf->capacity = len;
  buf->size = 0;
  buf->data = data;
}
static enum cfmt_error_code put_char_to_buf(struct string_buffer *buf,
                                            char ch) {
  if (buf->size + 1 >= buf->capacity) return kBufferOverflow;
  buf->data[buf->size++] = ch;
  assert(buf->size + 1 <= buf->capacity);
  buf->data[buf->size] = '\0';
  return kNoError;
}
// clang-format off
static enum cfmt_error_code append_data_to_buf(struct string_buffer *buf,
                                               const char *data, int len) {
  enum cfmt_error_code err = kNoError;
  if (buf->size + len + 1 >= buf->capacity) {
    len = buf->capacity - buf->size - 1;
    err = kBufferOverflow;
  }
  // clang-format on
  assert(buf->size + len + 1 <= buf->capacity);
  memcpy(buf->data + buf->size, data, len);
  buf->size += len;
  buf->data[buf->size] = '\0';
  return err;
}
static enum cfmt_error_code concat_to_buf(struct string_buffer *buf,
                                          const char *str) {
  return append_data_to_buf(buf, str, strlen(str));
}
static enum cfmt_error_code concat_vsnprintf_one(struct string_buffer *buf,
                                                 enum cfmt_typeid type_id,
                                                 const char *fmt,
                                                 struct cfmt_valist *list) {
  // In GNU/clang, taking a copy of va_list actually take a reference.
  // while in MSVC, it take a copy. So we have to pop value by value.
  int ret = 0;
  char *start = buf->data + buf->size;
  int size = buf->capacity - buf->size;
  switch (type_id) {
    case kChar:
    case kUChar:
    case kShort:
    case kUShort:
    case kInt:
    case kUInt:
      // char will be promoted to int in function arguments
      // short will be promoted to int in function arguments
      ret = snprintf(start, size, fmt, (int)va_arg(list->wrapped, int));
      break;
    case kLong:
    case kULong:
      ret = snprintf(start, size, fmt, (long)va_arg(list->wrapped, long));
      break;
    case kLLong:
    case kULLong:
      ret = snprintf(start, size, fmt,
                     (long long)va_arg(list->wrapped, long long));
      break;
    case kFloat:
    case kDouble:
      // float will be promoted to int in function arguments
      ret = snprintf(start, size, fmt, (double)va_arg(list->wrapped, double));
      break;
    case kCharP:
    case kUCharP:
      ret = snprintf(start, size, fmt, (char *)va_arg(list->wrapped, char *));
      break;
    case kShortP:
    case kUShortP:
    case kIntP:
    case kUIntP:
    case kLongP:
    case kULongP:
    case kLLongP:
    case kULLongP:
    case kFloatP:
    case kDoubleP:
    case kVoidP:
      ret = snprintf(start, size, fmt, va_arg(list->wrapped, void *));
      break;
    case kUnknow:
      return kUnknowType;
      break;
    default:
      abort();
  }
  if (ret < 0) abort();
  if (ret >= size) return kBufferOverflow;
  buf->size += ret;
  return kNoError;
}
static char *get_buf_as_c_string(struct string_buffer *buf) {
  buf->data[buf->size] = '\0';
  return buf->data;
}
static char conversion_specifier(enum cfmt_typeid type_id) {
  // clang-format off
  switch (type_id) {
    case kChar:    return 'c';
    case kUChar:   return 'c';
    case kShort:   return 'd';
    case kUShort:  return 'u';
    case kInt:     return 'd';
    case kUInt:    return 'u';
    case kLong:    return 'd';
    case kULong:   return 'u';
    case kLLong:   return 'd';
    case kULLong:  return 'u';
    case kFloat:   return 'f';
    case kDouble:  return 'f';

    case kCharP:
    case kUCharP:  return 's';
    case kShortP:
    case kUShortP:
    case kIntP:
    case kUIntP:
    case kLongP:
    case kULongP:
    case kLLongP:
    case kULLongP:
    case kFloatP:
    case kDoubleP:
    case kVoidP:   return 'p';
    case kUnknow:  return '?';
    default: return 's';
  }
  // clang-format on
}

static const char *length_modifier(enum cfmt_typeid type_id) {
  // clang-format off
  switch (type_id) {
    case kChar:
    case kUChar:   return "hh";
    case kShort:
    case kUShort:  return "h";
    case kInt:
    case kUInt:    return "";
    case kLong:
    case kULong:   return "l";
    case kLLong:
    case kULLong:  return "ll";
    case kFloat:   return "";
    case kDouble:  return "l";

    case kCharP:
    case kUCharP:
    case kShortP:
    case kUShortP:
    case kIntP:
    case kUIntP:
    case kLongP:
    case kULongP:
    case kLLongP:
    case kULLongP:
    case kFloatP:
    case kDoubleP:
    case kVoidP:
    case kUnknow:
    default: return "";
  }
  // clang-format on
}
static enum cfmt_error_code append_spec(struct string_buffer *buf,
                                        enum cfmt_typeid type_id,
                                        const char *spec, int spec_len) {
  if (spec_len == 0 || spec == NULL) {
    concat_to_buf(buf, length_modifier(type_id));
    put_char_to_buf(buf, conversion_specifier(type_id));
    return kNoError;
  }
  char conv;
  // clang-format off
  switch (tolower(spec[spec_len - 1])) {
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'o':
    case 'p':
    case 's':
    case 'u':
    case 'x':
      spec_len--;
      conv = spec[spec_len];
      break;
    default:
      conv = conversion_specifier(type_id);
      if (conv == '?')
        return kUnknowType;
      break;
  }
  // clang-format on
  enum cfmt_error_code err = kNoError;
  // If buffer overflow, then they must return kBufferOverflow.
  err = append_data_to_buf(buf, spec, spec_len);
  err = concat_to_buf(buf, length_modifier(type_id));
  err = put_char_to_buf(buf, conv);
  return err;
}
_Thread_local static char fmt_raw_buf[256], out_raw_buf[60000];
_Thread_local static char cfmt_errstr[1024];
_Thread_local struct {
  int char_at;
  enum cfmt_error_code cfmt_errno;
} last_err_record;
enum cfmt_error_code last_cfmt_errno(void) {
  return last_err_record.cfmt_errno;
}
const char *cfmt_strerr(void) { return cfmt_errstr; }
// convert first value of va_list to string, with format `fmt`
// if `fmt` = NULL or `fmt` = "" use default format.
static enum cfmt_error_code format_and_append(struct string_buffer *buf,
                                              struct string_buffer *fmt,
                                              enum cfmt_typeid type_id,
                                              struct cfmt_valist *list) {
  char raw_spec_buf[300];
  struct string_buffer spec_buf;
  string_buffer_init(&spec_buf, raw_spec_buf, sizeof(raw_spec_buf));
  put_char_to_buf(&spec_buf, '%');
  const char *fmt_str = "";
  int fmt_str_len = 0;
  if (fmt != NULL) {
    fmt_str = get_buf_as_c_string(fmt);
    fmt_str_len = fmt->size;
  }
  enum cfmt_error_code err = kNoError;
  err = append_spec(&spec_buf, type_id, fmt_str, fmt_str_len);
  if (err != kNoError) return err;
  return concat_vsnprintf_one(buf, type_id, raw_spec_buf, list);
}
static const char *cfmt(const char *fmt, int count, enum cfmt_typeid types[],
                        struct cfmt_valist *list) {
  char stat = 'c';
  const char *fmt_spec_start = NULL;
  const char *fmt_spec_end = NULL;
  int type_id_index = 0;
  struct string_buffer fmt_buf;
  struct string_buffer out_buf;
  string_buffer_init(&fmt_buf, fmt_raw_buf, sizeof(fmt_raw_buf));
  string_buffer_init(&out_buf, out_raw_buf, sizeof(out_raw_buf));
  int needed_arg_count = 0;
  enum cfmt_error_code err = kNoError;
  int line_no = types[0];
  int i = 0;
  types++;
  // https://fmt.dev/12.0/syntax/
  for (i = 0; fmt[i] != '\0' && err == kNoError; i++) {
    char ch = fmt[i];
    switch (stat) {
      case 'c':
        if (ch == '{') {
          stat = '{';
          string_buffer_init(&fmt_buf, fmt_raw_buf, sizeof(fmt_raw_buf));
        } else if (ch == '}') {
          stat = '}';
        } else {
          err = put_char_to_buf(&out_buf, ch);
          // stat = 'c';
        }
        break;
      case '{':
        if (ch == '{') {
          stat = 'c';
          err = put_char_to_buf(&out_buf, '{');
        } else if (ch == ':') {
          fmt_spec_start = fmt + i;
          stat = ':';
        } else if (ch == '}') {
          enum cfmt_typeid type_id =
              type_id_index >= count ? kUnknow : types[type_id_index++];
          // TODO
          err = format_and_append(&out_buf, &fmt_buf, type_id, list);
          needed_arg_count++;
          stat = 'c';
        } else {
          put_char_to_buf(&out_buf, ch);
          stat = 'c';
        }
        break;
      case '}':
        if (ch == '}') {
          err = put_char_to_buf(&out_buf, '}');
          stat = 'c';
        } else if (ch == '{') {
          err = put_char_to_buf(&out_buf, '}');
          string_buffer_init(&fmt_buf, fmt_raw_buf, sizeof(fmt_raw_buf));
          stat = '{';
        } else {
          err = put_char_to_buf(&out_buf, '}');
          err = put_char_to_buf(&out_buf, ch);
          stat = 'c';
        }
        break;
      case ':':
        if (ch == '}') {
          fmt_spec_end = fmt + i;
          enum cfmt_typeid type_id =
              type_id_index >= count ? kUnknow : types[type_id_index++];
          // It is ':', so skip it.
          fmt_spec_start++;
          int spec_len = (int)(fmt_spec_end - fmt_spec_start);
          assert(spec_len >= 0);
          err = format_and_append(&out_buf, &fmt_buf, type_id, list);
          needed_arg_count++;
          stat = 'c';
        } else if (ch == '*') {
          enum cfmt_typeid type_id =
              type_id_index >= count ? kUnknow : types[type_id_index++];
          err = format_and_append(&fmt_buf, NULL, type_id, list);
        } else {
          err = put_char_to_buf(&fmt_buf, ch);
          // stat = ':';
        }
        break;
    }
  }
  if (err == kNoError && fmt[i] == '\0') {
    switch (stat) {
      case '}':
        err = put_char_to_buf(&out_buf, '}');
        break;
      case ':':
        err = kUnmatchedBrace;
        break;
      case '{':
        err = kUnmatchedBrace;
        break;
      default:
        break;
    }
  }
  switch (err) {
    case kNoError:
      break;
    case kBufferOverflow:
      fprintf(stderr, "cfmt:%d: format string too long, fmt='%s'\n", line_no,
              fmt);
      break;
    case kWrongType:
      fprintf(stderr,
              "cfmt:%d: incorrect type at '*', this has to be int, fmt='%s'\n",
              line_no, fmt);
      break;
    case kUnknowType:
      fprintf(stderr, "cfmt:%d: Unknow type when format, fmt='%s'", line_no,
              fmt);
      break;
    case kUnmatchedBrace:
      fprintf(stderr, "cfmt:%d: Unmatched braces, fmt='%s'", line_no, fmt);
      break;
  }
  if (err == kNoError && needed_arg_count != count) {
    fprintf(
        stderr,
        "cfmt:%d: We need %d argument(s), but %d argument(s) given, fmt='%s'\n",
        line_no, needed_arg_count, count, fmt);
  }
  return get_buf_as_c_string(&out_buf);
}
// clang-format off
const char *_cfmt_format(const char *fmt, int count,
                         enum cfmt_typeid types[], ...) {
  // clang-format on
  struct cfmt_valist list;
  va_start(list.wrapped, types);
  const char *ret = cfmt(fmt, count, types, &list);
  va_end(list.wrapped);
  return ret;
}
// clang-format off
void _cfmt_fprint(FILE *fp, const char *fmt, int count,
                  enum cfmt_typeid types[], ...) {
  // clang-format on
  struct cfmt_valist list;
  va_start(list.wrapped, types);
  const char *output = cfmt(fmt, count, types, &list);
  va_end(list.wrapped);
  fputs(output, fp);
}
void cfmt_internal_test(void) {
  cfmt_print("testing {}", __func__);
  struct string_buffer buf;
  char raw_buf[1024];
  enum cfmt_error_code err;
  string_buffer_init(&buf, raw_buf, sizeof(raw_buf));
  buf.capacity = 10;
  for (int i = 0; i < 10; i++) err = put_char_to_buf(&buf, '0' + i);
  assert(0 == strcmp(get_buf_as_c_string(&buf), "012345678"));
  assert(err == kBufferOverflow);

  string_buffer_init(&buf, raw_buf, sizeof(raw_buf));
  buf.capacity = 10;
  err = concat_to_buf(&buf, "123456789abcdef");
  assert(err == kBufferOverflow);
  assert(0 == strcmp(get_buf_as_c_string(&buf), "123456789"));

  string_buffer_init(&buf, raw_buf, sizeof(raw_buf));
  buf.capacity = 10;
  err = concat_to_buf(&buf, "1234567");
  assert(err == kNoError);
  err = concat_to_buf(&buf, "89abcdef");
  assert(err == kBufferOverflow);
  puts(get_buf_as_c_string(&buf));
  assert(0 == strcmp(get_buf_as_c_string(&buf), "123456789"));

  string_buffer_init(&buf, raw_buf, sizeof(raw_buf));
  buf.capacity = 10;
  err = append_data_to_buf(&buf, "0123abcdefg", 4);
  assert(err == kNoError);
  assert(0 == strcmp(get_buf_as_c_string(&buf), "0123"));
  err = append_data_to_buf(&buf, "456789abcd", 6);
  assert(err == kBufferOverflow);
  assert(0 == strcmp(get_buf_as_c_string(&buf), "012345678"));

  char another_buf[10];
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
#endif
  assert(snprintf(another_buf, 10, "0123456789") >= 10);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
  cfmt_println("success!");
}
