#include "ext_header.h"

#include "utils.h"

#include <string.h>
#include <stdlib.h>

struct ext_header *ext_header_init(struct ext_header *eh)
{
  if (eh == NULL)
    return NULL;
  memset(eh, 0, sizeof(*eh));
  eh->state = unset;
  eh->size = 6;
  return eh;
}

struct ext_header *ext_header_new()
{
  struct ext_header *new = malloc(sizeof(*new));
  return ext_header_init(new);
}

struct ext_header *ext_header_cpy(struct ext_header *dest, const struct ext_header *src)
{
  memcpy(dest, src, sizeof(*src));
  return dest;
}

struct ext_header *ext_header_clone(const struct ext_header *src)
{
  struct ext_header *dest = malloc(sizeof(*dest));
  if (dest == NULL)
    return NULL;
  return ext_header_cpy(dest, src);
}

struct ext_header *ext_header_clear(struct ext_header *eh)
{
  return ext_header_init(eh);
}

void ext_header_del(struct ext_header *eh)
{
  free(eh);
}

b8 ext_header_read(struct ext_header *eh, FILE *stream)
{
  char  buf[6];
  u64   cur = ftell(stream);

  eh->state = unset;
  if (fread(buf, sizeof(char), 6, stream) < 6) {
    fseek(stream, cur, SEEK_SET);
    return false;
  }

  eh->size = read_synchsafe_u28((u8 *)buf);
  if (eh->size < 6) {
    fseek(stream, cur, SEEK_SET);
    return false;
  }

  /* Ignoring number of flag bytes */

  eh->flags.update = buf[5] & F_UPDATE ? true : false;
  eh->flags.crc = buf[5] & F_CRCPRES ? true : false;
  eh->flags.restriction = buf[5] & F_RESTRICT ? true : false;
  eh->state = bad;

  if (eh->size != (u32)(6 + (eh->flags.update * 1) + (eh->flags.crc * 6) + (eh->flags.restriction * 2))) {
    fseek(stream, cur, SEEK_SET);
    return false;
  }
  if (eh->size == 6) {
    eh->state = good;
    return true;
  }

  char flagbuf[10];
  if (fread(flagbuf, sizeof(char), eh->size - 6, stream) < eh->size - 6) {
    fseek(stream, cur, SEEK_SET);
    return false;
  }

  u8  pos = 0;
  if (eh->flags.update && flagbuf[pos++] != 0) {
    fseek(stream, cur, SEEK_SET);
    return false;
  }

  if (eh->flags.crc) {
    if (flagbuf[pos++] != 5) {
      fseek(stream, cur, SEEK_SET);
      return false;
    }
    eh->crc = read_synchsafe_u35((u8 *)(flagbuf + pos));
    pos += 5;
  }

  if (eh->flags.restriction) {
    if (flagbuf[pos++] != 1) {
      fseek(stream, cur, SEEK_SET);
      return false;
    }
    eh->restriction.tag_size = flagbuf[pos] & M_RES_TAGSIZE ? true : false;
    eh->restriction.txt_encode = flagbuf[pos] & F_RES_TXTENC ? true : false;
    eh->restriction.txt_size = flagbuf[pos] & M_RES_TXTSIZE ? true : false;
    eh->restriction.img_encode = flagbuf[pos] & F_RES_IMGENC ? true : false;
    eh->restriction.img_size = flagbuf[pos] & M_RES_IMGSIZE ? true : false;
  }
  eh->pos = cur;

  eh->state = good;
  return true;
}

b8 ext_header_update(struct ext_header *eh)
{
  if (eh->state == unset) {
    ext_header_init(eh);
    eh->state = good;
  }
  eh->size = (u32)(6 + (eh->flags.update * 1) + (eh->flags.crc * 6) + (eh->flags.restriction * 2));
  return true;
}

b8 ext_header_set_update(struct ext_header *eh, b8 set)
{
  eh->size += 1 * (!(!(set)) - !(!(eh->flags.update)));
  eh->flags.update = set ? true : false;
  return true;
}

b8 ext_header_set_crc(struct ext_header *eh, b8 set, u64 val)
{
  eh->size += 6 * (!(!(set)) - !(!(eh->flags.crc)));
  eh->flags.crc = set ? true : false;
  if (!set)
    return true;
  eh->crc = val;
  return true;
}

b8 ext_header_set_restrictions(struct ext_header *eh, b8 set, u8 flags)
{
  eh->size += 2 * (!(!(set)) - !(!(eh->flags.restriction)));
  eh->flags.restriction = set ? true : false;
  if (!set)
    return true;
  eh->restriction.tag_size = flags & M_RES_TAGSIZE;
  eh->restriction.txt_encode = flags & F_RES_TXTENC ? true : false;
  eh->restriction.txt_size = flags & M_RES_TXTSIZE;
  eh->restriction.img_encode = flags & F_RES_IMGENC ? true : false;
  eh->restriction.img_size = flags & M_RES_IMGSIZE;
  return true;
}

static const char *tagresstr(enum res_tag rt) {
  static const char strs[5][12] = {
    "max 128x1m",
    "max 64x128k",
    "max 32x40k",
    "max 32x4k",
    "undefined",
  };
  if (rt > 4)
    rt = 4;
  return strs[rt];
};

static const char *txtresstr(enum res_txt rt) {
  static const char strs[5][10] = {
    "none",
    "max 1024",
    "max 128",
    "max 30",
    "undefined",
  };
  if (rt > 4)
    rt = 4;
  return strs[rt];
};

static const char *imgresstr(enum res_img rt) {
  static const char strs[5][12] = {
    "none",
    "max 256x256",
    "max 64x64",
    "exact 64x64",
    "undefined",
  };
  if (rt > 4)
    rt = 4;
  return strs[rt];
};

FILE *ext_header_stream(FILE *stream, const struct ext_header *eh)
{
  fprintf(stream, "update: %s, crc: %s, restrictions: %s",
      boolstr(eh->flags.update), boolstr(eh->flags.crc), boolstr(eh->flags.restriction));
  return stream;
}

FILE *ext_header_stream_full(FILE *stream, const struct ext_header *eh)
{
  fprintf(stream, "update: %s, crc: %s { %ld }, restrictions: %s { tag size: %s, txt encoding: %s, txt size: %s, img encoding: %s, img size: %s }",
      boolstr(eh->flags.update), boolstr(eh->flags.crc), eh->crc, boolstr(eh->flags.restriction), tagresstr(eh->restriction.tag_size), boolstr(eh->restriction.txt_encode), txtresstr(eh->restriction.txt_size), boolstr(eh->restriction.img_encode), imgresstr(eh->restriction.img_size));
  return stream;
}
