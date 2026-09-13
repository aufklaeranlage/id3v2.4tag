#ifndef ID3V240_SPEC_FRAMES_H
# define ID3V240_SPEC_FRAMES_H

#include "id3v240.h"

#include <stdio.h>

struct fr_wrapper {
  struct frame  *frame;
  void          *spec_data;
  b8            modified;

  FILE  *(*stream)(FILE *, struct fr_wrapper *);
  FILE  *(*stream_full)(FILE *, struct fr_wrapper *);
};

struct fr_wrapper *fr_wrapper_init(struct fr_wrapper *fw);

struct fr_wrapper *fr_wrapper_new();

struct fr_wrapper *fr_wrapper_cpy(struct fr_wrapper *dest, struct fr_wrapper *src);

struct aenc {
  //  <Header for 'Audio encryption', ID: "AENC">
  //  Owner identifier   <text string> $00
  //  Preview start      $xx xx
  //  Preview length     $xx xx
  //  Encryption info    <binary data>
  char  *owner;
  u32   start;
  u32   length;
  char  *info;
  u32   *info_len;

  struct fr_wrapper *(*cpy)(struct fr_wrapper *dest, struct fr_wrapper *src);
  void              (*del)(struct fr_wrapper *fw);
};



#endif
