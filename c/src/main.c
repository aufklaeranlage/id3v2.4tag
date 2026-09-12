#include "file.h"

#include <stdio.h>

int main(int ac, char *av[])
{
	char	*filename;
  if (ac < 2)
    return 1;
	filename = av[1];
	b8	fshort = true;
	for (i32 i = 2; i < ac; i++) {
		if (av[i][0] == '-') {
			switch (av[i][1]) {
				case 'l': fshort = false; break;
				case 's': fshort = true; break;
			}
		}
	}
  struct file *f = file_new();
  if (file_read(f, filename) == false) {
    fprintf(stderr, "File '%s' was not readable\n", av[1]);
    return 1;
  }
	if (fshort)
		file_stream(stdout, f);
	else
		file_stream_full(stdout, f);
	file_del(f);
  return 0;
}
