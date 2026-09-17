#include "aenc.h"

#include <string.h>
#include <stdlib.h>

struct aenc *aenc_init(struct aenc *aenc)
{
  memset(aenc, 0, sizeof(*aenc));
  return aenc;
}

struct aenc *aenc_new()
{
  struct aenc *new = malloc(sizeof(*new));
  if (new == NULL)
    return NULL;
  return aenc_init(new);
}

b8 aenc_cpy(struct aenc *dest, const struct aenc *src)
{
  aenc_clear(dest);
  dest->owner = calloc(sizeof(*dest->owner), src->owner_len + 1);
  dest->info = calloc(sizeof(*dest->info), src->info_len);
  if (dest->owner == false || dest->info == false) {
    aenc_clear(dest);
    return false;
  }
  dest->owner_len = src->owner_len;
  dest->start = src->start;
  dest->length = src->length;
  dest->info_len = src->info_len;
  memcpy(dest->owner, src->owner, dest->owner_len + 1);
  memcpy(dest->info, src->info, dest->info_len + 1);
  return true;
}

struct aenc *aenc_clone(struct aenc *src)
{
  struct aenc *dest = aenc_new();
  if (dest == NULL)
    return NULL;
  if (!aenc_cpy(dest, src)) {
    aenc_del(dest);
    return NULL;
  }
  return dest;
}

struct aenc *aenc_clear(struct aenc *aenc)
{
  free(aenc->owner);
  free(aenc->info);
  return aenc_init(aenc);
}

void aenc_del(struct aenc *aenc)
{
  aenc_clear(aenc);
  free(aenc);
}

b8 aenc_read(struct aenc *aenc, const struct frame *frame)
{
  aenc_clear(aenc);
  aenc->owner_len = strlen(frame->data);
  if (aenc->owner_len < frame->size - 5)
    return false;
  aenc->info_len = frame->size - (aenc->owner_len + 4);
  aenc->owner = calloc(sizeof(*aenc->owner), aenc->owner_len + 1);
  aenc->info = calloc(sizeof(*aenc->info), aenc->info_len);
  if (aenc->owner == NULL || aenc->info == NULL) {
    aenc_clear(aenc);
    return false;
  }
  aenc->start = (u16)*(frame->data + aenc->owner_len + 1);
  aenc->length = (u16)*(frame->data + aenc->owner_len + 3);
  return true;
}

b8 aenc_write(struct frame *frame, const struct aenc *aenc)
{
  frame_clear(frame);
  frame->size = aenc->owner_len + 5 + aenc->info_len;
  frame->data = calloc(sizeof(*frame->data), frame->size + 1);
  if (frame->data == NULL) {
    frame_clear(frame);
    return false;
  }
  memcpy(frame->data, aenc->owner, aenc->owner_len);
  *(u16 *)(frame->data + aenc->owner_len + 1) = aenc->start;
  *(u16 *)(frame->data + aenc->owner_len + 3) = aenc->length;
  memcpy(frame->data + aenc->owner_len + 5, aenc->info, aenc->info_len);
  return true;
}

FILE  *aenc_stream(FILE *stream, const struct aenc *aenc)
{
  fprintf(stream, "AENC: {owner: %s, start: %u, length: %u, info: <binary>}", aenc->owner, aenc->start, aenc->length);
  return stream;
}

