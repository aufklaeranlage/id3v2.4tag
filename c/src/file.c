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
  footer_clear(&f->footer);
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


  if (!header_read(stream, &f->header)) {
    /* TODO outsource ?? */
    i32 offset = -50;
    char buf[50];
    fseek(stream, offset, SEEK_END);
    while (ftell(stream) <= 0) {
      fread(buf, sizeof(*buf), 50, stream);
      char *pos = memchr(buf, '3', 50);
      if (pos == NULL || pos -buf > 47 || pos[1] != 'D' || pos[2] != 'I') {
        offset -= 47;
        fseek(stream, offset, SEEK_END);
        continue ;
      }
      fseek(stream, offset + (pos - buf), SEEK_END);
      if (footer_read(stream, &f->footer)) {
        offset = f->footer.pos + 10 - f->footer.size;
        fseek(stream, offset, SEEK_END);
        if (!header_read(stream, &f->header))
          return false;
      } else {
        return false;
      }
    }
  }

  if (f->header.flags.extended)
    ext_header_read(stream, &f->ext_header);

  if (f->header.state == unset && f->footer.state == unset ) {
    return true;
  }
  /* TODO Header and Footer comparison exit */

  u32 size = f->header.size;
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

  if (f->padding >= 10) {
    fseek(stream, f->header.pos + f->header.size - 10, SEEK_SET);
    if (f->footer.state != unset || footer_read(stream, &f->footer))
      f->padding -= 10;
  }

  f->state = good;
  fclose(stream);
  return true;
}

FILE *file_stream(FILE *stream, const struct file *f)
{
  if (f->header.state == good) {
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
  }

  if (f->footer.state == good) {
    fwrite("Footer:\n", sizeof(char), 8, stream);
    fwrite("\t", sizeof(char), 1, stream);
    footer_stream(stream, &f->footer);
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

struct frame **file_get_frames_by_id(const struct file *f, const char id[4])
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

static b8 file_write_header(struct file *f, FILE *stream)
{
  char  buf[10];
  memcpy(buf, "ID3", 3);
  buf[3] = (u8)f->header.version.major;
  buf[4] = (u8)f->header.version.minor;
  buf[5] = !(!(f->header.flags.unsynchronisation)) * F_UNSYN |
    !(!(f->header.flags.extended)) * F_EXTEN |
    !(!(f->header.flags.experimental)) * F_EXPER |
    !(!(f->header.flags.footer)) * F_FOOTR;
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
  if (ferror(stream))
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

static b8 file_update_size_pos(struct file *f) {
  u32 size = f->header.size;
  u32 new_size = 0;
  u32 pos = f->header.flags.extended ? 10 : 10 + f->ext_header.size;
  for (u32 i = 0; i < f->nframes; i++)  {
    f->frames[i]->pos = pos + new_size;
    new_size += f->frames[i]->size + 10;
  }
  if (f->header.state != unset && size < new_size + f->size_update.wanted_padding) {
    f->size_update.needed = true;
    f->size_update.diff = new_size - size + f->size_update.wanted_padding;
  }
  if (f->header.state != unset)
    f->header.size = new_size;
  if (f->footer.state != unset)
    f->footer.size = new_size;
  return true;
}

b8 file_update(struct file *f)
{
  if (f->header.state != unset) {
    header_update(&f->header);
    if (f->header.flags.extended)
      ext_header_update(&f->ext_header);
    if (f->header.flags.footer) {
      footer_update(&f->footer);
    }
  }
  file_update_size_pos(f);
  return true;
}

b8 file_save(struct file *f)
{
  if (!file_update(f))
    return false;

  FILE *stream = fopen(f->name, "r+");
  if (stream == NULL)
    return false;

  if (f->size_update.needed) {
    if (move_file_contents(f->name, f->header.size, f->size_update.diff) == false)
      return false;
  }
  fseek(stream, 0, SEEK_SET);
  if (f->header.state != unset) {
    if (!file_write_header(f, stream))
      return false;
    if (f->header.flags.extended == true)
      if (!file_write_ext_header(f, stream))
        return false;
  }
  if (!file_write_frames(f, stream))
    return false;
  if (f->footer.state != unset) {
    if (!file_write_header(f, stream))
      return false;
  }

  return true;
}

b8 file_add_header(struct file *f)
{
  if (f->header.state != unset)
    return false;
  header_init(&f->header);
  return true;
}

b8 file_add_ext_header(struct file *f)
{
  if (f->header.state == unset || f->ext_header.state != unset)
    return false;
  if (!f->header.flags.extended) {
    header_set_flags(&f->header, F_EXTEN);
    f->header.size += 6;
    ext_header_init(&f->ext_header);
  }
  return true;
}

b8 file_add_footer(struct file *f)
{
  if (f->header.state == unset || f->footer.state != unset)
    return false;
  if (!f->header.flags.footer) {
    header_set_flags(&f->header, F_FOOTR);
    f->header.size += 10;
    footer_cpy_header(&f->footer, &f->header);
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
