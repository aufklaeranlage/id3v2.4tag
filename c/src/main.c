#include "file.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

b8  get_lcurly(const char *json, u32 *pos)
{
  u32 i = *pos;
  while (isspace(json[i]))
    ++i;
  if (json[i] == '{') {
    *pos = ++i;
    return true;
  }
  return false;
}

b8  get_rcurly(const char *json, u32 *pos)
{
  u32 i = *pos;
  while (isspace(json[i]))
    ++i;
  if (json[i] == '}') {
    *pos = ++i;
    return true;
  }
  return false;
}

b8  get_colon(const char *json, u32 *pos)
{
  u32 i = *pos;
  while (isspace(json[i]))
    ++i;
  if (json[i] == ':') {
    *pos = ++i;
    return true;
  }
  return false;
}

b8  get_comma(const char *json, u32 *pos)
{
  u32 i = *pos;
  while (isspace(json[i]))
    ++i;
  if (json[i] == ',') {
    *pos = ++i;
    return true;
  }
  return false;
}

b8  get_string(const char *json, u32 *pos, char **str, u32 *size)
{
  u32 i = *pos;
  while (isspace(json[i]))
    ++i;
  u32 start = 0;
  if (json[i] == '"') {
    ++i;
    start = i;
    b8  esc = false;
    while (json[i] != '\0' && (esc == true || json[i] != '"')) {
      esc = false;
      if (json[i] == '\\')
        esc = true;
      ++i;
    }
    if (json[i] == '"') {
      *str = (char *)json + start;
      *size = i - start;
      *pos = ++i;
      return true;
    }
  }
  return false;
}

b8  get_bool(const char *json, u32 *pos, b8 *ret)
{
  u32 i = *pos;
  while (isspace(json[i]))
    ++i;
  if (memcmp(json + i, "True", 4) == 0) {
    *ret = true;
    *pos += 5;
  } else if (memcmp(json + i, "False", 5) == 0) {
    *ret = false;
    *pos += 6;
  } else {
    return false;
  }
  return true;
}

b8  get_u32(const char *json, u32 *pos, u32 *ret)
{
  u32 i = *pos;
  while (isspace(json[i]))
    ++i;
  char  *endptr;
  u64 retval = strtol(json + i, &endptr, 10);
  if (!isspace(*endptr))
    return false;
  i = endptr - json;
  *pos = i;
  *ret = (u32)retval;
  return true;
}

u32 get_unescaped_size(const char *str)
{
  u32 size = 0;
  for (u32 i = 0; str[i] != '\0'; i++) {
    if (str[i] == '\\') {
      switch (str[++i]) {
        case '0':
          if (isdigit(str[i + 1]) && isdigit(str[i + 1]))
            i += 2;
          break ;
        case 'x':
          ++i;
          u32 hexdec_chars = 0;
          while (hexdec_chars < 16 && strchr("0123456789ABCDEF", toupper(str[i]) != 0)) {
              ++i;
              ++hexdec_chars;
          }
          --i;
          break;
      }
    }
    ++size;
  }
  return size;
}

b8  make_unescaped_str(const char *str, char **ret, u32 *size)
{
  u32 ret_size = get_unescaped_size(str);
  char  *ret_str = malloc(sizeof(*ret_str) * (ret_size + 1));
  if (ret_str == NULL)
    return false;
  char  base[] = "0123456789ABCDEF";
  u32 pos = 0;
  for (u32 i = 0; str[i] != '\0'; i++) {
    if (str[i] == '\\') {
      switch (str[++i]) {
        case '0':
          if (isdigit(str[i + 1]) && isdigit(str[i + 2])) {
            ret_str[pos] = (str[i + 1] - '0') * 8 + (str[i + 2] - '0');
            i += 2;
          } else {
            ret_str[pos] = '\0';
          }
          break ;
        case 'x':
          ++i;
          u32 hexdec_chars = 0;
          u64 val = 0;
          char  *strchr_ret = strchr(base, toupper(str[i]));
          while (hexdec_chars < 16 && strchr_ret != 0) {
            val *= 16;
            val += strchr_ret - base;
            ++i;
            ++hexdec_chars;
            strchr_ret = strchr(base, toupper(str[i]));
          }
          --i;
          for (i32 j = hexdec_chars / 2; j >= 0; j--) {
            ret_str[pos + j] = val % 256;
            val /= 256;
          }
          pos += hexdec_chars / 2;
          break;
        case 't':
          ret_str[pos] = '\t';
          break ;
        case 'n':
          ret_str[pos] = '\n';
          break ;
        default:
          ret_str[pos] = str[i];
      }
    } else {
      ret_str[pos] = str[i];
    }
    ++pos;
  }
  *size = ret_size;
  *ret = ret_str;
  return true;
}

struct frame *frame_from_json(const char *json)
{
  struct frame  *frame = frame_new();
  if (frame == NULL)
    return NULL;
  struct {
    b8  id;
    b8  preserve_tag;
    b8  preserve_file;
    b8  read_only;
    b8  grouped;
    b8  compressed;
    b8  encrypted;
    b8  unsynchronisation;
    b8  length_indicated;
    b8  data;
    b8  size;
  } set;
  memset(&set, 0, sizeof(set));
  u32 pos = 0;
  if (!get_lcurly(json, &pos))
    goto free_failure;
  while (json[pos] != '\0') {
    char  *var, *val;
    u32   varsize, valsize;
    u32   numret;
    b8    boolret;
    if (!get_string(json, &pos, &var, &varsize))
      goto free_failure;
    if (!get_colon(json, &pos))
      goto free_failure;
    if (memcmp(var, "id", varsize) == 0) {
      if (set.id == true || !get_string(json, &pos, &val, &valsize))
        goto free_failure;
      if (valsize != 4)
        goto free_failure;
      set.id = true;
      memcpy(frame->id, val, 4);
      frame->id[4] = '\0';
    } else if (memcmp(var, "size", varsize) == 0) {
      if (set.size == true || !get_u32(json, &pos, &numret))
        goto free_failure;
      set.size = true;
      frame->size = numret;
    } else if (memcmp(var, "preserve_tag", varsize) == 0) {
      if (set.preserve_tag == true || !get_bool(json, &pos, &boolret))
        goto free_failure;
      set.preserve_tag = true;
      frame->status.preserve_tag = boolret;
    } else if (memcmp(var, "preserve_file", varsize) == 0) {
      if (set.preserve_file == true || !get_bool(json, &pos, &boolret))
        goto free_failure;
      set.preserve_file = true;
      frame->status.preserve_file = boolret;
    } else if (memcmp(var, "read_only", varsize) == 0) {
      if (set.read_only == true || !get_bool(json, &pos, &boolret))
        goto free_failure;
      set.read_only = true;
      frame->status.read_only = boolret;
    } else if (memcmp(var, "grouped", varsize) == 0) {
      if (set.grouped == true || !get_bool(json, &pos, &boolret))
        goto free_failure;
      set.grouped = true;
      frame->format.grouped = boolret;
    } else if (memcmp(var, "compressed", varsize) == 0) {
      if (set.compressed == true || !get_bool(json, &pos, &boolret))
        goto free_failure;
      set.compressed = true;
      frame->format.compressed = boolret;
    } else if (memcmp(var, "encrypted", varsize) == 0) {
      if (set.encrypted == true || !get_bool(json, &pos, &boolret))
        goto free_failure;
      set.encrypted = true;
      frame->format.encrypted = boolret;
    } else if (memcmp(var, "unsynchronisation", varsize) == 0) {
      if (set.unsynchronisation == true || !get_bool(json, &pos, &boolret))
        goto free_failure;
      set.unsynchronisation = true;
      frame->format.unsynchronisation = boolret;
    } else if (memcmp(var, "length_inidcated", varsize) == 0) {
      if (set.length_indicated == true || !get_bool(json, &pos, &boolret))
        goto free_failure;
      set.length_indicated = true;
      frame->format.length_indicated = boolret;
    } else if (memcmp(var, "data", varsize) == 0) {
      if (set.read_only == true || !get_string(json, &pos, &val, &valsize))
        goto free_failure;
      set.data = true;
      char  *data = calloc(sizeof(*data), valsize + 1);
      if (data == NULL)
        goto free_failure;
      memcpy(data, val, valsize);
      data[valsize] = 0;
      u32 size;
      if (!make_unescaped_str(data, &frame->data, &size))
        goto free_failure;
      if (set.size) {
        if (size > frame->size) {
          goto free_failure;
        } else if (size < frame->size) {
          char *new_data = calloc(sizeof(*new_data), size + 1);
          if (new_data == NULL)
            goto free_failure;
          memcpy(new_data, frame->data, frame->size);
          free(frame->data);
          frame->data = new_data;
          frame->size = size;
        }
      }
    } else {
      goto free_failure;
    }
    if (get_rcurly(json, &pos))
      return frame;
    if (!get_comma(json, &pos))
      goto free_failure;
  }

  if (!set.data || !set.id)
    goto free_failure;

free_failure:
  frame_del(frame);
  return NULL;
}

int main(int ac, char *av[])
{
  char  *filename;
  if (ac < 2)
    return 1;
  filename = av[1];
  struct file *f = NULL;
  b8  fshort = true;
  char  id[4];
  b8  search = false;
  struct frame **to_add = NULL;
  u32 nto_add = 0;
  for (i32 i = 2; i < ac; i++) {
    if (av[i][0] == '-') {
      switch (av[i][1]) {
        case 'l': fshort = false;
                  break;
        case 's': fshort = true; break;

        case '-': {
          if (memcmp(av[i] + 2, "search", 6) == 0) {
            ++i;
            if (strlen(av[i]) != 4) {
              fprintf(stderr, "%s: --search: id needs to be exactly 4 characters long.\n", av[0]);
              return 1;
            }
            memcpy(id, av[i], 4);
            search = true;
            break ;
          } else if (memcmp(av[i] + 2, "add", 3) == 0) {
            ++i;
            struct frame *frame = frame_from_json(av[i]);
            if (frame == NULL) {
              fprintf(stderr, "%s: --add: invalid JSON passed.\n", av[0]);
              goto exit;
            }
            struct frame  **new_to_add = realloc(to_add, sizeof(*to_add) * ++nto_add);
            if (new_to_add == NULL) {
              frame_del(frame);
              fprintf(stderr, "%s: Couldn't allocate memory for frame to add.\n", av[0]);
              goto exit;
            }
            to_add = new_to_add;
            to_add[nto_add - 1] = frame;
            break ;
          }
        }
      }
    }
  }
  f = file_new();
  if (file_read(f, filename) == false) {
    fprintf(stderr, "File '%s' was not readable\n", av[1]);
    return 1;
  }
  
  if (nto_add != 0) {
    for (u32 i = 0; i < nto_add; i++) {
      if (!file_add_frame(f, to_add[i], 0)) {
        fprintf(stderr, "%s: Couldn't add frame to file.\n", av[0]);
        goto exit;
      }
    }
    file_save(f);
  }

  if (search == true) {
    struct frame  **results = file_get_frames_by_id(f, id);
    if (results == NULL) {
      fprintf(stderr, "%s: error while searching for frames.\n", av[0]);
      goto  exit;
    }
    for (u32 i = 0; results[i] != NULL; i++) {
      if (fshort)
        frame_stream(stdout, results[i]);
      else
        frame_stream_full(stdout, results[i]);
      fprintf(stdout, "\n");
    }
  } else {
    if (fshort)
      file_stream(stdout, f);
    else
      file_stream_full(stdout, f);
  }

exit:
  if (to_add != NULL) {
    for (u32 i = 0; i < nto_add; i++) {
      frame_del(to_add[i]);
    }
    free(to_add);
  }
  if (f != NULL)
    file_del(f);
  return 0;
}
