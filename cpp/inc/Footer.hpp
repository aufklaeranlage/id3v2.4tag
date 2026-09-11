#pragma once

#include "id3v2.hpp"

#include <fstream>
#include <stdint.h>

#define F_UNSYN 0b10000000
#define F_EXTEN 0b01000000
#define F_EXPER 0b00100000
#define F_FOOTR 0b00010000
#define F_UNCLR 0b00001111

namespace id3v2 {
	class Footer {
		private:
			id3v2::state	state;
		public:
			struct {
				uint32_t	major;
				uint32_t	revision;
			}			version;
			struct {
				bool	unsynchronisation;
				bool	extended;
				bool	experimental;
				bool	footer;
				bool	uncleared;
			}			flags;
			uint32_t	size;

			Footer();
			Footer(const Footer & other);
			~Footer();

			Footer & operator=(const Footer & other);

			bool get_footer(std::ifstream & file);

			bool unset() const { return (state == id3v2::unset); };
			bool bad() const { return (state == id3v2::bad); };
			bool good() const { return (state == id3v2::good); };
	};
};

#include "Header.hpp"

bool operator==(const id3v2::Footer & footer, const id3v2::Header & header);
bool operator!=(const id3v2::Footer & footer, const id3v2::Header & header);

#include <iostream>

std::ostream & operator<<(std::ostream & out, const id3v2::Footer & id3);
