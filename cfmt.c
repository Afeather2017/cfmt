#include "cfmt.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
enum cfmt_error_code {
  kNoError,
  kBufferOverflow,
  kWrongType,
  kUnknowType,
  kUnmatchedBrace,
};
struct string_buffer {
  int size;
  int capacity;
  char data[60000];
};
static void string_buffer_init(struct string_buffer *buf) {
  buf->capacity = sizeof(buf->data);
  buf->size = 0;
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
static char *get_buf_as_c_string(struct string_buffer *buf) {
  buf->data[buf->size] = '\0';
  return buf->data;
}
static char conversion_specifier(int type_id) {
  // clang-format off
  switch (type_id) {
    case 0u: return 'c';
    case 1u: return 'c';
    case 2u: return 'd';
    case 3u: return 'u';
    case 4u: return 'd';
    case 5u: return 'u';
    case 6u: return 'd';
    case 7u: return 'u';
    case 8u: return 'd';
    case 9u: return 'u';
    case 10: return 'f';
    case 11: return 'f';

    case 50:
    case 51: return 's';
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62: return 'p';
    case 2000000000: return '?';
    default: abort();
  }
  // clang-format on
}
static const char *length_modifier(int type_id) {
  // clang-format off
  switch (type_id) {
    case 0u:
    case 1u: return "hh";
    case 2u:
    case 3u: return "h";
    case 4u:
    case 5u: return "";
    case 6u:
    case 7u: return "l";
    case 8u:
    case 9u: return "ll";
    case 10: return "";
    case 11: return "l";

    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 2000000000: return "";
    default: abort();
  }
  // clang-format on
}
static enum cfmt_error_code append_spec(struct string_buffer *buf, int type_id,
                                        const char *spec, int spec_len) {
  put_char_to_buf(buf, '%');
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
_Thread_local static struct string_buffer fmt_buf, out_buf;
const char *cfmt(const char *fmt, int count, int types[], va_list list) {
  char stat = 'c';
  const char *fmt_spec_start = NULL;
  const char *fmt_spec_end = NULL;
  int type_id_index = 0;
  string_buffer_init(&fmt_buf);
  string_buffer_init(&out_buf);
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
        } else if (ch == '}') {
          stat = '}';
        } else if (ch == '%') {
          err = concat_to_buf(&fmt_buf, "%%");
          // stat = 'c';
        } else {
          err = put_char_to_buf(&fmt_buf, ch);
          // stat = 'c';
        }
        break;
      case '{':
        if (ch == '{') {
          stat = 'c';
          err = put_char_to_buf(&fmt_buf, '{');
        } else if (ch == '%') {
          stat = 'c';
          err = concat_to_buf(&fmt_buf, "%%");
        } else if (ch == ':') {
          fmt_spec_start = fmt + i;
          stat = 's';
        } else if (ch == '}') {
          int type_id =
              type_id_index >= count ? 2000000000 : types[type_id_index++];
          err = append_spec(&fmt_buf, type_id, NULL, 0);
          needed_arg_count++;
          stat = 'c';
        } else {
          put_char_to_buf(&fmt_buf, ch);
          stat = 'c';
        }
        break;
      case '}':
        if (ch == '}') {
          err = put_char_to_buf(&fmt_buf, ch);
          stat = 'c';
        } else if (ch == '%') {
          stat = 'c';
          err = concat_to_buf(&fmt_buf, "%%");
        } else if (ch == '{') {
          stat = '{';
        } else {
          err = put_char_to_buf(&fmt_buf, '}');
          err = put_char_to_buf(&fmt_buf, ch);
        }
        break;
      case 's':
        if (ch == '}') {
          fmt_spec_end = fmt + i;
          int type_id =
              type_id_index >= count ? 2000000000 : types[type_id_index++];
          // It is ':', so skip it.
          fmt_spec_start++;
          int spec_len = (int)(fmt_spec_end - fmt_spec_start);
          assert(spec_len >= 0);
          err = append_spec(&fmt_buf, type_id, fmt_spec_start, spec_len);
          needed_arg_count++;
          stat = 'c';
        } else if (ch == '*') {
          int type_id =
              type_id_index >= count ? 2000000000 : types[type_id_index++];
          if (type_id != 4) err = kWrongType;
          needed_arg_count++;
        } else {
          // spec too. continue;
        }
        break;
    }
  }
  if (err == kNoError && fmt[i] == '\0') {
    switch (stat) {
      case '}':
        err = put_char_to_buf(&fmt_buf, '}');
        break;
      case 's':
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
  fmt = get_buf_as_c_string(&fmt_buf);
  // printf("format str: %s|\n", fmt);
  out_buf.size = vsnprintf(out_buf.data, out_buf.capacity, fmt, list);
  if (out_buf.size >= out_buf.capacity) {
    fprintf(stderr,
            "cfmt:%d: format result too long for fmt='%s'! "
            "needs %d bytes buf\n",
            line_no, fmt, out_buf.size);
  }
  return get_buf_as_c_string(&out_buf);
}
const char *_cfmt_format(const char *fmt, int count, int types[], ...) {
  va_list list;
  va_start(list, types);
  return cfmt(fmt, count, types, list);
}
void _cfmt_fprint(FILE *fp, const char *fmt, int count, int types[], ...) {
  va_list list;
  va_start(list, types);
  const char *output = cfmt(fmt, count, types, list);
  fputs(output, fp);
}
void cfmt_internal_test(void) {
  void *ptr = malloc(sizeof(struct string_buffer));
  struct string_buffer *buf = (struct string_buffer *)ptr;
  enum cfmt_error_code err;
  string_buffer_init(buf);
  buf->capacity = 10;
  for (int i = 0; i < 10; i++) err = put_char_to_buf(buf, '0' + i);
  assert(0 == strcmp(get_buf_as_c_string(buf), "012345678"));
  assert(err == kBufferOverflow);

  string_buffer_init(buf);
  buf->capacity = 10;
  err = concat_to_buf(buf, "123456789abcdef");
  assert(err == kBufferOverflow);
  assert(0 == strcmp(get_buf_as_c_string(buf), "123456789"));

  string_buffer_init(buf);
  buf->capacity = 10;
  err = concat_to_buf(buf, "1234567");
  assert(err == kNoError);
  err = concat_to_buf(buf, "89abcdef");
  assert(err == kBufferOverflow);
  puts(get_buf_as_c_string(buf));
  assert(0 == strcmp(get_buf_as_c_string(buf), "123456789"));

  string_buffer_init(buf);
  buf->capacity = 10;
  err = append_data_to_buf(buf, "0123abcdefg", 4);
  assert(err == kNoError);
  assert(0 == strcmp(get_buf_as_c_string(buf), "0123"));
  err = append_data_to_buf(buf, "456789abcd", 6);
  assert(err == kBufferOverflow);
  assert(0 == strcmp(get_buf_as_c_string(buf), "012345678"));

  char another_buf[10];
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
#endif
  assert(snprintf(another_buf, 10, "0123456789") >= 10);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
}
