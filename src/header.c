#include "header.h"

#include "utils.h"

#include <string.h>
#include <stdlib.h>

struct header *header_init(struct header *h)
{
  if (h == NULL)
    return NULL;
  memset(h, 0, sizeof(*h));
  h->state = unset;
  return h;
}

struct header *header_new()
{
  struct header *new = malloc(sizeof(*new));
  return header_init(new);
}

struct header *header_cpy(struct header *dest, const struct header *src)
{
  memcpy(dest, src, sizeof(*src));
  return dest;
}

struct header *header_clone(const struct header *src)
{
  struct header *dest = malloc(sizeof(*dest));
  if (dest == NULL)
    return NULL;
  return header_cpy(dest, src);
}

struct header *header_clear(struct header *h)
{
  return header_init(h);
}

void header_del(struct header *h)
{
  header_clear(h);
  free(h);
}

b8 header_read(struct header *h, FILE *stream)
{
  header_clear(h);
  char  buf[10];
  u64   cur = ftell(stream);

  h->state = unset;
  if (fread(buf, sizeof(u8), 10, stream) != 10 || ferror(stream) == true || strncmp(buf, "ID3", 3) != 0) {
    fseek(stream, cur, SEEK_SET);
    return false;
  }
  h->state = good;

  h->version.major = buf[3];
  h->version.minor = buf[4];

  h->flags.unsynchronisation = buf[5] & F_UNSYN ? true : false;
  h->flags.extended = buf[5] & F_EXTEN ? true : false;
  h->flags.experimental = buf[5] & F_EXPER ? true : false;
  h->flags.footer = buf[5] & F_FOOTR ? true : false;
  h->flags.uncleared = buf[5] & F_UNCLR ? true : false;

  h->size = read_synchsafe_u28((u8 *)buf + 6);
  h->pos = cur;

  return true;
}

b8 header_update(struct header *h)
{
  (void)h;
  return true;
}

b8 header_set_flags(struct header *h, u8 flags)
{
  h->flags.unsynchronisation = flags & F_UNSYN ? true : false;
  h->flags.extended = flags & F_EXTEN ? true : false;
  h->flags.experimental = flags & F_EXPER ? true : false;
  h->flags.footer = flags & F_FOOTR ? true : false;
  return true;
}

FILE *header_stream(FILE *stream, const struct header *h)
{
  fprintf(stream, "ID3v%d.%d, size: %d", h->version.major, h->version.minor, h->size);
  return stream;
}

FILE *header_stream_full(FILE *stream, const struct header *h)
{
  fprintf(stream, "version { major: %d, minor: %d }, flags { unsynchronisation: %s, extended: %s, experimental: %s, footer: %s, uncleared: %s },size: %d",
      h->version.major, h->version.minor, boolstr(h->flags.unsynchronisation), boolstr(h->flags.extended), boolstr(h->flags.experimental), boolstr(h->flags.footer), boolstr(h->flags.uncleared), h->size);
  return stream;
}
