#ifndef ID3V240_EXT_HEADER_H
# define ID3V240_EXT_HEADER_H

#include "id3v240.h"

#include <stdio.h>

#define F_UPDATE 64   // 0b01000000
#define F_CRCPRES 32  // 0b00100000
#define F_RESTRICT 16 // 0b00010000

#define M_RES_TAGSIZE 192 // 0b11000000

enum res_tag {
  max128x1m = 0,
  max64x128k = 64,
  max32x40k = 128,
  max32x4k = 192,
};

#define F_RES_TXTENC 32   // 0b00100000

#define M_RES_TXTSIZE 24  // 0b00011000

enum res_txt {
  none_txt = 0,
  max1024 = 8,
  max128 = 16,
  max30 = 24,
};

#define F_RES_IMGENC 4    // 0b00000100

#define M_RES_IMGSIZE 3   // 0b00000011

enum res_img {
  none_img = 0,
  max256 = 1,
  max64 = 2,
  exact64 = 3,
};

struct ext_header {
  enum state  state;
  u32   pos;
  u32   size;
  struct {
    b8  update : 1;
    b8  crc : 1;
    b8  restriction : 1;
  } flags;
  u64   crc;
  struct {
    enum res_tag  tag_size;
    b8            txt_encode : 1;
    enum res_txt  txt_size;
    b8            img_encode : 1;
    enum res_img  img_size;
  } restriction;
};

/** @brief Initializes a `struct ext_header`. @return Returns a pointer to the initialized `struct ext_header`.
 *  @param `f` A pointer to the `struct ext_header` to be initialized.
 *
 *  `NULL` pointer guarded. Will return `NULL` if given `NULL`.
 */
struct ext_header *ext_header_init(struct ext_header *f);

/** @brief Allocated a `struct ext_header` and initializes it.
 *  @return Returns a pointer to the initialized `struct ext_header` on success;
 *    Retruns `NULL` on failure.
 */ 
struct ext_header *ext_header_new();

/** @brief Makes `dest` a copy of `src`.
 *  @return Returns a pointer to `dest`.
 *  @param `dest` The destination `struct ext_header` to become a copy of `src`.
 *  @param `src` The source `struct ext_header` who's data will be copied into
 *    `dest`.
 */
struct ext_header *ext_header_cpy(struct ext_header *dest, const struct ext_header *src);

/** @brief Allocates a new `struct ext_header` and copies the data from `src` into
 *    it.
 *  @return Returns the new `struct ext_header` on success; Returns `NULL` on failure
 *    to allocate memory for the `struct ext_header` or it's data.
 */
struct ext_header *ext_header_clone(const struct ext_header *src);

/** @brief Reinitializes a `struct ext_header`
 *  @return Returns the re-initialized `struct ext_header`.
 */
struct ext_header *ext_header_clear(struct ext_header *f);

/** @brief Frees the memory used by a `struct ext_header` and it's internal data.
 *  @param A pointer to the `struct ext_header` to be freed.
 */
void          ext_header_del(struct ext_header *f);

/** @brief Reads the data from `stream` into the `struct frame *f`.
 *  @return Returns `true` on successfull read; `false` on failure to read a frame
 */
b8            ext_header_read(struct ext_header *eh, FILE *stream);

/** @brief Updates a `struct ext_header`s internal fields to reflect changes
 *  made by the user.
 *  @return Always `true`.
 *  
 *  It is important to update all fields of a file before saving it to properly
 *    reflect changes made by the user. This function is called as part of the
 *    `file_update()` function.
 */
b8            ext_header_update(struct ext_header *eh);

/** @brief Sets the update flag for the extended header equal to the passed value.
 *  @return Always `true`.
 *  @param eh The `struct ext_header` that's supposed to be modified.
 *  @param set The value the flag should be set to. `true` or `false`
 *
 *  Modifies the size of the extended header for later use in `file_update()`.
 *  If you want to modify the flags in an extended header always do it through 
 *  the `ext_header_set_xxx()` set of functions
 */
b8            ext_header_set_update(struct ext_header *eh, b8 set);

/** @brief Sets the crc for the extended header equal to the passed value.
 *  @return Always `true`.
 *  @param eh The `struct ext_header` that's supposed to be modified.
 *  @param set The value the flag should be set to. `true` or `false`
 *  @param val The CRC that's supposed to be stored in the extended header.
 *
 *  Modifies the size of the extended header for later use in `file_update()`.
 *  If you want to modify the flags in an extended header always do it through 
 *  the `ext_header_set_xxx()` set of functions
 */
b8            ext_header_set_crc(struct ext_header *eh, b8 set, u64 crc);

/** @brief Sets the restrictions for the extended header equal to the passed value.
 *  @return Always `true`.
 *  @param eh The `struct ext_header` that's supposed to be modified.
 *  @param set The value the flag should be set to. `true` or `false`
 *  @param flags The ORd combination of the available Macros listed below.
 *
 *  The list of Macros available for the `flag` parameter:
 *    - F_RES_TXTENC :  If textencoding is present
 *    - F_RES_IMGENC :  If imgencoding is present
 *
 *    - a combination of values from these enums:
 *      - enum res_tag: Restrictions on the tag size:
 *        - max128x1m   = 0
 *        - max64x128k  = 64
 *        - max32x40k   = 128
 *        - max32x4k    = 192
 *      - enum res_txt
 *        - none_txt    = 0
 *        - max1024     = 8
 *        - max128      = 16
 *        - max30       = 24
 *      - enum res_img
 *        - none_img    = 0
 *        - max256      = 1
 *        - max64       = 2
 *        - exact64     = 3
 *
 *  Modifies the size of the extended header for later use in `file_update()`.
 *  If you want to modify the flags in an extended header always do it through 
 *  the `ext_header_set_xxx()` set of functions
 */
b8            ext_header_set_restrictions(struct ext_header *eh, b8 set, u8 flags);

/** @brief Writes the information from `struct ext_header *h` into the stream `stream`.
 *  @return Returns the stream.
 *  The data written to the stream is not the actual extended header data but the human
 *    readable representation of it.
 *
 *  The data shown is:
 *    - Whether the frames should be updated (True/False)
 *    - Whether a CRC is present (True/False)
 *    - Whether special restrictions are given (True/False)
 */
FILE *ext_header_stream(FILE *stream, const struct ext_header *h);

/** @brief Writes the information from `struct ext_header *h` into the stream `stream`.
 *  @return Returns the stream.
 *  
 *  The data written to the stream is not the actual extnded header data but the human
 *    readable representation of it.
 *
 *  The data fields shown are:
 *    - Whether the frames should be updated (True/False)
 *    - Whether a CRC is present (True/False)
 *      - The CRC
 *    - Whether special restrictions are given (True/False)
 *      - The tag size limit
 *      - Whether text restrictions are given (True/False)
 *      - The text size limit
 *      - Whether image restrictions are given (True/False)
 *      - The image size limit
 */
FILE *ext_header_stream_full(FILE *stream, const struct ext_header *h);

#endif
