#ifndef ID3V240_AENC_H
# define ID3V240_AENC_H

#include "id3v240.h"
#include "frame.h"

#include <stdio.h>

struct ufid {
  //  <Header for 'Unique file identifier', ID: "UFID">
  //  Owner identifier        <text string> $00
  //  Identifier              <up to 64 bytes binary data>
  char  *owner_id;
  u32   owner_id_len;
  char  *id;
  u32   id_len;
};

struct ufid *ufid_init(struct ufid *ufid);
struct ufid *ufid_new();
b8          ufid_cpy(struct ufid *dest, const struct ufid *src);
struct ufid *ufid_clone(struct ufid *src);
struct ufid *ufid_clear(struct ufid *ufid);
void        ufid_del(struct ufid *ufid);

b8          ufid_read(struct ufid *ufid, const struct frame *frame);
b8          ufid_write(struct frame *frame, const struct ufid *ufid);

FILE        *ufid_stream(FILE *stream, const struct ufid *ufid);

#endif
