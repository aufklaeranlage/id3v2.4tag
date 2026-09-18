#include "ufid.h"

#include <string.h>
#include <stdlib.h>

struct ufid *ufid_init(struct ufid *ufid)
{
  memset(ufid, 0, sizeof(*ufid));
  return ufid;
}

struct ufid *ufid_new()
{
  struct ufid *new = malloc(sizeof(*new));
  if (new == NULL)
    return NULL;
  return ufid_init(new);
}

b8 ufid_cpy(struct ufid *dest, const struct ufid *src)
{
  ufid_clear(dest);
  dest->owner_id = calloc(sizeof(*dest->owner_id), src->owner_id_len + 1);
  dest->id = calloc(sizeof(*dest->id), src->id_len);
  if (dest->owner_id == NULL || dest->id == NULL) {
    ufid_clear(dest);
    return false;
  }
  memcpy(dest->owner_id, src->owner_id, dest->owner_id_len + 1);
  memcpy(dest->id, src->id, dest->id_len + 1);
  return true;
}

struct ufid *ufid_clone(struct ufid *src)
{
  struct ufid *dest = ufid_new();
  if (dest == NULL)
    return NULL;
  if (!ufid_cpy(dest, src)) {
    ufid_del(dest);
    return NULL;
  }
  return dest;
}

struct ufid *ufid_clear(struct ufid *ufid)
{
  free(ufid->owner_id);
  free(ufid->id);
  return ufid_init(ufid);
}

void ufid_del(struct ufid *ufid)
{
  ufid_clear(ufid);
  free(ufid);
}

b8 ufid_read(struct ufid *ufid, const struct frame *frame)
{
  ufid_clear(ufid);
  u32 pos = 0;
  ufid->owner_id_len = strlen(frame->data + pos);
  pos += ufid->owner_id_len + 1;
  if (frame->size < pos + 64) {
    ufid_clear(ufid);
    return false;
  }
  ufid->id_len = frame->size - pos;
  ufid->owner_id = calloc(sizeof(*ufid->owner_id), ufid->owner_id_len + 1);
  ufid->id = calloc(sizeof(*ufid->id), ufid->id_len);
  if (ufid->owner_id == NULL || ufid->id == NULL) {
    ufid_clear(ufid);
    return false;
  }
  memcpy(ufid->owner_id, frame->data, ufid->owner_id_len + 1);
  memcpy(ufid->id, frame->data + 1 + ufid->owner_id_len, ufid->id_len);
  return true;
}

b8 ufid_write(struct frame *frame, const struct ufid *ufid)
{
  frame_clear(frame);
  frame->size = 1 + ufid->owner_id_len + ufid->id_len;
  frame->data = calloc(sizeof(*frame->data), frame->size + 1);
  if (frame->data == NULL) {
    frame_clear(frame);
    return false;
  }
  u32 pos = 0;
  memcpy(frame->data + pos, ufid->owner_id, ufid->owner_id_len + 1);
  pos += ufid->owner_id_len + 1;
  memcpy(frame->data + pos, ufid->id, ufid->id_len);
  return true;
}

FILE  *ufid_stream(FILE *stream, const struct ufid *ufid)
{
  fprintf(stream, "ufid: {owner id: %s, img data: <binary>}", ufid->owner_id);
  return stream;
}

