#include "GEOB.hpp"

#include "Frame.hpp"

#include <string.h>

id3v2::GEOB::GEOB() : Frame() {
	memcpy(id, "GEOB\0", 5);
}

id3v2::GEOB::GEOB(const id3v2::GEOB & other) : id3v2::Frame(other) {
	*this = other;
}

id3v2::GEOB::GEOB(std::ifstream & stream) {
	get_frame(stream);
}

id3v2::GEOB::~GEOB() { }

id3v2::GEOB & id3v2::GEOB::operator=(const id3v2::GEOB & other) {
	if (this == &other)
		return (*this);
	id3v2::Frame::operator=(other);
	if (state == id3v2::bad)
		return (*this);

	txtenc = other.txtenc;
	mime = other.mime - other.data + data;
	filename = other.filename - other.data + data;
	desc = other.desc - other.data + data;
	obj = other.obj - other.data + data;
	obj_len = other.obj_len;
	return (*this);
}

bool id3v2::GEOB::get_frame(std::ifstream & stream) {
	id3v2::Frame::get_frame(stream);
	if (state == id3v2::bad || state == id3v2::unset)
		return (false);
	state = id3v2::bad;

	if (size == 0)
		return (false);

	uint32_t pos = 0;
	txtenc = txtenc_from_char(data[pos++]);

	mime = data + pos;
	while (pos < size && data[pos] != '\0')
		++pos;
	if (pos++ == size)
		return (false);

	filename = data + pos;
	while (pos < size && data[pos] != '\0')
		++pos;
	if (pos++ == size)
		return (false);

	desc = data + pos;
	while (pos < size && data[pos] != '\0')
		++pos;
	if (pos++ == size)
		return (false);

	obj = data + pos;
	obj_len = size - pos;

	state = id3v2::good;
	return (true);
}

std::ostream & id3v2::GEOB::insert(std::ostream & out) const {
	out << "id: " << id <<
		", mime: " << mime <<
		", filename: " << filename <<
		", desc: " << desc;
	return (out);
}
