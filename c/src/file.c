#include "file.h"

#include <stdlib.h>
#include <string.h>

struct file *file_init(struct file *f)
{
  if (f == NULL)
    return NULL;
  memset(f, 0, sizeof(*f));
  f->state = unset;
  header_clear(&f->header);
  ext_header_clear(&f->ext_header);
  return f;
}

struct file *file_new()
{
  struct file *new = malloc(sizeof(*new));
  return file_init(new);
}

struct file *file_cpy(struct file *dest, const struct file *src)
{
  file_clear(dest);
  dest->state = bad;
  dest->frames = malloc(sizeof(*dest->frames));
  if (dest->frames == NULL)
    return dest;
  for (u32 i = 0; i < src->nframes; i++) {
    struct frame  *cpy = frame_clone(src->frames[i]);
    if (cpy == NULL) {
      dest->nframes = i;
      file_clear(dest);
      return dest;
    }
  }
  dest->nframes = src->nframes;
  dest->name = malloc(sizeof(*dest->name) * strlen(src->name) + 1);
  if (dest->name == NULL) {
      file_clear(dest);
      return dest;
  }
  memcpy(dest->name, src->name, strlen(src->name) + 1);
  header_cpy(&dest->header, &src->header);
  ext_header_cpy(&dest->ext_header, &src->ext_header);
  dest->padding = src->padding;

  dest->state = src->state;
  return dest;
}

struct file *file_clone(const struct file *src)
{
  struct file *dest = malloc(sizeof(*dest));
  if (dest == NULL)
    return NULL;
  file_init(dest);
  return file_cpy(dest, src);
}

struct file *file_clear(struct file *f)
{
  if (f->frames) {
    for (u32 i = 0; i < f->nframes; i++)
      frame_del(f->frames[i]);
    free(f->frames);
    f->frames = NULL;
  }
  return file_init(f);
}

void file_del(struct file *f)
{
  file_clear(f);
  free(f);
}

b8 file_read(struct file *f, const char *filename)
{
  file_clear(f);
  f->state = bad;
  f->name = strndup(filename, strlen(filename));
  if (f->name == NULL)
    return false;
  FILE *stream = fopen(filename, "r");
  if (stream == NULL)
    return false;

  if (header_read(stream, &f->header) == false) {
    /* TODO Footer searching */
  }

  if (f->header.state == unset /* && f->footer.state == unset */ ) {
    return false;
  }
  /* TODO Header and Footer comparison exit */

  u32 size = f->header.size;
  // u32 size = f->header.state == unset ? f->footer.size : f->header.size;
  u32 pos = 0;
  while (pos < size) {
    u64 cur = ftell(stream);

    struct frame *frame = frame_new();
    if (frame == NULL)
      return false;
    if (frame_read(frame, stream) == false || frame->state == unset || frame->size == 0) {
      fseek(stream, cur, SEEK_SET);
      frame_del(frame);
      break ;
    }
    ++f->nframes;
    struct frame  **re_frames = realloc(f->frames, sizeof(*f->frames) * f->nframes);
    if (re_frames == NULL) {
      frame_del(frame);
      fclose(stream);
      return false;
    }
    f->frames = re_frames;
    pos += 10;
    pos += frame->size;
    f->frames[f->nframes - 1] = frame;
  }
  f->padding = size - pos;
  f->state = good;
  fclose(stream);
  return true;
}

FILE *file_stream(FILE *stream, const struct file *f)
{
  fwrite("Header:\n", sizeof(char), 8, stream);
  fwrite("\t", sizeof(char), 1, stream);
  header_stream(stream, &f->header);
  fwrite("\n", sizeof(char), 1, stream);

  if (f->header.flags.extended) {
    fwrite("Extended Header:\n", sizeof(char), 17, stream);
    fwrite("\t", sizeof(char), 1, stream);
    ext_header_stream(stream, &f->ext_header);
    fwrite("\n", sizeof(char), 1, stream);
  }

  if (f->nframes) {
    fwrite("Frames:\n", sizeof(char), 8, stream);
    for (u32 i = 0; i < f->nframes; i++) {
      fwrite("\t", sizeof(char), 1, stream);
      frame_stream(stream, f->frames[i]);
      fwrite("\n", sizeof(char), 1, stream);
    }
  }

  return stream;
}

FILE *file_stream_full(FILE *stream, const struct file *f)
{
  fwrite("Header:\n", sizeof(char), 8, stream);
  fwrite("\t", sizeof(char), 1, stream);
  header_stream_full(stream, &f->header);
  fwrite("\n", sizeof(char), 1, stream);

  if (f->header.flags.extended) {
    fwrite("Extended Header:\n", sizeof(char), 17, stream);
    fwrite("\t", sizeof(char), 1, stream);
    ext_header_stream_full(stream, &f->ext_header);
    fwrite("\n", sizeof(char), 1, stream);
  }

  if (f->nframes) {
    fwrite("Frames:\n", sizeof(char), 8, stream);
    for (u32 i = 0; i < f->nframes; i++) {
      fwrite("\t", sizeof(char), 1, stream);
      frame_stream_full(stream, f->frames[i]);
      fwrite("\n", sizeof(char), 1, stream);
    }
  }

  return stream;
}
