#include "ExtendedHeader.hpp"

#include "id3v2.hpp"

#include <cstdlib>

id3v2::ExtendedHeader::ExtendedHeader() {
	state = id3v2::unset;
}

id3v2::ExtendedHeader::ExtendedHeader(const id3v2::ExtendedHeader & other) {
	*this = other;
}

id3v2::ExtendedHeader::~ExtendedHeader() { }

id3v2::ExtendedHeader & id3v2::ExtendedHeader::operator=(const id3v2::ExtendedHeader & other) {
	if (this == &other)
		return (*this);
	state = other.state;
	size = other.size;
	flags = other.flags;
	crc = other.crc;
	restrict = other.restrict;
	return (*this);
}

bool id3v2::ExtendedHeader::get_ext_header(std::ifstream & file) {
	char	buf[6];

	off_t	cur = file.tellg();

	file.read(buf, 6);
	if (file.bad()) {
		return (false);
	}
	state = id3v2::bad;

	size = read_synchsafe_uint32(reinterpret_cast<uint8_t *>(buf));
	if (size < 6) {
		file.seekg(cur);
		return (false);
	}

	/* ignoring number fo flag bytes for convenience sake */

	flags.update = buf[5] & F_UPDATE;
	flags.crc = buf[5] & F_CRCPRES;
	flags.restrict = buf[5] & F_RESTRICT;
	if (size == 6) {
		state = id3v2::good;
		return (true);
	}

	if (size != static_cast<uint32_t>(6 + (flags.update * 1) +
			(flags.crc * 6) + (flags.restrict * 2))) {
		file.seekg(cur);
		return (false);
	}

	char flagbuf[10];
	file.read(flagbuf, size - 6);
	if (file.bad())
		return (false);
	flagbuf[size - 6] = '\0';

	int	pos = 0;

	if (flags.update && flagbuf[pos++] != 0) {
		file.seekg(cur);
		return (false);
	}

	if (flags.crc) {
		if (flagbuf[pos++] != 5) {
			file.seekg(cur);
			return (false);
		}
		crc = read_synchsafe_uint35(reinterpret_cast<uint8_t *>(flagbuf + pos));
		pos += 5;
	}

	if (flags.restrict) {
		if (flagbuf[pos++] != 1) {
			file.seekg(cur);
			return (false);
		}
		switch (flagbuf[pos] & M_RES_TAGSIZE) {
			case F_RES_TAG1M:
				restrict.tag_size = max128x1m; break;
			case F_RES_TAG128K:
				restrict.tag_size = max64x128k; break;
			case F_RES_TAG40K:
				restrict.tag_size = max32x40k; break;
			case F_RES_TAG4K:
				restrict.tag_size = max32x4k; break;
		}
		restrict.txt_encode = flagbuf[pos] & F_RES_TXTENC;
		switch (flagbuf[pos] & M_RES_TXTSIZE) {
			case F_RES_TXTNONE:
				restrict.txt_size = none_txt; break;
			case F_RES_TXT1024:
				restrict.txt_size = max1024; break;
			case F_RES_TXT128:
				restrict.txt_size = max128; break;
			case F_RES_TXT30:
				restrict.txt_size = max30; break;
		}
		restrict.img_encode = flagbuf[pos] & F_RES_IMGENC;
		switch (flagbuf[pos] & M_RES_IMGSIZE) {
			case F_RES_IMGNONE:
				restrict.img_size = none_img; break;
			case F_RES_IMG256:
				restrict.img_size = max256; break;
			case F_RES_IMG64:
				restrict.img_size = max64; break;
			case F_RES_IMGEX64:
				restrict.img_size = exact64; break;
		}
	}
	state = id3v2::good;
	return (true);
}
