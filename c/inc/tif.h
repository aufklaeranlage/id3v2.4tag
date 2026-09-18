#ifndef ID3V240_APIC_H
# define ID3V240_APIC_H

#include "id3v240.h"
#include "frame.h"

#include <stdio.h>

struct tif {
  //  <Header for 'Text information frame', ID: "T000" - "TZZZ",
  //  excluding "tif" described in 4.2.6.>
  //  Text encoding                $xx
  //  Information                  <text string(s) according to encoding>
  char          id[5];
  enum txtenc   txtenc;
  char          **info;
  u32           *info_len;
  u32           ninfo;
};

struct tif *tif_init(struct tif *tif);
struct tif *tif_new();
b8          tif_cpy(struct tif *dest, const struct tif *src);
struct tif *tif_clone(struct tif *src);
struct tif *tif_clear(struct tif *tif);
void        tif_del(struct tif *tif);

b8          tif_read(struct tif *tif, const struct frame *frame);
b8          tif_write(struct frame *frame, const struct tif *tif);

FILE        *tif_stream(FILE *stream, const struct tif *tif);

#endif
