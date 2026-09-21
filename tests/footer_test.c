#include "et.h"

#include "header.h"
#include "footer.h"

#include <stdlib.h>
#include <string.h>

void setup(void) {
  // executed before *every* non-skipped test
}

void teardown(void) {
  // executed after *every* non-skipped and non-failing test
}

TEST_GROUP("Footer") {

  TEST("init") {
    struct footer	f;

    footer_init(&f);

    VERIFY(f.state == unset);
    VERIFY(f.version.major == 0);
    VERIFY(f.version.minor == 0);
    VERIFY(f.flags.unsynchronisation == false);
    VERIFY(f.flags.extended == false);
    VERIFY(f.flags.experimental == false);
    VERIFY(f.flags.footer == false);
    VERIFY(f.flags.uncleared == false);
    VERIFY(f.size == 0);
    VERIFY(f.pos == 0);
  }

  struct footer	*f = footer_new();
  TEST("new") {
    VERIFY(f != NULL);

    VERIFY(f->state == unset);
    VERIFY(f->version.major == 0);
    VERIFY(f->version.minor == 0);
    VERIFY(f->flags.unsynchronisation == false);
    VERIFY(f->flags.extended == false);
    VERIFY(f->flags.experimental == false);
    VERIFY(f->flags.footer == false);
    VERIFY(f->flags.uncleared == false);
    VERIFY(f->size == 0);
    VERIFY(f->pos == 0);
  }

  f->state = good;
  f->version.major = 2;
  f->version.minor = 4;
  f->flags.unsynchronisation = true;
  f->flags.extended = true;
  f->size = 500;
  f->pos = 490;

  struct footer cpy;
  TEST("cpy") {
    footer_cpy(&cpy, f);

    VERIFY(cpy.state == f->state);
    VERIFY(cpy.version.major == f->version.major);
    VERIFY(cpy.version.minor == f->version.minor);
    VERIFY(cpy.flags.unsynchronisation == f->flags.unsynchronisation);
    VERIFY(cpy.flags.extended == f->flags.extended);
    VERIFY(cpy.flags.experimental == f->flags.experimental);
    VERIFY(cpy.flags.footer == f->flags.footer);
    VERIFY(cpy.flags.uncleared == f->flags.uncleared);
    VERIFY(cpy.size == f->size);
    VERIFY(cpy.pos == f->pos);
  }

  struct footer *clone;
  TEST("clone") {
    clone = footer_clone(f);

    VERIFY(clone->state == f->state);
    VERIFY(clone->version.major == f->version.major);
    VERIFY(clone->version.minor == f->version.minor);
    VERIFY(clone->flags.unsynchronisation == f->flags.unsynchronisation);
    VERIFY(clone->flags.extended == f->flags.extended);
    VERIFY(clone->flags.experimental == f->flags.experimental);
    VERIFY(clone->flags.footer == f->flags.footer);
    VERIFY(clone->flags.uncleared == f->flags.uncleared);
    VERIFY(clone->size == f->size);
    VERIFY(clone->pos == f->pos);
  }

  TEST("clear") {
    footer_clear(&cpy);

    VERIFY(cpy.state == unset);
    VERIFY(cpy.version.major == 0);
    VERIFY(cpy.version.minor == 0);
    VERIFY(cpy.flags.unsynchronisation == false);
    VERIFY(cpy.flags.extended == false);
    VERIFY(cpy.flags.experimental == false);
    VERIFY(cpy.flags.footer == false);
    VERIFY(cpy.flags.uncleared == false);
    VERIFY(cpy.size == 0);
    VERIFY(cpy.pos == 0);
  }

  SKIP_TEST("del") {
    footer_del(clone);
  }

  struct header h;
  header_init(&h);
  header_set_flags(&h, F_UNSYN | F_EXTEN);
  h.size = 40;
  h.pos = 100;

  TEST("cpy_header") {
    VERIFY(footer_cpy_header(f, &h) != 0);

    VERIFY(h.state == f->state);
    VERIFY(h.version.major == f->version.major);
    VERIFY(h.version.minor == f->version.minor);
    VERIFY(h.flags.unsynchronisation == f->flags.unsynchronisation);
    VERIFY(h.flags.extended == f->flags.extended);
    VERIFY(h.flags.experimental == f->flags.experimental);
    VERIFY(h.flags.footer == f->flags.footer);
    VERIFY(h.flags.uncleared == f->flags.uncleared);
    VERIFY(h.size == f->size);
    VERIFY(f->pos == h.pos + h.size - 10);
  }

  footer_del(f);
}
