#ifndef ID3V240_FOOTER_H
# define ID3V240_FOOTER_H

#include "id3v240.h"

#include <stdio.h>

#define F_UNSYN 128 // 0b10000000
#define F_EXTEN 64  // 0b01000000
#define F_EXPER 32  // 0b00100000
#define F_FOOTR 16  // 0b00010000
#define F_UNCLR 15   // 0b00001111

struct footer {
  enum state  state;
  struct {
    u32 major;
    u32 minor;
  } version;
  struct {
    b8  unsynchronisation;
    b8  extended;
    b8  experimental;
    b8  footer;
    b8  uncleared;
  } flags;
  u32 size;
};

/** @brief Initializes a `struct footer`.
 *  @return Returns a pointer to the initialized `struct footer`.
 *  @param `f` A pointer to the `struct footer` to be initialized.
 *
 *  `NULL` pointer guarded. Will return `NULL` if given `NULL`.
 */
struct footer *footer_init(struct footer *f);

/** @brief Allocated a `struct footer` and initializes it.
 *  @return Returns a pointer to the initialized `struct footer` on success;
 *    Retruns `NULL` on failure.
 */ 
struct footer *footer_new();

/** @brief Makes `dest` a copy of `src`.
 *  @return Returns a pointer to `dest`.
 *  @param `dest` The destination `struct footer` to become a copy of `src`.
 *  @param `src` The source `struct footer` who's data will be copied into
 *    `dest`.
 */
struct footer *footer_cpy(struct footer *dest, const struct footer *src);

/** @brief Allocates a new `struct footer` and copies the data from `src` into
 *    it.
 *  @return Returns the new `struct footer` on success; Returns `NULL` on failure
 *    to allocate memory for the `struct footer` or it's data.
 */
struct footer *footer_clone(const struct footer *src);

/** @brief Reinitializes a `struct footer`
 *  @return Returns the re-initialized `struct footer`.
 */
struct footer *footer_clear(struct footer *f);

/** @brief Frees the memory used by a `struct footer` and it's internal data.
 *  @param A pointer to the `struct footer` to be freed.
 */
void          footer_del(struct footer *f);

/** @brief Reads the data from `stream` into the `struct frame *f`.
 *  @return Returns `true` on successfull read; `false` on failure to read a frame
 */
b8 footer_read(FILE *stream, struct footer *h);

/** @brief Writes the information from `struct footer *h` into the stream `stream`.
 *  @return Returns the stream.
 *  
 *  The data written to the stream is not the actual footer data but the human
 *    readable representation of it.
 *
 *  The data shown is:
 *    - The version string in the forma "ID3v{major}.{revision}"
 *    - The size of the footer in bytes
 */
FILE *footer_stream(FILE *stream, const struct footer *h);

/** @brief Writes the information from `struct footer *h` into the stream `stream`.
 *  @return Returns the stream.
 *  
 *  The data written to the stream is not the actual footer data but the human
 *    readable representation of it.
 *
 *  The data fields shown are:
 *    - The version:
 *      - The major part
 *      - The minor part
 *    - The footer flags:
 *      - Whether the frames are unsynchronized (True/False)
 *      - Whether the footer is extended (True/False)
 *      - Whether experimental features are used (True/False)
 *      - Whether a footer is present (True/False)
 *      - Whether the frames are uncleared (True/False)
 *    - The size of the footer in bytes
 */
FILE *footer_stream_full(FILE *stream, const struct footer *h);

#endif
