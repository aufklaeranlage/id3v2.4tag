#include "UFID.hpp"

#include "Frame.hpp"

#include <string.h>

id3v2::UFID::UFID() : Frame() {
	memcpy(id, "UFID\0", 5);
}

id3v2::UFID::UFID(const id3v2::UFID & other) : id3v2::Frame(other) {
	*this = other;
}

id3v2::UFID::UFID(std::ifstream & stream) {
	get_frame(stream);
}

id3v2::UFID::~UFID() { }

id3v2::UFID & id3v2::UFID::operator=(const id3v2::UFID & other) {
	if (this == & other)
		return (*this);
	id3v2::Frame::operator=(other);
	if (state == id3v2::bad)
		return (*this);

	owner_id = other.owner_id - other.data + data;
	file_id = other.file_id - other.data + data;
	return (*this);
}

bool id3v2::UFID::get_frame(std::ifstream & stream) {
	id3v2::Frame::get_frame(stream);
	if (state == id3v2::bad || state == id3v2::unset)
		return (false);
	state = id3v2::bad;

	owner_id = data;

	uint32_t pos = 0;
	while (pos < size && data[pos] != '\0')
		++pos;
	if (pos == size)
		return (false);
	file_id = data + ++pos;
	if (size - pos > 64)
		return (false);

	state = id3v2::good;
	return (true);
}

std::ostream & id3v2::UFID::insert(std::ostream & out) const {
	out << "id: " << id <<
		", owner_id: " << owner_id <<
		", file_id: " << file_id;
	return (out);
}
