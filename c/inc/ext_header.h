#ifndef ID3V240_EXT_HEADER_H
# define ID3V240_EXT_HEADER_H

#include "id3v240.h"

#include <stdio.h>

#define F_UPDATE 64   // 0b01000000
#define F_CRCPRES 32  // 0b00100000
#define F_RESTRICT 16 // 0b00010000

#define M_RES_TAGSIZE 192 // 0b11000000
#define F_RES_TAG1M 0     // 0b00000000
#define F_RES_TAG128K 64  // 0b01000000
#define F_RES_TAG40K 128  // 0b10000000
#define F_RES_TAG4K 192   // 0b11000000

#define F_RES_TXTENC 32   // 0b00100000

#define M_RES_TXTSIZE 24  // 0b00011000
#define F_RES_TXTNONE 0   // 0b00000000
#define F_RES_TXT1024 64  // 0b01000000
#define F_RES_TXT128 128  // 0b10000000
#define F_RES_TXT30 192   // 0b11000000

#define F_RES_IMGENC 4    // 0b00000100

#define M_RES_IMGSIZE 3   // 0b00000011
#define F_RES_IMGNONE 0   // 0b00000000
#define F_RES_IMG256 64   // 0b01000000
#define F_RES_IMG64 128   // 0b10000000
#define F_RES_IMGEX64 192 // 0b11000000

enum res_tag {
  max128x1m,
  max64x128k,
  max32x40k,
  max32x4k,
};

enum res_txt {
  none_txt,
  max1024,
  max128,
  max30,
};

enum res_img {
  none_img,
  max256,
  max64,
  exact64,
};

struct ext_header {
  enum state  state;
  u32   size;
  struct {
    b8  update;
    b8  crc;
    b8 restriction;
  } flags;
  u64   crc;
  struct {
    enum res_tag  tag_size;
    b8            txt_encode;
    enum res_txt  txt_size;
    b8            img_encode;
    enum res_img  img_size;
  } restriction;
};

/**	@brief Initializes a `struct ext_header`. @return Returns a pointer to the initialized `struct ext_header`.
 *	@param `f` A pointer to the `struct ext_header` to be initialized.
 *
 *	`NULL` pointer guarded. Will return `NULL` if given `NULL`.
 */
struct ext_header	*ext_header_init(struct ext_header *f);

/**	@brief Allocated a `struct ext_header` and initializes it.
 *	@return Returns a pointer to the initialized `struct ext_header` on success;
 *		Retruns `NULL` on failure.
 */ 
struct ext_header	*ext_header_new();

/**	@brief Makes `dest` a copy of `src`.
 *	@return Returns a pointer to `dest`.
 *	@param `dest` The destination `struct ext_header` to become a copy of `src`.
 *	@param `src` The source `struct ext_header` who's data will be copied into
 *		`dest`.
 */
struct ext_header	*ext_header_cpy(struct ext_header *dest, const struct ext_header *src);

/**	@brief Allocates a new `struct ext_header` and copies the data from `src` into
 *		it.
 *	@return Returns the new `struct ext_header` on success; Returns `NULL` on failure
 *		to allocate memory for the `struct ext_header` or it's data.
 */
struct ext_header	*ext_header_clone(const struct ext_header *src);

/**	@brief Reinitializes a `struct ext_header`
 *	@return Returns the re-initialized `struct ext_header`.
 */
struct ext_header	*ext_header_clear(struct ext_header *f);

/**	@brief Frees the memory used by a `struct ext_header` and it's internal data.
 *	@param A pointer to the `struct ext_header` to be freed.
 */
void					ext_header_del(struct ext_header *f);

/** @brief Reads the data from `stream` into the `struct frame *f`.
 *  @return Returns `true` on successfull read; `false` on failure to read a frame
 */
b8 ext_header_read(FILE *stream, struct ext_header *h);

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
