#include "apic.h"

#include <string.h>
#include <stdlib.h>

static const char *imgtype_str(enum imgtype it)
{
  static const char imgtype_strs[24][36] = {
    "Other",
    "32x32 pixels 'file icon' (PNG only)",
    "Other file icon",
    "Cover (front)",
    "Cover (back)",
    "Leaflet page",
    "Media (e.g. label side of CD)",
    "Lead artist/lead performer/soloist",
    "Artist/performer",
    "Conductor",
    "Band/Orchestra",
    "Composer",
    "Lyricist/text writer",
    "Recording Location",
    "During recording",
    "During performance",
    "Movie/video screen capture",
    "A bright coloured fish",
    "Illustration",
    "Band/artist logotype",
    "Publisher/Studio logotype",
  };
  if (it < 0 || it > 21)
    return NULL;
  return imgtype_strs[it];
}

struct apic *apic_init(struct apic *apic)
{
  memset(apic, 0, sizeof(*apic));
  return apic;
}

struct apic *apic_new()
{
  struct apic *new = malloc(sizeof(*new));
  if (new == NULL)
    return NULL;
  return apic_init(new);
}

b8 apic_cpy(struct apic *dest, const struct apic *src)
{
  apic_clear(dest);
  dest->mime = calloc(sizeof(*dest->mime), src->mime_len + 1);
  dest->description = calloc(sizeof(*dest->description), src->description_len + 1);
  dest->img_data = calloc(sizeof(*dest->img_data), src->img_data_len);
  if (dest->mime == NULL || dest->description == NULL || dest->img_data == NULL) {
    apic_clear(dest);
    return false;
  }
  memcpy(dest->mime, src->mime, dest->mime_len + 1);
  memcpy(dest->description, src->description, dest->description_len + 1);
  memcpy(dest->img_data, src->img_data, dest->img_data_len);
  dest->txtenc = src->txtenc;
  dest->mime_len = src->mime_len;
  dest->type = src->type;
  dest->description_len = src->description_len;
  dest->img_data_len = src->img_data_len;
  return true;
}

struct apic *apic_clone(struct apic *src)
{
  struct apic *dest = apic_new();
  if (dest == NULL)
    return NULL;
  if (!apic_cpy(dest, src)) {
    apic_del(dest);
    return NULL;
  }
  return dest;
}

struct apic *apic_clear(struct apic *apic)
{
  free(apic->mime);
  free(apic->description);
  free(apic->img_data);
  return apic_init(apic);
}

void apic_del(struct apic *apic)
{
  apic_clear(apic);
  free(apic);
}

b8 apic_read(struct apic *apic, const struct frame *frame)
{
  apic_clear(apic);
  u32 pos = 0;
  apic->txtenc = frame->data[pos] > 3 ? 4 : frame->data[pos];
  ++pos;
  if (frame->size < pos) {
    apic_clear(apic);
    return false;
  }
  apic->mime_len = strlen(frame->data + pos);
  pos += apic->mime_len + 1;
  if (frame->size < pos) {
    apic_clear(apic);
    return false;
  }
  apic->type = frame->data[pos] > 21 ? 22 : frame->data[pos];
  ++pos;
  if (frame->size < pos) {
    apic_clear(apic);
    return false;
  }
  apic->description_len = strlen(frame->data + pos);
  pos += apic->description_len + 1;
  apic->img_data_len = frame->size - pos;
  apic->mime = calloc(sizeof(*apic->mime), apic->mime_len + 1);
  apic->description = calloc(sizeof(*apic->description), apic->description_len + 1);
  apic->img_data = calloc(sizeof(*apic->img_data), apic->img_data_len);
  if (apic->mime == NULL || apic->description == NULL || apic->img_data == NULL) {
    apic_clear(apic);
    return false;
  }
  memcpy(apic->mime, frame->data + 1, apic->mime_len);
  memcpy(apic->description, frame->data + 3 + apic->mime_len, apic->description_len);
  memcpy(apic->img_data, frame->data + 4 + apic->mime_len + apic->img_data_len, apic->img_data_len);
  return true;
}

b8 apic_write(struct frame *frame, const struct apic *apic)
{
  frame_clear(frame);
  frame->size = 4 + apic->mime_len + apic->description_len + apic->img_data_len;
  frame->data = calloc(sizeof(*frame->data), frame->size + 1);
  if (frame->data == NULL) {
    frame_clear(frame);
    return false;
  }
  u32 pos = 0;
  frame->data[pos++] = (u8)apic->txtenc;
  memcpy(frame->data + pos, apic->mime, apic->mime_len);
  pos += apic->mime_len + 1;
  frame->data[pos++] = (u8)apic->type;
  memcpy(frame->data + pos, apic->description, apic->description_len);
  pos += apic->description_len;
  memcpy(frame->data + pos, apic->img_data, apic->img_data_len);
  return true;
}

FILE  *apic_stream(FILE *stream, const struct apic *apic)
{
  fprintf(stream, "apic: {mime: %s, type: %s, description: %s, img data: <binary>}", apic->mime, imgtype_str(apic->type), apic->description);
  return stream;
}

