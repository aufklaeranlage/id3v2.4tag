#pragma once

#include "Frame.hpp"

#include <stdint.h>

#include <iostream>

namespace id3v2 {
	//	<Header for 'Text information frame', ID: "T000" - "TZZZ",
	//	excluding "TXXX" described in 4.2.6.>
	//	Text encoding                $xx
	//	Information                  <text string(s) according to encoding>
	
	//	<Header for 'User defined text information frame', ID: "TXXX">
	//	Text encoding     $xx
	//	Description       <text string according to encoding> $00 (00)
	//	Value             <text string according to encoding>
	class TIF : public Frame {
		public:
			id3v2::txtenc	enc;
			const char **	strs;
			uint32_t		nstrs;

			TIF();
			TIF(const TIF & other);
			TIF(const char * str);
			TIF(std::ifstream & stream);
			~TIF();

			TIF & operator=(const TIF & other);
			
			bool get_frame(std::ifstream & stream);

			virtual std::ostream & insert(std::ostream & out) const;
	};
}
