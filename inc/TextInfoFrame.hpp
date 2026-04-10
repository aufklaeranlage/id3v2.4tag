#pragma once

#include "Frame.hpp"

#include <stdint.h>

#include <iostream>

namespace id3v2 {
	//	<Header for 'Text information frame', ID: "T000" - "TZZZ",
	//	excluding "TXXX" described in 4.2.6.>
	//	Text encoding                $xx
	//	Information                  <text string(s) according to encoding>
	class TIFrame : public Frame {
		public:
			id3v2::txtenc	enc;
			const char **	strs;
			uint32_t		nstrs;

			TIFrame();
			TIFrame(const TIFrame & other);
			TIFrame(const char * str);
			TIFrame(std::ifstream & stream);
			~TIFrame();

			TIFrame & operator=(const TIFrame & other);
			
			bool get_frame(std::ifstream & stream);

			virtual std::ostream & insert(std::ostream & out) const;
	};
}
