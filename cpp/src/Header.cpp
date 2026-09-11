#include "Header.hpp"

#include "id3v2.hpp"

#include <cstring>

id3v2::Header::Header() {
	state = id3v2::unset;
};

id3v2::Header::Header(const id3v2::Header & other) {
	*this = other;
};

id3v2::Header::~Header() { };

id3v2::Header & id3v2::Header::operator=(const id3v2::Header & other) {
	if (this == &other)
		return (*this);
	state = other.state;
	version = other.version;
	flags = other.flags;
	size = other.size;
	return (*this);
}

bool id3v2::Header::get_header(std::ifstream & file) {
	char	buf[10];

	off_t	cur = file.tellg();

	file.read(buf, 10);
	if (file.bad())
		return (false);

	state = id3v2::bad;
	if (std::strncmp(buf, "ID3", 3) != 0) {
		file.seekg(cur);
		return (false);
	}
	state = id3v2::good;

	version.major = buf[3];
	version.revision = buf[4];

	flags.unsynchronisation = buf[5] & F_UNSYN;
	flags.extended = buf[5] & F_EXTEN;
	flags.experimental = buf[5] & F_EXPER;
	flags.footer = buf[5] & F_FOOTR;
	flags.uncleared = buf[5] & F_UNCLR;

	size = read_synchsafe_uint32(reinterpret_cast<uint8_t *>(buf + 6));

	return (true);
}

std::ostream & operator<<(std::ostream & out, const id3v2::Header & id3) {
	out << "version { major: " << id3.version.major <<
		", revision: " << id3.version.revision <<
		" }, flags { unsynchronisation: " << id3.flags.unsynchronisation <<
		", extended header: " << id3.flags.extended <<
		", experimental indicator: " << id3.flags.experimental <<
		", footer present: " << id3.flags.footer <<
		", uncleared flags: " << id3.flags.uncleared <<
		" }, size: " << id3.size << " bytes";
	return (out);		
}
