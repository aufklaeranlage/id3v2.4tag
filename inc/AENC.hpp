#pragma once

#include "Frame.hpp"

#include <stdint.h>

#include <iostream>

namespace id3v2 {
	//	Owner identifier   <text string> $00
	//	Preview start      $xx xx
	//	Preview length     $xx xx
	//	Encryption info    <binary data>
	class AENC : public Frame {
		public:
			char		* owner;
			uint32_t	start;
			uint32_t	length;
			char		* info;
			uint32_t	info_len;

			AENC();
			AENC(const AENC & other);
			AENC(std::ifstream & stream);
			~AENC();

			AENC & operator=(const AENC & other);
			
			bool get_frame(std::ifstream & stream);

			virtual std::ostream & insert(std::ostream & out) const;
	};
}
