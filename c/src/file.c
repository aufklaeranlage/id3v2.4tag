#include "file.h"

#include "utils.h"

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

struct frame **file_get_frames(const struct file *f, const char id[4])
{
  u32 nframes = 0;
  for (u32 i = 0; i < f->nframes; i++) {
    if (memcmp(id, f->frames[i]->id, 4) == 0)
      ++nframes;
  }
  struct frame  **ret = calloc(sizeof(*ret), nframes + 1);
  if (ret == NULL)
    return NULL;
  u32 retpos = 0;
  for (u32 i = 0; i < f->nframes; i++) {
    if (memcmp(id, f->frames[i]->id, 4) == 0) {
      ret[retpos++] = f->frames[i];
    }
  }
  return ret;
}

static void file_update_frame_pos(struct file *f)
{
  if (f->nframes == 0)
    return ;
  u32 pos = f->frames[0]->pos;
  for (u32 i = 0; i < f->nframes; i++) {
    f->frames[i]->pos = pos;
    pos += f->frames[i]->size;
  }
}

static b8 file_write_header(struct file *f, FILE *stream)
{
  char  buf[10];
  memcpy(buf, "ID3", 3);
  buf[3] = (u8)f->header.version.major;
  buf[4] = (u8)f->header.version.minor;
  buf[5] = !(!(f->header.flags.unsynchronisation)) * F_UNSYN |
    !(!(f->header.flags.extended)) * F_EXTEN |
    !(!(f->header.flags.experimental)) * F_EXPER |
    !(!(f->header.flags.footer)) * F_FOOTR |
    !(!(f->header.flags.uncleared)) * F_UNCLR;
  write_synchsafe_u28((u8 *)buf + 6, f->header.size);
  fwrite(buf, sizeof(*buf), 10, stream);
  return !ferror(stream);
}

static b8 file_write_ext_header(struct file *f, FILE *stream)
{
  char  buf[15]; /* Max size of ext header */
  write_synchsafe_u28((u8 *)buf, f->ext_header.size);
  buf[4] = 1; /* Number of flag bytes */
  buf[5] = !(!(f->ext_header.flags.update)) * F_UPDATE |
    !(!(f->ext_header.flags.crc)) * F_CRCPRES |
    !(!(f->ext_header.flags.restriction)) * F_RESTRICT;
  u32 bufpos = 6;
  if (f->ext_header.flags.update)
    buf[bufpos++] = '\0'; /* Data can be anything this is more of a confirmation byte */
  if (f->ext_header.flags.crc) {
    buf[bufpos++] = 5;
    write_synchsafe_u35((u8 *)buf + bufpos, f->ext_header.crc);
    bufpos += 5;
  }
  if (f->ext_header.flags.restriction) {
    buf[bufpos++] = 1;
    buf[bufpos++] = f->ext_header.restriction.tag_size |
      !(!(f->ext_header.restriction.txt_encode)) * F_RES_TXTENC |
      f->ext_header.restriction.txt_size |
      !(!(f->ext_header.restriction.img_encode)) * F_RES_IMGENC |
      f->ext_header.restriction.img_size;
  }
  fwrite(buf, sizeof(*buf), bufpos, stream);
  return !ferror(stream);
}

static b8 file_write_frame(struct frame *f, FILE *stream) {
  char  headerbuf[10];
  memcpy(headerbuf, f->id, 4);
  write_synchsafe_u28((u8 *)headerbuf + 4, f->size);
  headerbuf[8] = !(!(f->status.preserve_tag)) * F_TAGPRES |
    !(!(f->status.preserve_file)) * F_FILPRES |
    !(!(f->status.read_only)) * F_RDONLY;
  headerbuf[9] = !(!(f->format.grouped)) * F_GROUPED |
    !(!(f->format.compressed)) * F_COMPRES |
    !(!(f->format.encrypted)) * F_ENCRYPT |
    !(!(f->format.unsynchronisation)) * F_UNSYNC |
    !(!(f->format.length_indicated)) * F_LENINDI;

  fwrite(headerbuf, sizeof(*f->id), 10, stream);
  if (!ferror(stream))
    return false;
  fwrite(f->data, sizeof(*f->data), f->size, stream);
  return !ferror(stream);
}

static b8 file_write_frames(struct file *f, FILE *stream)
{
  for (u32 i = 0; i < f->nframes; i++) {
    if (file_write_frame(f->frames[i], stream) == false)
      return i;
  }
  return 0;
}

b8 file_save(struct file *f)
{
  FILE *stream = fopen(f->name, "rw");
  if (stream == NULL)
    return false;
  u32 size = f->header.state == unset ? f->footer.size : f->header.size;
  u32 new_size = 0;
  for (u32 i = 0; i < f->nframes; i++)
    new_size += f->frames[i]->size;

  if (new_size > f->header.size) {
    if (move_file_contents(f->name, size, new_size - size) == false)
      return false;
  }
  file_update_frame_pos(f);
  if (f->header.state != unset)
    f->header.size = new_size;
  if (f->footer.state != unset)
    f->footer.size = new_size;
  fseek(stream, 0, SEEK_SET);
  if (!file_write_header(f, stream) ||
    !file_write_ext_header(f, stream) ||
    !file_write_frames(f, stream)) {
    return false;
  }

  return true;
}

b8 file_add_frame(struct file *file, struct frame *frame, u32 flags)
{
  (void)flags;
  /* TODO Flesh out code for special scenarios and flags */
  struct frame  *cpy = frame_clone(frame);
  if (cpy == NULL)
    return false;
  struct frame  **new_frames = realloc(file->frames, sizeof(*file->frames) * (file->nframes + 1));
  if (new_frames == NULL) {
    frame_del(cpy);
    return false;
  }
  u32 pos = file->frames[file->nframes - 1]->pos + file->frames[file->nframes - 1]->size;
  cpy->pos = pos;
  new_frames[file->nframes++] = cpy;
  file->frames = new_frames;
  return true;
}
