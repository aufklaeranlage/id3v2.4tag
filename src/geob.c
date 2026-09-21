#include "geob.h"

#include <string.h>
#include <stdlib.h>

struct geob *geob_init(struct geob *geob)
{
  memset(geob, 0, sizeof(*geob));
  return geob;
}

struct geob *geob_new()
{
  struct geob *new = malloc(sizeof(*new));
  if (new == NULL)
    return NULL;
  return geob_init(new);
}

b8 geob_cpy(struct geob *dest, const struct geob *src)
{
  geob_clear(dest);
  dest->mime = calloc(sizeof(*dest->mime), src->mime_len + 1);
  dest->filename = calloc(sizeof(*dest->filename), src->filename_len);
  dest->desc = calloc(sizeof(*dest->desc), src->desc_len + 1);
  dest->object = calloc(sizeof(*dest->object), src->object_len);
  if (dest->mime == NULL || dest->filename == NULL || dest->desc == NULL || dest->object == NULL) {
    geob_clear(dest);
    return false;
  }
  dest->txtenc = src->txtenc;
  dest->mime_len = src->mime_len;
  dest->filename_len = src->filename_len;
  dest->desc_len = src->desc_len;
  dest->object_len = src->object_len;
  memcpy(dest->mime, src->mime, dest->mime_len + 1);
  memcpy(dest->filename, src->filename, dest->filename_len + 1);
  memcpy(dest->desc, src->desc, dest->desc_len + 1);
  memcpy(dest->object, src->object, dest->object_len);
  return true;
}

struct geob *geob_clone(struct geob *src)
{
  struct geob *dest = geob_new();
  if (dest == NULL)
    return NULL;
  if (!geob_cpy(dest, src)) {
    geob_del(dest);
    return NULL;
  }
  return dest;
}

struct geob *geob_clear(struct geob *geob)
{
  free(geob->mime);
  free(geob->filename);
  free(geob->desc);
  free(geob->object);
  return geob_init(geob);
}

void geob_del(struct geob *geob)
{
  geob_clear(geob);
  free(geob);
}

b8 geob_read(struct geob *geob, const struct frame *frame)
{
  geob_clear(geob);
  u32 pos = 0;
  geob->txtenc = frame->data[pos] > 3 ? 4 : frame->data[pos];
  ++pos;
  if (frame->size < pos) {
    geob_clear(geob);
    return false;
  }
  geob->mime_len = strlen(frame->data + pos);
  pos += geob->mime_len + 1;
  if (frame->size < pos) {
    geob_clear(geob);
    return false;
  }
  geob->filename_len = strlen(frame->data + pos);
  pos += geob->filename_len + 1;
  if (frame->size < pos) {
    geob_clear(geob);
    return false;
  }
  geob->desc_len = strlen(frame->data + pos);
  pos += geob->desc_len + 1;
  if (frame->size < pos) {
    geob_clear(geob);
    return false;
  }
  geob->object_len = frame->size - pos;
  geob->mime = calloc(sizeof(*geob->mime), geob->mime_len + 1);
  geob->filename = calloc(sizeof(*geob->filename), geob->filename_len + 1);
  geob->desc = calloc(sizeof(*geob->desc), geob->desc_len + 1);
  geob->object = calloc(sizeof(*geob->object), geob->object_len);
  if (geob->mime == NULL || geob->filename == NULL || geob->desc == NULL || geob->object == NULL) {
    geob_clear(geob);
    return false;
  }
  memcpy(geob->mime, frame->data + 1, geob->mime_len + 1);
  memcpy(geob->filename, frame->data + 2 + geob->mime_len, geob->filename_len + 1);
  memcpy(geob->desc, frame->data + 3 + geob->mime_len + geob->filename_len, geob->desc_len + 1);
  memcpy(geob->object, frame->data + 4 + geob->mime_len + geob->filename_len + geob->desc_len, geob->object_len);
  return true;
}

b8 geob_write(struct frame *frame, const struct geob *geob)
{
  frame_clear(frame);
  frame->size = 4 + geob->mime_len + geob->filename_len + geob->desc_len + geob->object_len;
  frame->data = calloc(sizeof(*frame->data), frame->size + 1);
  if (frame->data == NULL) {
    frame_clear(frame);
    return false;
  }
  u32 pos = 0;
  frame->data[pos++] = (u8)geob->txtenc;
  memcpy(frame->data + pos, geob->mime, geob->mime_len + 1);
  pos += geob->mime_len + 1;
  memcpy(frame->data + pos, geob->filename, geob->filename_len + 1);
  pos += geob->filename_len + 1;
  memcpy(frame->data + pos, geob->desc, geob->desc_len + 1);
  pos = geob->desc_len;
  memcpy(frame->data + pos, geob->object, geob->object_len);
  memcpy(frame->id, "GEOB", 5);
  return true;
}

FILE  *geob_stream(FILE *stream, const struct geob *geob)
{
  fprintf(stream, "GEOB: {mime: %s, filename: %s, description: %s, object: <binary>}", geob->mime, geob->filename, geob->desc);
  return stream;
}

