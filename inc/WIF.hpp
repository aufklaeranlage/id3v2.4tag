#pragma once

#include "Frame.hpp"

#include <stdint.h>

#include <iostream>

namespace id3v2 {
	//	<Header for 'URL link frame', ID: "W000" - "WZZZ", excluding "WXXX"
	//	described in 4.3.2.>
	//	URL              <text string>
	
	// If the text string is followed by a string termination, all following
	// information should be ignored.
	
	//	<Header for 'User defined URL link frame', ID: "WXXX">
	//	Text encoding     $xx
	//	Description       <text string according to encoding> $00 (00)
	//	URL               <text string>
	class WIF : public Frame {
		public:
			id3v2::txtenc	enc;
			const char *	desc;
			const char *	str;

			WIF();
			WIF(const WIF & other);
			WIF(const char * str);
			WIF(std::ifstream & stream);
			~WIF();

			WIF & operator=(const WIF & other);
			
			bool get_frame(std::ifstream & stream);

			virtual std::ostream & insert(std::ostream & out) const;
	};
}
