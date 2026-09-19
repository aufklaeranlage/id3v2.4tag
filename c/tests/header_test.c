#include "et.h"

#include "header.h"

#include <stdlib.h>
#include <string.h>

void setup(void) {
  // executed before *every* non-skipped test
}

void teardown(void) {
  // executed after *every* non-skipped and non-failing test
}

TEST_GROUP("Header") {

  TEST("init") {
    struct header	h;

    header_init(&h);

    VERIFY(h.state == unset);
    VERIFY(h.version.major == 0);
    VERIFY(h.version.minor == 0);
    VERIFY(h.flags.unsynchronisation == false);
    VERIFY(h.flags.extended == false);
    VERIFY(h.flags.experimental == false);
    VERIFY(h.flags.footer == false);
    VERIFY(h.flags.uncleared == false);
    VERIFY(h.size == 0);
    VERIFY(h.pos == 0);
  }

  struct header	*h = header_new();
  TEST("new") {
    VERIFY(h != NULL);

    VERIFY(h->state == unset);
    VERIFY(h->version.major == 0);
    VERIFY(h->version.minor == 0);
    VERIFY(h->flags.unsynchronisation == false);
    VERIFY(h->flags.extended == false);
    VERIFY(h->flags.experimental == false);
    VERIFY(h->flags.footer == false);
    VERIFY(h->flags.uncleared == false);
    VERIFY(h->size == 0);
    VERIFY(h->pos == 0);
  }

  h->state = good;

  TEST("set_flags - baseline") {
    VERIFY(header_set_flags(h, F_UNSYN | F_EXTEN) == true);

    VERIFY(h->flags.unsynchronisation == true);
    VERIFY(h->flags.extended == true);
    VERIFY(h->flags.experimental == false);
    VERIFY(h->flags.footer == false);
    VERIFY(h->flags.uncleared == false);
  }

  TEST("set_flags - overwrite") {

    VERIFY(header_set_flags(h, F_UNSYN | F_EXPER) != 0);

    VERIFY(h->flags.unsynchronisation == true);
    VERIFY(h->flags.extended == false);
    VERIFY(h->flags.experimental == true);
    VERIFY(h->flags.footer == false);
    VERIFY(h->flags.uncleared == false);
  }

  struct header cpy;
  TEST("cpy") {
    header_cpy(&cpy, h);

    VERIFY(cpy.state == h->state);
    VERIFY(cpy.version.major == h->version.major);
    VERIFY(cpy.version.minor == h->version.minor);
    VERIFY(cpy.flags.unsynchronisation == h->flags.unsynchronisation);
    VERIFY(cpy.flags.extended == h->flags.extended);
    VERIFY(cpy.flags.experimental == h->flags.experimental);
    VERIFY(cpy.flags.footer == h->flags.footer);
    VERIFY(cpy.flags.uncleared == h->flags.uncleared);
    VERIFY(cpy.size == h->size);
    VERIFY(cpy.pos == h->pos);
  }

  struct header *clone;
  TEST("clone") {
    clone = header_clone(h);

    VERIFY(clone->state == h->state);
    VERIFY(clone->version.major == h->version.major);
    VERIFY(clone->version.minor == h->version.minor);
    VERIFY(clone->flags.unsynchronisation == h->flags.unsynchronisation);
    VERIFY(clone->flags.extended == h->flags.extended);
    VERIFY(clone->flags.experimental == h->flags.experimental);
    VERIFY(clone->flags.footer == h->flags.footer);
    VERIFY(clone->flags.uncleared == h->flags.uncleared);
    VERIFY(clone->size == h->size);
    VERIFY(clone->pos == h->pos);
  }

  TEST("clear") {
    header_clear(&cpy);

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
    header_del(clone);
  }

  header_del(h);
}
