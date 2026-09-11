#include "et.h"

#include "utils.h"

#include <stdlib.h>
#include <string.h>

void setup(void) {
    // executed before *every* non-skipped test
}

void teardown(void) {
    // executed after *every* non-skipped and non-failing test
}

TEST_GROUP("boolstr") {

	TEST("0 == False") {
		const u8 *ret = boolstr(0);
		VERIFY(memcmp(ret, "False", 5) == 0);
	}

	TEST("1 == True") {
		const u8 *ret = boolstr(1);
		VERIFY(memcmp(ret, "True", 5) == 0);
	}

	TEST("123 == True") {
		const u8 *ret = boolstr(123);
		VERIFY(memcmp(ret, "True", 5) == 0);
	}
}
