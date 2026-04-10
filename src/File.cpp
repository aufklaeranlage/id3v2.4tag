#include "File.hpp"

#include "UFID.hpp"
#include "TIF.hpp"
#include "AENC.hpp"
#include "APIC.hpp"
#include "GEOB.hpp"

#include <iostream>

id3v2::File::File() { }

id3v2::File::File(const id3v2::File & other) {
	*this = other;
}

id3v2::File::~File() {
	for (std::vector<id3v2::Frame *>::iterator it = frames.begin();
			it != frames.end(); it++) {
		delete *it;
	}
}

id3v2::File & id3v2::File::operator=(const id3v2::File & other) {
	if (this == &other)
		return (*this);
	name = other.name;
	header = other.header;
	ext_header = other.ext_header;
	footer = other.footer;
	frames = other.frames;
	return (*this);
}

#include <string.h>

bool id3v2::File::read_file(const std::string & filename) {
	name = filename;

	std::ifstream stream;
	stream.open(name.c_str());

	if (header.get_header(stream) == false) {
		/* No header found, try to get footer scanning from end of file */
		off_t	offset = -50;
		char	buf[50];

		stream.seekg(offset, std::ios_base::end);
		while (stream.tellg() != 0) {
			/* three chars less to have identifier sized overlap */
			offset -= 47;
			stream.read(buf, 50);
			char	* pos = reinterpret_cast<char *>(memchr(buf, '3', 50));
			if (pos == NULL || pos - buf > 47 ||
					(pos[1] != 'D' && pos[2] != 'I')) {
				stream.seekg(offset, std::ios_base::end);
				continue ;
			}
			if (footer.get_footer(stream))
				break ;
		}
	} else if ((header.unset() == false && header.flags.extended) ||
			(footer.unset() == false && footer.flags.extended)) {
		if (ext_header.get_ext_header(stream))
			return (false);
	}
	if (header.unset() && footer.unset())
		return (false);
	if (header.unset() == false && footer.unset() == false && header != footer)
		return (false);

	uint32_t size = header.unset() ? footer.size : header.size;
	uint32_t pos = 0;
	while (pos < size) {
		char	id[5];
		id[4] = '\0';
		off_t	cur = stream.tellg();
		stream.read(id, 4);
		if (stream.bad())
			return (false);
		pos += 10;

		id3v2::Frame *	frame;
		if (strcmp(id, "UFID") == 0) {
			frame = new id3v2::UFID();
		} else if (id[0] == 'T' &&
				(id[1] >= '0' && id[1] <= 'Z') &&
				(id[2] >= '0' && id[2] <= 'Z') &&
				(id[3] >= '0' && id[3] <= 'Z')) {
			frame = new id3v2::TIF();
		} else if (strcmp(id, "AENC") == 0) {
			frame = new id3v2::AENC();
		} else if (strcmp(id, "APIC") == 0) {
			frame = new id3v2::APIC();
		} else if (strcmp(id, "GEOB") == 0) {
			frame = new id3v2::GEOB();
		} else {
			frame = new id3v2::Frame();
		}

		if (frame == NULL) {
			stream.seekg(cur);
			return (false);
		}

		frame->set_id(id);
		if (frame->get_frame(stream) == false || frame->unset() || frame->size == 0) {
			delete frame;
			break ;
		}
		pos += frame->size;
		frames.push_back(frame);
	}
	padding = size - pos;

	return (true);
}
