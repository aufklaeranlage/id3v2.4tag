#include "WIF.hpp"

#include "Frame.hpp"

#include <string.h>

#include <cstdlib>

id3v2::WIF::WIF() : Frame() {
	memcpy(id, "W000\0", 5);
	str = NULL;
	desc = NULL;
}

id3v2::WIF::WIF(const id3v2::WIF & other) : id3v2::Frame(other) {
	str = NULL;
	desc = NULL;
	*this = other;
}

id3v2::WIF::WIF(std::ifstream & stream) {
	get_frame(stream);
}

id3v2::WIF::~WIF() { }

id3v2::WIF & id3v2::WIF::operator=(const id3v2::WIF & other) {
	if (this == &other)
		return (*this);
	id3v2::Frame::operator=(other);
	if (state == id3v2::bad)
		return (*this);
	enc = other.enc;
	str = other.str - other.data + data;
	desc = other.desc - other.data + data;
	return (*this);
}

bool id3v2::WIF::get_frame(std::ifstream & stream) {
	id3v2::Frame::get_frame(stream);
	if (state == id3v2::bad || state == id3v2::unset)
		return (false);
	state = id3v2::bad;
	uint32_t pos = 0;
	if (strcmp(id, "WXXX") == 0) {
		enc = id3v2::txtenc_from_char(data[pos++]);
		desc = data + pos;
		while (pos < size && data[pos] != '\0')
			++pos;
		if (pos == size)
			return (false);
	}
	str = data + pos;
	state = id3v2::good;
	return (true);
}

std::ostream & id3v2::WIF::insert(std::ostream & out) const {
	out << "id: " << id;
	if (strcmp(id, "WXXX") == 0) {
		out << ", desc: " << desc;
	}
	out << ", str: " << str;
	return (out);
}
