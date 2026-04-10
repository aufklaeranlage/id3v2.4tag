#pragma once

#include "id3v2.hpp"

#include <stdint.h>

#include <cstring>
#include <fstream>

#define F_TAGPRES 0b01000000
#define F_FILPRES 0b00100000
#define F_RDONLY 0b00010000

#define F_GROUPED 0b01000000
#define F_COMPRES 0b00001000
#define F_ENCRYPT 0b00000100
#define F_UNSYNC 0b00000010
#define F_LENINDI 0b00000001

namespace id3v2 {
	class Frame {
		protected:
			id3v2::state	state;
		public:
			char		id[5];
			uint64_t	position;
			uint32_t	size;
			struct {
				bool	preserve_tag;
				bool	preserve_file;
				bool	read_only;
			}			status;
			struct {
				bool	grouped;
				bool	compressed;
				bool	encrypted;
				bool	unsynchronisation;
				bool	length_indicated;
			}			format;
			char		*data;

			Frame();
			Frame(const Frame & other);
			virtual ~Frame();

			virtual Frame & operator=(const Frame & other);

			virtual bool get_frame(std::ifstream & stream);

			bool unset() const { return (state == id3v2::unset); };
			bool bad() const { return (state == id3v2::bad); };
			bool good() const { return (state == id3v2::good); };

			void set_id(const char * idstr) { memcpy(id, idstr, 4); };

			virtual std::ostream & insert(std::ostream & out) const;
	};
};

#include <iostream>

std::ostream & operator<<(std::ostream & out, const id3v2::Frame & frame);
