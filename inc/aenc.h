#ifndef ID3V240_AENC_H
# define ID3V240_AENC_H

#include "id3v240.h"
#include "frame.h"

#include <stdio.h>

struct aenc {
  //  <Header for 'Audio encryption', ID: "AENC">
  //  Owner identifier   <text string> $00
  //  Preview start      $xx xx
  //  Preview length     $xx xx
  //  Encryption info    <binary data>
  char  *owner;
  u32   owner_len;
  u16   start;
  u16   length;
  char  *info;
  u32   info_len;
};

struct aenc *aenc_init(struct aenc *aenc);
struct aenc *aenc_new();
b8          aenc_cpy(struct aenc *dest, const struct aenc *src);
struct aenc *aenc_clone(struct aenc *src);
struct aenc *aenc_clear(struct aenc *aenc);
void        aenc_del(struct aenc *aenc);

b8          aenc_read(struct aenc *aenc, const struct frame *frame);
b8          aenc_write(struct frame *frame, const struct aenc *aenc);

FILE        *aenc_stream(FILE *stream, const struct aenc *aenc);

#endif
