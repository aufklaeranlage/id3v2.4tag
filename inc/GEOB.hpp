#pragma once

#include "Frame.hpp"

#include <stdint.h>

#include <iostream>

namespace id3v2 {
	//	<Header for 'General encapsulated object', ID: "GEOB">
	//	Text encoding          $xx
	//	MIME type              <text string> $00
	//	Filename               <text string according to encoding> $00 (00)
	//	Content description    <text string according to encoding> $00 (00)
	//	Encapsulated object    <binary data>
	class GEOB : public Frame {
		public:
			id3v2::txtenc	txtenc;
			const char *	mime;
			const char *	filename;
			const char *	desc;
			const char *	obj;
			uint32_t		obj_len;

			GEOB();
			GEOB(const GEOB & other);
			GEOB(std::ifstream & stream);
			~GEOB();

			GEOB & operator=(const GEOB & other);
			
			bool get_frame(std::ifstream & stream);

			virtual std::ostream & insert(std::ostream & out) const;
	};
}
