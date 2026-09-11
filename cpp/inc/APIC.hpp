#pragma once

#include "Frame.hpp"

#include <stdint.h>

#include <iostream>

namespace id3v2 {
	//	<Header for 'Attached picture', ID: "APIC">
	//	Text encoding      $xx
	//	MIME type          <text string> $00
	//	Picture type       $xx
	//	Description        <text string according to encoding> $00 (00)
	//	Picture data       <binary data>
	class APIC : public Frame {
		public:
			id3v2::txtenc		txtenc;
			const char *		mime;
			uint8_t				type;
			const char *		desc;
			const char *		img_data;
			uint32_t			img_len;

			APIC();
			APIC(const APIC & other);
			APIC(std::ifstream & stream);
			~APIC();

			APIC & operator=(const APIC & other);
			
			bool get_frame(std::ifstream & stream);

			virtual std::ostream & insert(std::ostream & out) const;
	};
}
