#include "File.hpp"

#include "TextInfoFrame.hpp"

#include <fstream>
#include <iostream>
#include <vector>

int main(int ac, char *av[]) {
	if (ac != 2)
		return (1);
	id3v2::File	file;

	if (file.read_file(av[1])) {
		const std::vector<id3v2::Frame *> & frames = file.get_frames();
		for (std::vector<id3v2::Frame *>::const_iterator it = frames.begin();
				it != frames.end(); it++) {
			if ((*it)->id[0] == 'T' && strcmp((*it)->id + 1, "XXX")) {
				id3v2::TIFrame frame = id3v2::TIFrame(*dynamic_cast<id3v2::TIFrame *>(*it));
				std::cout << frame << std::endl;
			} else
				std::cout << **it << std::endl;
		}
	}

	return (0);
}
