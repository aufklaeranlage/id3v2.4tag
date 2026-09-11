#include "frame.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"

struct frame *init_frame(struct frame *f)
{
	if (f == NULL)
		return f;
	f->state = unset;
	memset(f->id, '\0', 5);
	f->data = NULL;
	f->size = 0;
	f->pos = 0;
	return f;
}

struct frame *frame_new()
{
	struct frame	*f = malloc(sizeof(*f));
	return init_frame(f);
}

struct frame *frame_cpy(struct frame *dest, struct frame *src)
{
	char	*data;
	data = malloc(sizeof(dest->data) * (src->size + 1));
	if (data == NULL)
		return NULL;
	memcpy(data, src->data, src->size + 1);
	frame_clear(dest);
	memcpy(dest, src, sizeof(*src));
	dest->data = data;
	return dest;
}

struct frame *frame_clone(struct frame *src)
{
	struct frame	*dest;
	dest = malloc(sizeof(*dest));
	if (dest == NULL)
		return NULL;
	dest->data = NULL;
	if (frame_cpy(dest, src) == NULL) {
		free(dest);
		return dest;
	}
	return dest;
}

struct frame *frame_clear(struct frame *f)
{
	free(f->data);
	return init_frame(f);
}

void frame_del(struct frame *f)
{
	frame_clear(f);
	free(f);
}

b8 frame_read(struct frame *f, FILE *stream)
{
	frame_clear(f);
	u8 headerbuf[10];

	u64 cur = ftell(stream);

	f->state = bad;
	f->pos = cur;
	if (fread(headerbuf, sizeof(u8), 10, stream) < 0)
		return false;
	f->size = read_synchsafe_u32((u8 *)headerbuf);

	f->status.preserve_tag = headerbuf[8] & F_TAGPRES;
	f->status.preserve_file = headerbuf[8] & F_FILPRES;
	f->status.read_only = headerbuf[8] & F_RDONLY;

	f->format.grouped = headerbuf[9] & F_GROUPED;
	f->format.compressed = headerbuf[9] & F_COMPRES;
	f->format.encrypted = headerbuf[9] & F_ENCRYPT;
	f->format.unsynchronisation = headerbuf[9] & F_UNSYNC;
	f->format.length_indicated = headerbuf[9] & F_LENINDI;
	
	f->data = malloc(sizeof(*f->data) * (f->size + 1));
	if (f->data == NULL || fread(f->data, sizeof(char), f->size, stream) < 0) {
		fseek(stream, cur, SEEK_SET);
		return false;
	}
	f->data[f->size] = '\0';
	f->state = good;
	return true;
}

FILE *frame_stream(FILE *stream, const struct frame *f)
{
	fprintf(stream, "id: %4s, data: %s", f->id, f->data);
	return stream;
}

FILE *frame_stream_full(FILE *stream, const struct frame *f)
{
	fprintf(stream, "id: %4s, pos: %u, size: %u, \
status { preserve tag: %s, preserve file: %s, read only: %s }, \
format { grouped: %s, compressed: %s, encrypted: %s, unsynchronisation: %s, length indicated: %s }, \
data: %s",
		f->id, f->pos, f->size,
		boolstr(f->status.preserve_tag), boolstr(f->status.preserve_file), boolstr(f->status.read_only),
		boolstr(f->format.grouped), boolstr(f->format.compressed), boolstr(f->format.encrypted), boolstr(f->format.unsynchronisation), boolstr(f->format.length_indicated),
		f->data);
	return stream;
}
