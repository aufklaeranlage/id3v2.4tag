#include "file.h"

#include <stdio.h>

int main(int ac, char *av[])
{
  if (ac < 2)
    return 1;
  struct file *f = file_new();
  if (file_read(f, av[1]) == false) {
    fprintf(stderr, "File '%s' was not readable\n", av[1]);
    return 1;
  }
  file_stream_full(stdout, f);
  file_del(f);
  return 0;
}
