#include "cfmt.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
struct string_buffer {
  int size;
  int capacity;
  char data[60000];
};
static void string_buffer_init(struct string_buffer *buf) {
  buf->capacity = sizeof(buf->data);
  buf->size = 0;
}
static void put_char_to_buf(struct string_buffer *buf, char ch) {
  if (buf->size + 1 >= buf->capacity) return;
  buf->data[buf->size++] = ch;
  assert(buf->size + 1 <= buf->capacity);
  buf->data[buf->size] = '\0';
}
// clang-format off
static void append_data_to_buf(struct string_buffer *buf,
                               const char *data, int len) {
  if (buf->size + len + 1 >= buf->capacity)
    len = buf->capacity - buf->size - 1;
  // clang-format on
  assert(buf->size + len + 1 <= buf->capacity);
  memcpy(buf->data + buf->size, data, len);
  buf->size += len;
  buf->data[buf->size] = '\0';
}
static void concat_to_buf(struct string_buffer *buf, const char *str) {
  append_data_to_buf(buf, str, strlen(str));
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
    default: return 'd';
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
    default: return "";
  }
  // clang-format on
}
static void append_spec(struct string_buffer *buf, int type_id,
                        const char *spec, int spec_len) {
  put_char_to_buf(buf, '%');
  if (spec_len == 0 || spec == NULL) {
    concat_to_buf(buf, length_modifier(type_id));
    put_char_to_buf(buf, conversion_specifier(type_id));
    return;
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
      break;
  }
  // clang-format on
  append_data_to_buf(buf, spec, spec_len);
  concat_to_buf(buf, length_modifier(type_id));
  put_char_to_buf(buf, conv);
}
static struct string_buffer fmt_buf, out_buf;
const char *cfmt(const char *fmt, int count, int types[], va_list list) {
  char stat = 'c';
  const char *fmt_spec_start = NULL;
  const char *fmt_spec_end = NULL;
  int type_id_index = 0;
  string_buffer_init(&fmt_buf);
  string_buffer_init(&out_buf);
  // https://fmt.dev/12.0/syntax/
  for (int i = 0; fmt[i] != '\0'; i++) {
    char ch = fmt[i];
    switch (stat) {
      case 'c':
        if (ch == '{') {
          stat = '{';
        } else if (ch == '}') {
          stat = '{';
        } else if (ch == '%') {
          concat_to_buf(&fmt_buf, "%%");
          // stat = 'c';
        } else {
          put_char_to_buf(&fmt_buf, ch);
          // stat = 'c';
        }
        break;
      case '{':
        if (ch == '{') {
          stat = 'c';
          put_char_to_buf(&fmt_buf, '{');
        } else if (ch == ':') {
          fmt_spec_start = fmt + i;
          stat = 's';
        } else if (ch == '}') {
          int type_id = types[type_id_index++];
          append_spec(&fmt_buf, type_id, NULL, 0);
          stat = 'c';
        } else {
          put_char_to_buf(&fmt_buf, ch);
          stat = 'c';
        }
        break;
      case '}':
        if (ch == '}') {
          put_char_to_buf(&fmt_buf, ch);
          stat = 'c';
        } else {
          put_char_to_buf(&fmt_buf, '}');
          put_char_to_buf(&fmt_buf, ch);
        }
        break;
      case 's':
        if (ch == '}') {
          fmt_spec_end = fmt + i;
          int type_id = types[type_id_index++];
          // It is ':', so skip it.
          fmt_spec_start++;
          int spec_len = (int)(fmt_spec_end - fmt_spec_start);
          assert(spec_len >= 0);
          append_spec(&fmt_buf, type_id, fmt_spec_start, spec_len);
          stat = 'c';
        } else {
          // spec too. continue;
        }
        break;
    }
  }
  fmt = get_buf_as_c_string(&fmt_buf);
  // printf("format str: %s|", fmt);
  out_buf.size = vsnprintf(out_buf.data, out_buf.capacity, fmt, list);
  return get_buf_as_c_string(&out_buf);
}
void _cfmt_println(const char *fmt, int count, int types[], ...) {
  va_list list;
  va_start(list, types);
  puts(cfmt(fmt, count, types + 1, list));
}
const char *_cfmt_format(const char *fmt, int count, int types[], ...) {
  va_list list;
  va_start(list, types);
  return cfmt(fmt, count, types + 1, list);
}
void _cfmt_fprint(FILE *fp, const char *fmt, int count, int types[], ...) {
  va_list list;
  va_start(list, types);
  const char *output = cfmt(fmt, count, types + 1, list);
  fputs(output, fp);
}
