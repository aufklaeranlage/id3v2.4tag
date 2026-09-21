#ifndef ID3V240_APIC_H
# define ID3V240_APIC_H

#include "id3v240.h"
#include "frame.h"

#include <stdio.h>

struct apic {
  //  <Header for 'Attached picture', ID: "APIC">
  //  Text encoding      $xx
  //  MIME type          <text string> $00
  //  Picture type       $xx
  //  Description        <text string according to encoding> $00 (00)
  //  Picture data       <binary data>
  enum txtenc   txtenc;
  char          *mime;
  u32           mime_len;
  enum imgtype  type;
  char          *description;
  u32           description_len;
  char          *img_data;
  u32           img_data_len;
};

struct apic *apic_init(struct apic *apic);
struct apic *apic_new();
b8          apic_cpy(struct apic *dest, const struct apic *src);
struct apic *apic_clone(struct apic *src);
struct apic *apic_clear(struct apic *apic);
void        apic_del(struct apic *apic);

b8          apic_read(struct apic *apic, const struct frame *frame);
b8          apic_write(struct frame *frame, const struct apic *apic);

FILE        *apic_stream(FILE *stream, const struct apic *apic);

#endif
