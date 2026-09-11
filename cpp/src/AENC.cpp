#include "AENC.hpp"

#include "Frame.hpp"

#include <string.h>

id3v2::AENC::AENC() : Frame() {
	memcpy(id, "AENC\0", 5);
}

id3v2::AENC::AENC(const id3v2::AENC & other) : id3v2::Frame(other) {
	*this = other;
}

id3v2::AENC::AENC(std::ifstream & stream) {
	get_frame(stream);
}

id3v2::AENC::~AENC() { }

id3v2::AENC & id3v2::AENC::operator=(const id3v2::AENC & other) {
	if (this == &other)
		return (*this);
	id3v2::Frame::operator=(other);
	if (state == id3v2::bad)
		return (*this);

	owner = data;
	start = other.start;
	length = other.length;
	info_len = other.info_len;
	info = other.info - other.data + data;
	return (*this);
}

bool id3v2::AENC::get_frame(std::ifstream & stream) {
	id3v2::Frame::get_frame(stream);
	if (state == id3v2::bad || state == id3v2::unset)
		return (false);
	state = id3v2::bad;

	owner = data;

	uint32_t pos = 0;
	while (pos < size && data[pos] != '\0')
		++pos;
	if (pos < size - 2)
		return (false);
	start = static_cast<uint32_t>(data[++pos]);
	length = static_cast<uint32_t>(data[++pos]);
	info = data + pos;
	info_len = size - pos;

	state = id3v2::good;
	return (true);
}

std::ostream & id3v2::AENC::insert(std::ostream & out) const {
	out << "id: " << id <<
		", owner: " << owner <<
		", info: " << info;
	return (out);
}
