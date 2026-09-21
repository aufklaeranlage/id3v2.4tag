#include "et.h"

#include "ext_header.h"

#include <stdlib.h>
#include <string.h>

void setup(void) {
  // executed before *every* non-skipped test
}

void teardown(void) {
  // executed after *every* non-skipped and non-failing test
}

TEST_GROUP("Extended Header") {

  TEST("init") {
    struct ext_header eh;

    ext_header_init(&eh);

    VERIFY(eh.state == unset);
    VERIFY(eh.pos == 0);
    VERIFY(eh.size == 6);
    VERIFY(eh.flags.update == false);
    VERIFY(eh.flags.crc == false);
    VERIFY(eh.flags.restriction == false);
    VERIFY(eh.crc == 0);
    VERIFY(eh.restriction.tag_size == max128x1m);
    VERIFY(eh.restriction.txt_encode == false);
    VERIFY(eh.restriction.txt_size == none_txt);
    VERIFY(eh.restriction.img_encode == false);
    VERIFY(eh.restriction.img_size == none_img);
  }

  struct ext_header	*eh = ext_header_new();
  TEST("new") {
    VERIFY(eh != NULL);

    VERIFY(eh->state == unset);
    VERIFY(eh->pos == 0);
    VERIFY(eh->size == 6);
    VERIFY(eh->flags.update == false);
    VERIFY(eh->flags.crc == false);
    VERIFY(eh->flags.restriction == false);
    VERIFY(eh->crc == 0);
    VERIFY(eh->restriction.tag_size == max128x1m);
    VERIFY(eh->restriction.txt_encode == false);
    VERIFY(eh->restriction.txt_size == none_txt);
    VERIFY(eh->restriction.img_encode == false);
    VERIFY(eh->restriction.img_size == none_img);
  }

  eh->state = good;

  TEST("set_update - enable") {
    VERIFY(ext_header_set_update(eh, true) == true);

    VERIFY(eh->flags.update == true);
    VERIFY(eh->size == 7);
  }

  TEST("set_update - enabled no size change") {
    VERIFY(ext_header_set_update(eh, true) == true);

    VERIFY(eh->flags.update == true);
    VERIFY(eh->size == 7);
  }

  TEST("set_update - disable") {
    VERIFY(ext_header_set_update(eh, false) == true);

    VERIFY(eh->flags.update == false);
    VERIFY(eh->size == 6);
  }

  TEST("set_update - disabled no size change") {
    VERIFY(ext_header_set_update(eh, false) == true);

    VERIFY(eh->flags.update == false);
    VERIFY(eh->size == 6);
  }

  TEST("set_crc - enable") {
    u64 val = 400;
    VERIFY(ext_header_set_crc(eh, true, val) == true);

    VERIFY(eh->flags.crc == true);
    VERIFY(eh->size == 12);
    VERIFY(eh->crc == val);
  }

  TEST("set_crc - enabled no size change | overwrite of value") {
    u64 val = 200;
    VERIFY(ext_header_set_crc(eh, true, val) == true);

    VERIFY(eh->flags.crc == true);
    VERIFY(eh->size == 12);
    VERIFY(eh->crc == val);
  }

  TEST("set_crc - disable") {
    VERIFY(ext_header_set_crc(eh, false, 0) == true);

    VERIFY(eh->flags.crc == false);
    VERIFY(eh->size == 6);
  }

  TEST("set_crc - disabled no size change") {
    VERIFY(ext_header_set_crc(eh, false, 0) == true);

    VERIFY(eh->flags.crc == false);
    VERIFY(eh->size == 6);
  }

  TEST("set_restrictions - enable") {
    u64 val = F_RES_IMGENC | max32x4k | max128 | exact64;
    VERIFY(ext_header_set_restrictions(eh, true, val) == true);

    VERIFY(eh->flags.restriction == true);
    VERIFY(eh->size == 8);
    VERIFY(eh->restriction.tag_size == max32x4k);
    VERIFY(eh->restriction.txt_encode == false);
    VERIFY(eh->restriction.txt_size == max128);
    VERIFY(eh->restriction.img_encode == true);
    VERIFY(eh->restriction.img_size == exact64);
  }

  TEST("set_restrictions - enabled no size change | overwrite of value") {
    u64 val = F_RES_TXTENC | max64x128k | max1024 | max256 ;
    VERIFY(ext_header_set_restrictions(eh, true, val) == true);

    VERIFY(eh->flags.restriction == true);
    VERIFY(eh->size == 8);
    VERIFY(eh->restriction.tag_size == max64x128k);
    VERIFY(eh->restriction.txt_encode == true);
    VERIFY(eh->restriction.txt_size == max1024);
    VERIFY(eh->restriction.img_encode == false);
    VERIFY(eh->restriction.img_size == max256);
  }

  TEST("set_restrictions - disable") {
    VERIFY(ext_header_set_restrictions(eh, false, 0) == true);

    VERIFY(eh->flags.restriction == false);
    VERIFY(eh->size == 6);
  }

  TEST("set_restrictions - disabled no size change") {
    VERIFY(ext_header_set_restrictions(eh, false, 0) == true);

    VERIFY(eh->flags.restriction == false);
    VERIFY(eh->size == 6);
  }

  struct ext_header cpy;
  TEST("cpy") {
    ext_header_cpy(&cpy, eh);

    VERIFY(cpy.state == eh->state);
    VERIFY(cpy.size == eh->size);
    VERIFY(cpy.pos == eh->pos);
    VERIFY(cpy.flags.update == eh->flags.update);
    VERIFY(cpy.flags.crc == eh->flags.crc);
    VERIFY(cpy.flags.restriction == eh->flags.restriction);
    VERIFY(cpy.crc == eh->crc);
    VERIFY(cpy.restriction.tag_size == eh->restriction.tag_size);
    VERIFY(cpy.restriction.txt_encode == eh->restriction.txt_encode);
    VERIFY(cpy.restriction.txt_size == eh->restriction.txt_size);
    VERIFY(cpy.restriction.img_encode == eh->restriction.img_encode);
    VERIFY(cpy.restriction.img_size == eh->restriction.img_size);
  }

  struct ext_header *clone;
  TEST("clone") {
    clone = ext_header_clone(eh);

    VERIFY(clone->state == eh->state);
    VERIFY(clone->size == eh->size);
    VERIFY(clone->pos == eh->pos);
    VERIFY(clone->flags.update == eh->flags.update);
    VERIFY(clone->flags.crc == eh->flags.crc);
    VERIFY(clone->flags.restriction == eh->flags.restriction);
    VERIFY(clone->crc == eh->crc);
    VERIFY(clone->restriction.tag_size == eh->restriction.tag_size);
    VERIFY(clone->restriction.txt_encode == eh->restriction.txt_encode);
    VERIFY(clone->restriction.txt_size == eh->restriction.txt_size);
    VERIFY(clone->restriction.img_encode == eh->restriction.img_encode);
    VERIFY(clone->restriction.img_size == eh->restriction.img_size);
  }

  TEST("clear") {
    ext_header_clear(&cpy);

    VERIFY(cpy.state == unset);
    VERIFY(cpy.pos == 0);
    VERIFY(cpy.size == 6);
    VERIFY(cpy.flags.update == false);
    VERIFY(cpy.flags.crc == false);
    VERIFY(cpy.flags.restriction == false);
    VERIFY(cpy.crc == 0);
    VERIFY(cpy.restriction.tag_size == max128x1m);
    VERIFY(cpy.restriction.txt_encode == false);
    VERIFY(cpy.restriction.txt_size == none_txt);
    VERIFY(cpy.restriction.img_encode == false);
    VERIFY(cpy.restriction.img_size == none_img);
  }

  SKIP_TEST("del") {
    ext_header_del(clone);
  }

  ext_header_del(eh);
}
