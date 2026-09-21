#include "et.h"

#include "frame.h"

#include <stdlib.h>
#include <string.h>

void setup(void) {
    // executed before *every* non-skipped test
}

void teardown(void) {
    // executed after *every* non-skipped and non-failing test
}

TEST_GROUP("Frame") {

	TEST("init") {
		struct frame	f;

		init_frame(&f);

		VERIFY(f.state == unset);
		VERIFY(memcmp(f.id, "\0\0\0\0\0", 5) == 0);
		VERIFY(f.data == NULL);
		VERIFY(f.size == 0);
		VERIFY(f.pos == 0);
	}

	struct frame	*f = frame_new();
	TEST("new") {
		VERIFY(f != NULL);
		VERIFY(f->state == unset);
		VERIFY(memcmp(f->id, "\0\0\0\0\0", 5) == 0);
		VERIFY(f->data == NULL);
		VERIFY(f->size == 0);
		VERIFY(f->pos == 0);
	}

	f->state = good;
	f->data = malloc(sizeof(*f->data) * 50);
	for (int i = 0; i < 50; i++) {
		f->data[i] = i;
	}
	f->size = 50;
	f->pos = 3;

	struct frame	cpy;
	TEST("cpy") {
		frame_cpy(&cpy, f);

		VERIFY(cpy.state == f->state);
		VERIFY(memcmp(cpy.id, f->id, 5) == 0);
		VERIFY(memcmp(cpy.data, f->data, 50) == 0);
		VERIFY(cpy.size == f->size);
		VERIFY(cpy.pos == f->pos);
	}

	struct frame	*clone;
	TEST("clone") {
		clone = frame_clone(f);

		VERIFY(clone->state == f->state);
		VERIFY(memcmp(clone->id, f->id, 5) == 0);
		VERIFY(memcmp(clone->data, f->data, 50) == 0);
		VERIFY(clone->size == f->size);
		VERIFY(clone->pos == f->pos);
	}

	TEST("clear") {
		frame_clear(&cpy);

		VERIFY(cpy.state == unset);
		VERIFY(memcmp(cpy.id, "\0\0\0\0\0", 5) == 0);
		VERIFY(cpy.data == NULL);
		VERIFY(cpy.size == 0);
		VERIFY(cpy.pos == 0);
	}

	SKIP_TEST("del") {
		frame_del(clone);
	}

	frame_del(f);
}
