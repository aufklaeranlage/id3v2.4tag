#include "tif.h"

#include <string.h>
#include <stdlib.h>

struct tif *tif_init(struct tif *tif)
{
  memset(tif, 0, sizeof(*tif));
  return tif;
}

struct tif *tif_new()
{
  struct tif *new = malloc(sizeof(*new));
  if (new == NULL)
    return NULL;
  return tif_init(new);
}

b8 tif_cpy(struct tif *dest, const struct tif *src)
{
  tif_clear(dest);
  dest->info = calloc(sizeof(*dest->info), src->ninfo);
  dest->info_len = calloc(sizeof(*dest->info_len), src->ninfo);
  dest->ninfo = src->ninfo;
  if (dest->info == NULL || dest->info_len == NULL) {
    tif_clear(dest);
    return false;
  }
  for (u32 i = 0; i < src->ninfo; i++) {
    dest->info[i] = calloc(sizeof(**dest->info), src->info_len[i] + 1);
    if (dest->info[i] == NULL) {
      tif_clear(dest);
      return false;
    }
    dest->info_len[i] = src->info_len[i];
    memcpy(dest->info[i], src->info[i], src->info_len[i] + 1);
  }
  memcpy(dest->id, src->id, 5);
  dest->txtenc = src->txtenc;
  return true;
}

struct tif *tif_clone(struct tif *src)
{
  struct tif *dest = tif_new();
  if (dest == NULL)
    return NULL;
  if (!tif_cpy(dest, src)) {
    tif_del(dest);
    return NULL;
  }
  return dest;
}

struct tif *tif_clear(struct tif *tif)
{
  for (u32 i = 0; i < tif->ninfo; i++)
    free(tif->info[i]);
  free(tif->info);
  free(tif->info_len);
  return tif_init(tif);
}

void tif_del(struct tif *tif)
{
  tif_clear(tif);
  free(tif);
}

b8 tif_read(struct tif *tif, const struct frame *frame)
{
  tif_clear(tif);
  u32 pos = 0;
  tif->txtenc = frame->data[pos] > 3 ? 4 : frame->data[pos];
  ++pos;
  if (frame->size < pos) {
    tif_clear(tif);
    return false;
  }
  while (pos < frame->size) {
    u32 size = 0;
    while (pos + size < frame->size && frame->data[pos + size] != 0)
      ++size;
    if (size == 0)
      break ;
    tif->info = realloc(tif->info, sizeof(*tif->info) * (tif->ninfo + 1) + 1);
    tif->info_len = realloc(tif->info_len, sizeof(*tif->info_len) * (tif->ninfo + 1));
    char *new_info_str = calloc(sizeof(*new_info_str), size + 1);
    if (tif->info == NULL || tif->info_len == NULL || new_info_str == NULL) {
      tif_clear(tif);
      free(new_info_str);
      return false;
    }
    tif->info[tif->ninfo] = new_info_str;
    tif->info_len[tif->ninfo] = size;
    memcpy(new_info_str, frame->data + pos, size);
    new_info_str[size] = 0;
  }
  memcpy(tif->id, frame->id, 5);
  return true;
}

static u32 tif_get_frame_size(const struct tif *tif)
{
  u32 size = 1;
  for (u32 i = 0; i < tif->ninfo; i++) {
    size += tif->info_len[i] + 1;
  }
  if (tif->ninfo != 0)
    size -= 1;
  return size;
}

b8 tif_write(struct frame *frame, const struct tif *tif)
{
  frame_clear(frame);
  memcpy(frame->id, tif->id, 5);
  frame->size = tif_get_frame_size(tif);
  frame->data = calloc(sizeof(*frame->data), frame->size + 1);
  if (frame->data == NULL) {
    frame_clear(frame);
    return false;
  }
  u32 pos = 0;
  frame->data[pos++] = (u8)tif->txtenc;
  for (u32 i = 0; i < tif->ninfo; i++) {
    memcpy(frame->data + pos, tif->info[i], tif->info_len[i]);
    pos += tif->info_len[i];
    if (i + 1 != tif->ninfo)
      frame->data[pos++] = 0;
  }
  return true;
}

FILE  *tif_stream(FILE *stream, const struct tif *tif)
{
  fprintf(stream, "%s: {", tif->id);
  if (tif->ninfo == 0)
    fprintf(stream, "No data");
  for (u32 i = 0; i < tif->ninfo; i++) {
    if (i != 0)
      fprintf(stream, ", ");
    fprintf(stream, "%s", tif->info[i]);
  }
  fprintf(stream, "}");
  return stream;
}

