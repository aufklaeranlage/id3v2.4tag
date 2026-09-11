#pragma once

#include "Frame.hpp"

#include <stdint.h>

#include <iostream>

namespace id3v2 {
	//	<Header for 'Unique file identifier', ID: "UFID">
	//	Owner identifier        <text string> $00
	//	Identifier              <up to 64 bytes binary data>
	class UFID: public Frame {
		public:
			const char *	owner_id;
			const char *	file_id;

			UFID();
			UFID(const UFID & other);
			UFID(std::ifstream & stream);
			~UFID();

			UFID & operator=(const UFID & other);

			bool get_frame(std::ifstream & stream);

			virtual std::ostream & insert(std::ostream & out) const;
	};
}
