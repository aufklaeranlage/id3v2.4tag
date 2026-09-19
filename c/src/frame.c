#include "frame.h"

#include <stdlib.h>
#include <string.h>

#include "utils.h"

struct frame *init_frame(struct frame *f)
{
  if (f == NULL)
    return f;
  f->state = unset;
  memset(f->id, '\0', 5);
  f->data = NULL;
  f->size = 0;
  f->size_synchunsafe = false;
  f->pos = 0;
  return f;
}

struct frame *frame_new()
{
  struct frame  *f = malloc(sizeof(*f));
  return init_frame(f);
}

struct frame *frame_cpy(struct frame *dest, const struct frame *src)
{
  char  *data;
  data = malloc(sizeof(dest->data) * (src->size + 1));
  if (data == NULL)
    return NULL;
  memcpy(data, src->data, src->size + 1);
  frame_clear(dest);
  memcpy(dest, src, sizeof(*src));
  dest->data = data;
  return dest;
}

struct frame *frame_clone(const struct frame *src)
{
  struct frame  *dest;
  dest = malloc(sizeof(*dest));
  if (dest == NULL)
    return NULL;
  dest->data = NULL;
  if (frame_cpy(dest, src) == NULL) {
    free(dest);
    return NULL;
  }
  return dest;
}

struct frame *frame_clear(struct frame *f)
{
  free(f->data);
  return init_frame(f);
}

void frame_del(struct frame *f)
{
  frame_clear(f);
  free(f);
}

b8 frame_read(struct frame *f, FILE *stream)
{
  frame_clear(f);
  char headerbuf[6];

  u64 cur = ftell(stream);

  f->state = bad;
  f->pos = cur;
  if (fread(f->id, sizeof(char), 4, stream) != 4 || ferror(stream)) {
    fclose(stream);
    return false;
  }
  if (fread(headerbuf, sizeof(char), 6, stream) != 6 || ferror(stream))
    return false;
  if (is_synchsafe_u28((u8 *)headerbuf)) {
    f->size = read_synchsafe_u28((u8 *)headerbuf);
  } else {
    f->size_synchunsafe = true;
    f->size = read_synchunsafe_u32((u8 *)headerbuf);
  }

  f->status.preserve_tag = headerbuf[4] & F_TAGPRES;
  f->status.preserve_file = headerbuf[4] & F_FILPRES;
  f->status.read_only = headerbuf[4] & F_RDONLY;

  f->format.grouped = headerbuf[5] & F_GROUPED;
  f->format.compressed = headerbuf[5] & F_COMPRES;
  f->format.encrypted = headerbuf[5] & F_ENCRYPT;
  f->format.unsynchronisation = headerbuf[5] & F_UNSYNC;
  f->format.length_indicated = headerbuf[5] & F_LENINDI;

  f->data = malloc(sizeof(*f->data) * (f->size + 1));
  if (f->data == NULL) {
    fseek(stream, cur, SEEK_SET);
  } else {
    if (fread(f->data, sizeof(char), f->size, stream) != f->size || ferror(stream))
      return false;
  }
  f->data[f->size] = '\0';
  f->state = good;
  return true;
}

FILE *frame_stream(FILE *stream, const struct frame *f)
{
  fprintf(stream, "id: %4s, data: %s", f->id, f->data);
  return stream;
}

FILE *frame_stream_full(FILE *stream, const struct frame *f)
{
  fprintf(stream, "id: %4s, pos: %u, size: %u, \
      status { preserve tag: %s, preserve file: %s, read only: %s }, \
      format { grouped: %s, compressed: %s, encrypted: %s, unsynchronisation: %s, length indicated: %s }, \
      data: %s",
      f->id, f->pos, f->size,
      boolstr(f->status.preserve_tag), boolstr(f->status.preserve_file), boolstr(f->status.read_only),
      boolstr(f->format.grouped), boolstr(f->format.compressed), boolstr(f->format.encrypted), boolstr(f->format.unsynchronisation), boolstr(f->format.length_indicated),
      f->data);
  return stream;
}
