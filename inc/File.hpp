#pragma once

#include "Header.hpp"
#include "ExtendedHeader.hpp"
#include "Footer.hpp"
#include "Frame.hpp"

#include <vector>

namespace id3v2 {
	class File {
		private:
			std::string				name;

			Header					header;
			ExtendedHeader			ext_header;
			Footer					footer;

			std::vector<Frame *>	frames;

			uint32_t				padding;

		public:
			File();
			File(const File & other);
			~File();

			File & operator=(const File & other);

			bool	read_file(const std::string & filename);

			const std::string			& get_name() const { return (name); };
			const Header				& get_header() const { return (header); };
			const ExtendedHeader		& get_ext_header() const { return (ext_header); };
			const Footer				& get_footer() const { return (footer); };
			const std::vector<Frame *>	& get_frames() const { return (frames); };

			void	set_name(const std::string & other) { name = other; };
			void	set_header(const Header & other) { header = other; };
			void	set_ext__header(const ExtendedHeader & other) { ext_header = other; };
			void	set_footer(const Footer & other) { footer = other; };
			void	set_frames(const std::vector<Frame *> & other) { frames = other; };
	};
}
