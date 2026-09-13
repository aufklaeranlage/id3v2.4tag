#include "file.h"

#include <stdio.h>
#include <string.h>

int main(int ac, char *av[])
{
  char  *filename;
  if (ac < 2)
    return 1;
  filename = av[1];
  b8  fshort = true;
  char  id[4];
  b8  search = false;
  for (i32 i = 2; i < ac; i++) {
    if (av[i][0] == '-') {
      switch (av[i][1]) {
        case 'l': fshort = false;
                  break;
        case 's': fshort = true; break;

        case '-': {
          if (memcmp(av[i] + 2, "search", 6) == 0) {
            ++i;
            if (strlen(av[i]) != 4) {
              fprintf(stderr, "%s: --search: id needs to be exactly 4 characters long.\n", av[0]);
              return 1;
            }
            memcpy(id, av[i], 4);
            search = true;
            break ;
          }
        }
      }
    }
  }
  struct file *f = file_new();
  if (file_read(f, filename) == false) {
    fprintf(stderr, "File '%s' was not readable\n", av[1]);
    return 1;
  }
  if (search == true) {
    struct frame  **results = file_get_frames(f, id);
    if (results == NULL) {
      fprintf(stderr, "%s: error while searching for frames.\n", av[0]);
      goto  exit;
    }
    for (u32 i = 0; results[i] != NULL; i++) {
      if (fshort)
        frame_stream(stdout, results[i]);
      else
        frame_stream_full(stdout, results[i]);
      fprintf(stdout, "\n");
    }
  } else {
    if (fshort)
      file_stream(stdout, f);
    else
      file_stream_full(stdout, f);
  }

exit:
  file_del(f);
  return 0;
}
