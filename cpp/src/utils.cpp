#include "id3v2.hpp"

uint32_t id3v2::read_synchsafe_uint32(uint8_t * synchsafe) {
	uint32_t	byte0 = synchsafe[0] & M_SYNCHSAFE;
	uint32_t	byte1 = synchsafe[1] & M_SYNCHSAFE;
	uint32_t	byte2 = synchsafe[2] & M_SYNCHSAFE;
	uint32_t	byte3 = synchsafe[3] & M_SYNCHSAFE;
	return (byte0 << 21 | byte1 << 14 | byte2 << 7 | byte3);
}

void id3v2::write_synchsafe_uint32(uint8_t * dest, uint32_t n) {
	dest[0] = (n >> 21) & M_SYNCHSAFE;
	dest[1] = (n >> 14) & M_SYNCHSAFE;
	dest[2] = (n >> 7) & M_SYNCHSAFE;
	dest[3] = n & M_SYNCHSAFE;
}

uint64_t id3v2::read_synchsafe_uint35(uint8_t * synchsafe) {
	uint32_t	byte0 = synchsafe[0] & M_SYNCHSAFE35UPPER;
	uint32_t	byte1 = synchsafe[1] & M_SYNCHSAFE;
	uint32_t	byte2 = synchsafe[2] & M_SYNCHSAFE;
	uint32_t	byte3 = synchsafe[3] & M_SYNCHSAFE;
	uint32_t	byte4 = synchsafe[4] & M_SYNCHSAFE;
	return (byte0 << 28 | byte1 << 21 | byte2 << 14 | byte3 << 7 | byte4);
}

void id3v2::write_synchsafe_uint35(uint8_t * dest, uint64_t n) {
	dest[0] = (n >> 28) & M_SYNCHSAFE35UPPER;
	dest[1] = (n >> 21) & M_SYNCHSAFE;
	dest[2] = (n >> 14) & M_SYNCHSAFE;
	dest[3] = (n >> 7) & M_SYNCHSAFE;
	dest[4] = n & M_SYNCHSAFE;
}

id3v2::txtenc id3v2::txtenc_from_char(const char c) {
	return (static_cast<id3v2::txtenc>(c));
}

id3v2::imgtype id3v2::imgtype_from_char(const char c) {
	return (static_cast<id3v2::imgtype>(c));
}

#include "Footer.hpp"
#include "Header.hpp"

bool operator==(const id3v2::Header & header, const id3v2::Footer & footer) {
	return (header.version.major == footer.version.major &&
			header.version.revision == footer.version.revision &&
			header.flags.unsynchronisation == footer.flags.unsynchronisation &&
			header.flags.extended == footer.flags.extended &&
			header.flags.experimental == footer.flags.experimental &&
			header.flags.footer == footer.flags.footer &&
			header.flags.uncleared == footer.flags.uncleared &&
			header.size == footer.size);
}

bool operator==(const id3v2::Footer & footer, const id3v2::Header & header) {
	return (header == footer);
}

bool operator!=(const id3v2::Header & header, const id3v2::Footer & footer) {
	return (!(header == footer));
}

bool operator!=(const id3v2::Footer & footer, const id3v2::Header & header) {
	return (!(header == footer));
}
