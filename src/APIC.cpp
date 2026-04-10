#include "APIC.hpp"

#include "Frame.hpp"

#include <string.h>

id3v2::APIC::APIC() : Frame() {
	memcpy(id, "APIC\0", 5);
}

id3v2::APIC::APIC(const id3v2::APIC & other) : id3v2::Frame(other) {
	*this = other;
}

id3v2::APIC::APIC(std::ifstream & stream) {
	get_frame(stream);
}

id3v2::APIC::~APIC() { }

id3v2::APIC & id3v2::APIC::operator=(const id3v2::APIC & other) {
	if (this == &other)
		return (*this);
	id3v2::Frame::operator=(other);
	if (state == id3v2::bad)
		return (*this);

	txtenc = other.txtenc;
	mime = other.mime - other.data + data;
	type = other.type;
	desc = other.desc - other.data + data;
	img_data = other.img_data - other.data + data;
	img_len = other.img_len;
	return (*this);
}

bool id3v2::APIC::get_frame(std::ifstream & stream) {
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
	if (pos == size)
		return (false);

	type = data[pos++];
	if (pos++ == size)
		return (false);
	
	desc = data + pos;
	while (pos < size && data[pos] != '\0')
		++pos;
	if (pos++ == size)
		return (false);

	img_data = data + pos;
	img_len = size - pos;

	state = id3v2::good;
	return (true);
}

std::ostream & id3v2::APIC::insert(std::ostream & out) const {
	out << "id: " << id <<
		", mime: " << mime <<
		", desc: " << desc;
	return (out);
}
