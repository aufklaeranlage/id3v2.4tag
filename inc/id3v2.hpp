#pragma once

#include <stdint.h>

#define M_SYNCHSAFE 0b01111111
#define M_SYNCHSAFE35UPPER 0b00001111

namespace id3v2 {
	enum state {
		unset,
		bad,
		good
	};

	enum txtenc {
		ascii,
		ucs2,
		utf16be,
		utf8,
		invalid_txtenc
	};

	txtenc		txtenc_from_char(const char c);

	enum imgtype {
		other,
		icon32x32,
		icon_other,
		cover_front,
		cover_back,
		leaflet,
		media,
		lead_artist,
		artist,
		conductor,
		band,
		composer,
		lyricist,
		location,
		during_recording,
		during_performance,
		video_capture,
		fish,
		illustration,
		band_logo,
		studio_logo
	};

	imgtype		imgtype_from_char(const char c);

	uint32_t	read_synchsafe_uint32(uint8_t * synchsafe);
	void		write_synchsafe_uint32(uint8_t * dest, uint32_t n);

	uint64_t	read_synchsafe_uint35(uint8_t * synchsafe);
	void		write_synchsafe_uint35(uint8_t * dest, uint64_t n);
}
