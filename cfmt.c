#include "cfmt.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
struct string_buffer {
  int size;
  int capacity;
  char data[60000];
};
static void string_buffer_init(struct string_buffer *buf) {
  buf->capacity = sizeof(buf->data);
  buf->size = 0;
}
static void put_to_buf(struct string_buffer *buf, char ch) {
  if (buf->size >= buf->capacity) abort();
  buf->data[buf->size++] = ch;
}
static void concat_to_buf(struct string_buffer *buf, const char *str) {
  strncpy(buf->data + buf->size, str, buf->capacity - buf->size);
  buf->size += strlen(str);
}
static char *get_buf_as_c_string(struct string_buffer *buf) {
  buf->data[buf->size] = '\0';
  return buf->data;
}
static const char *type_id_to_fmt(int type_id) {
  switch (type_id) {
    case 0u: return "%c";
    case 1u: return "%c";
    case 2u: return "%hu";
    case 3u: return "%hu";
    case 4u: return "%d";
    case 5u: return "%u";
    case 6u: return "%ld";
    case 7u: return "%lu";
    case 8u: return "%lld";
    case 9u: return "%llu";
    case 10: return "%f";
    case 11: return "%lf";

    case 50:
    case 51: return "%s";
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57: return "%#p";
    default: return NULL;
  }
}
static _Thread_local struct string_buffer fmt_buf, out_buf;
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
        } else {
          put_to_buf(&fmt_buf, ch);
          // stat = 'c';
        }
        break;
      case '{':
        if (ch == '{') {
          stat = 'c';
          put_to_buf(&fmt_buf, '{');
        } else if (ch == ':') {
          fmt_spec_start = fmt + i;
          stat = 's';
        } else if (ch == '}') {
          int type_id = types[type_id_index++];
          concat_to_buf(&fmt_buf, type_id_to_fmt(type_id));
          stat = 'c';
        } else {
          put_to_buf(&fmt_buf, ch);
          stat = 'c';
        }
        break;
      case '}':
        if (ch == '}') {
          put_to_buf(&fmt_buf, ch);
          stat = 'c';
        } else {
          put_to_buf(&fmt_buf, '}');
          put_to_buf(&fmt_buf, ch);
        }
        break;
      case 's':
        if (ch == '}') {
          fmt_spec_end = fmt + i;
          // using spec to format;
          int type_id = types[type_id_index++];
          concat_to_buf(&fmt_buf, type_id_to_fmt(type_id));
          stat = 'c';
        } else {
          // spec too. continue;
        }
        break;
    }
  }
  fmt = get_buf_as_c_string(&fmt_buf);
  out_buf.size = vsnprintf(out_buf.data, out_buf.capacity, fmt, list);
  return get_buf_as_c_string(&out_buf);
}
void _cfmt_println(const char *fmt, int count, int types[], ...) {
  va_list list;
  va_start(list, types);
  puts(cfmt(fmt, count, types, list));
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
