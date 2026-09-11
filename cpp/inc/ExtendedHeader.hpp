#pragma once

#include "id3v2.hpp"

#include <stdint.h>

#include <fstream>

#define F_UPDATE 0b01000000
#define F_CRCPRES 0b00100000
#define F_RESTRICT 0b00010000

#define M_RES_TAGSIZE 0b11000000
#define F_RES_TAG1M 0b00000000
#define F_RES_TAG128K 0b01000000
#define F_RES_TAG40K 0b10000000
#define F_RES_TAG4K 0b11000000

#define F_RES_TXTENC 0b00100000

#define M_RES_TXTSIZE 0b00011000
#define F_RES_TXTNONE 0b00000000
#define F_RES_TXT1024 0b01000000
#define F_RES_TXT128 0b10000000
#define F_RES_TXT30 0b11000000

#define F_RES_IMGENC 0b00000100

#define M_RES_IMGSIZE 0b00000011
#define F_RES_IMGNONE 0b00000000
#define F_RES_IMG256 0b01000000
#define F_RES_IMG64 0b10000000
#define F_RES_IMGEX64 0b11000000

namespace id3v2 {
	enum res_tag {
		max128x1m,
		max64x128k,
		max32x40k,
		max32x4k
	};

	enum res_txt {
		none_txt,
		max1024,
		max128,
		max30
	};

	enum res_img {
		none_img,
		max256,
		max64,
		exact64
	};

	class ExtendedHeader {
		private:
			id3v2::state	state;
		public:
			uint32_t	size;
			struct {
				bool	update;
				bool	crc;
				bool	restrict;
			}			flags;
			uint64_t	crc;
			struct {
				res_tag	tag_size;
				bool	txt_encode;
				res_txt	txt_size;
				bool	img_encode;
				res_img	img_size;
			}			restrict;

			ExtendedHeader();
			ExtendedHeader(const ExtendedHeader & other);
			~ExtendedHeader();

			ExtendedHeader & operator=(const ExtendedHeader & other);

			bool get_ext_header(std::ifstream & file);

			bool unset() const { return (state == id3v2::unset); };
			bool bad() const { return (state == id3v2::bad); };
			bool good() const { return (state == id3v2::good); };
	};
}
