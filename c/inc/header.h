#ifndef ID3V240_HEADER_H
# define ID3V240_HEADER_H

#include "id3v240.h"

#include <stdio.h>

#define F_UNSYN 128   // 0b10000000
#define F_EXTEN 64    // 0b01000000
#define F_EXPER 32    // 0b00100000
#define F_FOOTR 16    // 0b00010000
#define F_UNCLR 15    // 0b00001111

struct header {
  enum state  state;
  struct {
    u32 major;
    u32 minor;
  } version;
  struct {
    b8  unsynchronisation : 1;
    b8  extended : 1;
    b8  experimental : 1;
    b8  footer : 1;
    b8  uncleared : 1;
  } flags;
  u32 size;
  u32 pos;
};

/** @brief Initializes a `struct header`.
 *  @return Returns a pointer to the initialized `struct header`.
 *  @param `f` A pointer to the `struct header` to be initialized.
 *
 *  `NULL` pointer guarded. Will return `NULL` if given `NULL`.
 */
struct header *header_init(struct header *f);

/** @brief Allocated a `struct header` and initializes it.
 *  @return Returns a pointer to the initialized `struct header` on success;
 *    Retruns `NULL` on failure.
 */ 
struct header *header_new();

/** @brief Makes `dest` a copy of `src`.
 *  @return Returns a pointer to `dest`.
 *  @param `dest` The destination `struct header` to become a copy of `src`.
 *  @param `src` The source `struct header` who's data will be copied into
 *    `dest`.
 */
struct header  *header_cpy(struct header *dest, const struct header *src);

/** @brief Allocates a new `struct header` and copies the data from `src` into
 *    it.
 *  @return Returns the new `struct header` on success; Returns `NULL` on failure
 *    to allocate memory for the `struct header` or it's data.
 */
struct header *header_clone(const struct header *src);

/** @brief Reinitializes a `struct header`
 *  @return Returns the re-initialized `struct header`.
 */
struct header *header_clear(struct header *f);

/** @brief Frees the memory used by a `struct header` and it's internal data.
 *  @param A pointer to the `struct header` to be freed.
 */
void          header_del(struct header *f);

/** @brief Reads the data from `stream` into the `struct frame *f`.
 *  @return Returns `true` on successfull read; `false` on failure to read a frame
 */
b8            header_read(struct header *h, FILE *stream);

/** @brief Updates a `struct header`s internal fields to reflect changes
 *    made by the user.
 *  @return Always `true`.
 *  
 *  It is important to update all fields of a file before saving it to properly
 *    reflect changes made by the user. This function is called as part of the
 *    `file_update()` function.
 */
b8            header_update(struct header *h);

/** @brief Sets a `struct header`s flags According to the flags passed in `flags`.
 *  @return Always true.
 *  @param h The `struct header` whos flags should be altered.
 *  @param flags The byte composed of ORd flags from the list below.
 *
 *  The `flag` byte passed consists of multiple different flags ORd together. The
 *    Macros available are:
 *      - F_UNSYN :   To indicate unsynchronisation in the file
 *      - F_EXTEN :   To indicate the existence of an extended header
 *      - F_EXPER :   To indicate the existence of experimental frames
 *      - F_FOOTR :   To indicate the existence of a footer
 */
b8            header_set_flags(struct header *h, u8 flags);

/** @brief Writes the information from `struct header *h` into the stream `stream`.
 *  @return Returns the stream.
 *  
 *  The data written to the stream is not the actual header data but the human
 *    readable representation of it.
 *
 *  The data shown is:
 *    - The version string in the forma "ID3v{major}.{revision}"
 *    - The size of the header in bytes
 */
FILE          *header_stream(FILE *stream, const struct header *h);

/** @brief Writes the information from `struct header *h` into the stream `stream`.
 *  @return Returns the stream.
 *  
 *  The data written to the stream is not the actual header data but the human
 *    readable representation of it.
 *
 *  The data fields shown are:
 *    - The version:
 *      - The major part
 *      - The minor part
 *    - The header flags:
 *      - Whether the frames are unsynchronized (True/False)
 *      - Whether the header is extended (True/False)
 *      - Whether experimental features are used (True/False)
 *      - Whether a footer is present (True/False)
 *      - Whether the frames are uncleared (True/False)
 *    - The size of the header in bytes
 */
FILE          *header_stream_full(FILE *stream, const struct header *h);

#endif
