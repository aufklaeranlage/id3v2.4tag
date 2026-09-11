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
	class Header {
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

			Header();
			Header(const Header & other);
			~Header();

			Header & operator=(const Header & other);

			bool get_header(std::ifstream & file);

			bool unset() const { return (state == id3v2::unset); };
			bool bad() const { return (state == id3v2::bad); };
			bool good() const { return (state == id3v2::good); };
	};
};

#include "Footer.hpp"

bool operator==(const id3v2::Header & header, const id3v2::Footer & footer);
bool operator!=(const id3v2::Header & header, const id3v2::Footer & footer);

#include <iostream>

std::ostream & operator<<(std::ostream & out, const id3v2::Header & id3);
