#ifndef ID3V240_APIC_H
# define ID3V240_APIC_H

#include "id3v240.h"
#include "frame.h"

#include <stdio.h>

struct geob {
  //  <Header for 'General encapsulated object', ID: "GEOB">
  //  Text encoding          $xx
  //  MIME type              <text string> $00
  //  Filename               <text string according to encoding> $00 (00)
  //  Content description    <text string according to encoding> $00 (00)
  //  Encapsulated object    <binary data>
  enum txtenc   txtenc;
  char          *mime;
  u32           mime_len;
  char          *filename;
  u32           filename_len;
  char          *description;
  u32           description_len;
  char          *object;
  u32           object_len;
};

struct geob *geob_init(struct geob *geob);
struct geob *geob_new();
b8          geob_cpy(struct geob *dest, const struct geob *src);
struct geob *geob_clone(struct geob *src);
struct geob *geob_clear(struct geob *geob);
void        geob_del(struct geob *geob);

b8          geob_read(struct geob *geob, const struct frame *frame);
b8          geob_write(struct frame *frame, const struct geob *geob);

FILE        *geob_stream(FILE *stream, const struct geob *geob);

#endif
