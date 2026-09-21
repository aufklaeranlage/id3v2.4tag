#ifndef ID3V240_APIC_H
# define ID3V240_APIC_H

#include "id3v240.h"
#include "frame.h"

#include <stdio.h>

struct txxx {
  //  <Header for 'User defined text information frame', ID: "TXXX">
  //  Text encoding     $xx
  //  Description       <text string according to encoding> $00 (00)
  //  Value             <text string according to encoding>
  //
  //  Multiple "TXXX" frames may be present in a tag, but they need to have
  //  unique descriptions
  enum txtenc   txtenc;
  char          *desc;
  u32           desc_len;
  char          *val;
  u32           val_len;
};

struct txxx *txxx_init(struct txxx *txxx);
struct txxx *txxx_new();
b8          txxx_cpy(struct txxx *dest, const struct txxx *src);
struct txxx *txxx_clone(struct txxx *src);
struct txxx *txxx_clear(struct txxx *txxx);
void        txxx_del(struct txxx *txxx);

b8          txxx_read(struct txxx *txxx, const struct frame *frame);
b8          txxx_write(struct frame *frame, const struct txxx *txxx);

FILE        *txxx_stream(FILE *stream, const struct txxx *txxx);

#endif
