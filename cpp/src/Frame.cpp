#include "Frame.hpp"

#include "id3v2.hpp"

#include <cstdlib>
#include <cstring>

#include <fstream>

id3v2::Frame::Frame() {
	state = id3v2::unset;
	bzero(id, 5);
	data = NULL;
	size = 0;
	position = 0;
};

id3v2::Frame::Frame(const id3v2::Frame & other) {
	state = id3v2::unset;
	bzero(id, 5);
	data = NULL;
	size = 0;
	position = 0;
	*this = other;
}

id3v2::Frame::~Frame() {
	free (data);
}

id3v2::Frame & id3v2::Frame::operator=(const id3v2::Frame & other) {
	if (this == &other)
		return (*this);
	memcpy(id, other.id, 5);
	state = other.state;
	position = other.position;
	size = other.size;
	status = other.status;
	format = other.format;
	free (data);
	data = reinterpret_cast<char *>(malloc(sizeof(*data) * (size + 1)));
	if (data == NULL) {
		state = id3v2::bad;
		return (*this);
	}
	std::memcpy(data, other.data, size + 1);
	return (*this);
}

bool id3v2::Frame::get_frame(std::ifstream & stream) {
	char	headerbuf[6];

	off_t	cur = stream.tellg();

	state = id3v2::bad;
	position = stream.tellg();
	stream.read(headerbuf, 6);
	if (stream.bad())
		return (false);
	size = read_synchsafe_uint32(reinterpret_cast<uint8_t *>(headerbuf));

	status.preserve_tag = headerbuf[8] & F_TAGPRES;
	status.preserve_file = headerbuf[8] & F_FILPRES;
	status.read_only = headerbuf[8] & F_RDONLY;

	format.grouped = headerbuf[9] & F_GROUPED;
	format.compressed = headerbuf[9] & F_COMPRES;
	format.encrypted = headerbuf[9] & F_ENCRYPT;
	format.unsynchronisation = headerbuf[9] & F_UNSYNC;
	format.length_indicated = headerbuf[9] & F_LENINDI;

	free(data);
	data = reinterpret_cast<char *>(malloc(sizeof(*data) * (size + 1)));
	if (data == NULL) {
		stream.seekg(cur);
		return (false);
	}
	stream.read(data, size);
	if (stream.bad())
		return (false);
	data[size] = '\0';

	state = id3v2::good;
	return (true);
}

#include <iostream>

std::ostream & id3v2::Frame::insert(std::ostream & out) const {
	out << "id: " << id <<
		", position: " << position << " bytes" << 
		", size: " << size << " bytes" <<
		", status { preserve tag: " << status.preserve_tag <<
		", preserve file: " << status.preserve_file <<
		", read only: " << status.read_only <<
		" }, format { grouped: " << format.grouped <<
		", compressed: " << format.compressed <<
		", encrypted: " << format.encrypted <<
		", unsynchronisation: " << format.unsynchronisation <<
		", length indicated: " << format.length_indicated <<
		" }, data: " << data;
	return (out);
}

std::ostream & operator<<(std::ostream & out, const id3v2::Frame & frame) {
	frame.insert(out);
	return (out);
}
