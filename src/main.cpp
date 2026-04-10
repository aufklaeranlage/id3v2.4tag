#include "File.hpp"

#include "TIF.hpp"

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
			std::cout << **it << std::endl;
		}
	}

	return (0);
}
