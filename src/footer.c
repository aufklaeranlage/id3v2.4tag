#include "footer.h"

#include "utils.h"

#include <string.h>
#include <stdlib.h>

struct footer *footer_init(struct footer *h)
{
  if (h == NULL)
    return NULL;
  memset(h, 0, sizeof(*h));
  h->state = unset;
  return h;
}

struct footer *footer_new()
{
  struct footer *new = malloc(sizeof(*new));
  return footer_init(new);
}

struct footer *footer_cpy(struct footer *dest, const struct footer *src)
{
  memcpy(dest, src, sizeof(*src));
  return dest;
}

struct footer *footer_clone(const struct footer *src)
{
  struct footer *dest = malloc(sizeof(*dest));
  if (dest == NULL)
    return NULL;
  return footer_cpy(dest, src);
}

struct footer *footer_clear(struct footer *h)
{
  return footer_init(h);
}

void footer_del(struct footer *h)
{
  footer_clear(h);
  free(h);
}

b8 footer_read(FILE *stream, struct footer *h)
{
  footer_clear(h);
  char  buf[10];
  u64   cur = ftell(stream);

  h->state = unset;
  if (fread(buf, sizeof(u8), 10, stream) < 10 || strncmp(buf, "3DI", 3) != 0) {
    fseek(stream, cur, SEEK_SET);
    return false;
  }
  h->state = good;

  h->version.major = buf[3];
  h->version.minor = buf[4];

  h->flags.unsynchronisation = buf[5] & F_UNSYN;
  h->flags.extended = buf[5] & F_EXTEN;
  h->flags.experimental = buf[5] & F_EXPER;
  h->flags.footer = buf[5] & F_FOOTR;
  h->flags.uncleared = buf[5] & F_UNCLR;

  h->size = read_synchsafe_u28((u8 *)buf + 6);
  h->pos = cur;

  return true;
}

b8 footer_update(struct footer *f)
{
  if (f->state == unset)
    return false;
  return true;
}

b8 footer_cpy_header(struct footer *f, struct header *h)
{
  f->state = h->state;
  f->version.major = h->version.major;
  f->version.minor = h->version.minor;
  f->flags.unsynchronisation = h->flags.unsynchronisation;
  f->flags.extended = h->flags.extended;
  f->flags.experimental = h->flags.experimental;
  f->flags.footer = h->flags.footer;
  f->flags.uncleared = h->flags.uncleared;
  f->size = h->size;
  f->pos = h->pos + h->size - 10;
  return true;
}

FILE *footer_stream(FILE *stream, const struct footer *h)
{
  fprintf(stream, "ID3v%d.%d, size: %d", h->version.major, h->version.minor, h->size);
  return stream;
}

FILE *footer_stream_full(FILE *stream, const struct footer *h)
{
  fprintf(stream, "version { major: %d, minor: %d }, flags { unsynchronisation: %s, extended: %s, experimental: %s, footer: %s, uncleared: %s },size: %d",
      h->version.major, h->version.minor, boolstr(h->flags.unsynchronisation), boolstr(h->flags.extended), boolstr(h->flags.experimental), boolstr(h->flags.footer), boolstr(h->flags.uncleared), h->size);
  return stream;
}
