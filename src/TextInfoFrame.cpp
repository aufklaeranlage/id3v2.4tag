#include "TextInfoFrame.hpp"

#include "Frame.hpp"

#include <string.h>

#include <cstdlib>

id3v2::TIFrame::TIFrame() : Frame() {
	memcpy(id, "T000\0", 5);
	strs = NULL;
	nstrs = 0;
}

id3v2::TIFrame::TIFrame(const id3v2::TIFrame & other) : id3v2::Frame(other) {
	strs = NULL;
	nstrs = 0;
	*this = other;
}

id3v2::TIFrame::TIFrame(std::ifstream & stream) {
	get_frame(stream);
}

id3v2::TIFrame::~TIFrame() {
	free(strs);
}

id3v2::TIFrame & id3v2::TIFrame::operator=(const id3v2::TIFrame & other) {
	if (this == &other)
		return (*this);
	id3v2::Frame::operator=(other);
	if (state == id3v2::bad)
		return (*this);
	state = id3v2::bad;
	
	enc = other.enc;
	nstrs = other.nstrs;
	free(strs);
	strs = reinterpret_cast<const char **>(malloc(nstrs * sizeof(*strs)));
	if (strs == NULL)
		return (*this);
	for (uint32_t i = 0; i < nstrs; i++) {
		strs[i] = (other.strs[i] - other.data) + data;
	}
	
	state = id3v2::good;
	return (*this);
}

bool id3v2::TIFrame::get_frame(std::ifstream & stream) {
	id3v2::Frame::get_frame(stream);
	if (state == id3v2::bad || state == id3v2::unset)
		return (false);
	state = id3v2::bad;

	enc = txtenc_from_char(data[0]);

	free(strs);
	char *		tmp[32];
	bzero(tmp, 32 * sizeof(*tmp));
	nstrs = 1;
	tmp[0] = data + 1;
	for (uint32_t i = 1; i < size; i++) {
		if (data[i] == '\0' && size - 1 > i) {
			if (nstrs == 32)
				return (false);
			tmp[nstrs++] = data + i + 1;
		}
	}
	strs = reinterpret_cast<const char **>(malloc(nstrs * sizeof(*strs)));
	for (uint32_t i = 0; i < nstrs; i++) {
		strs[i] = tmp[i];
	}

	state = id3v2::good;
	return (true);
}

std::ostream & id3v2::TIFrame::insert(std::ostream & out) const {
	out << "id: " << id << ", strs { ";
	for (uint32_t i = 0; i < nstrs; i++) {
		if (i != 0)
			out << ", ";
		out << strs[i];
	}
	out << " }";
	return (out);
}
