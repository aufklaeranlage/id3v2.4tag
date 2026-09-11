#ifndef ID3V240_EXT_HEADER_H
# define ID3V240_EXT_HEADER_H

#include "id3v240.h"

#include <stdio.h>

#define F_UPDATE 64   // 0b01000000
#define F_CRCPRES 32  // 0b00100000
#define F_RESTRICT 16 // 0b00010000

#define M_RES_TAGSIZE 192 // 0b11000000
#define F_RES_TAG1M 0     // 0b00000000
#define F_RES_TAG128K 64  // 0b01000000
#define F_RES_TAG40K 128  // 0b10000000
#define F_RES_TAG4K 192   // 0b11000000

#define F_RES_TXTENC 32   // 0b00100000

#define M_RES_TXTSIZE 24  // 0b00011000
#define F_RES_TXTNONE 0   // 0b00000000
#define F_RES_TXT1024 64  // 0b01000000
#define F_RES_TXT128 128  // 0b10000000
#define F_RES_TXT30 192   // 0b11000000

#define F_RES_IMGENC 4    // 0b00000100

#define M_RES_IMGSIZE 3   // 0b00000011
#define F_RES_IMGNONE 0   // 0b00000000
#define F_RES_IMG256 64   // 0b01000000
#define F_RES_IMG64 128   // 0b10000000
#define F_RES_IMGEX64 192 // 0b11000000

enum res_tag {
  max128x1m,
  max64x128k,
  max32x40k,
  max32x4k,
};

enum res_txt {
  none_txt,
  max1024,
  max128,
  max30,
};

enum res_img {
  none_img,
  max256,
  max64,
  exact64,
};

struct ext_header {
  enum state  state;
  u32   size;
  struct {
    b8  update;
    b8  crc;
    b8 restriction;
  } flags;
  u64   crc;
  struct {
    enum res_tag  tag_size;
    b8            txt_encode;
    enum res_txt  txt_size;
    b8            img_encode;
    enum res_img  img_size;
  } restriction;
};

#endif
