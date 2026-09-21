#ifndef ID3V240FRAME_H
# define ID3V240FRAME_H

#include "typedef.h"
#include "id3v240.h"

#include <stdio.h>

#define F_TAGPRES 128   // 0b01000000
#define F_FILPRES 64    // 0b00100000
#define F_RDONLY 32     // 0b00010000

#define F_GROUPED 16    // 0b01000000
#define F_COMPRES 8     // 0b00001000
#define F_ENCRYPT 4     // 0b00000100
#define F_UNSYNC 2      // 0b00000010
#define F_LENINDI 1     // 0b00000001

struct frame {
  enum state state;
  char  id[5];
  u32   pos;
  u32   size;
  b8    size_synchunsafe;
  struct {
    b8  preserve_tag;
    b8  preserve_file;
    b8  read_only;
  }     status;
  struct {
    b8  grouped;
    b8  compressed;
    b8  encrypted;
    b8  unsynchronisation;
    b8  length_indicated;
  }     format;
  char  *data;
};

/** @brief Initializes a `struct frame`.
 *  @return Returns a pointer to the initialized `struct frame`.
 *  @param `f` A pointer to the `struct frame` to be initialized.
 *
 *  `NULL` pointer guarded. Will return `NULL` if given `NULL`.
 */
struct frame  *init_frame(struct frame *f);

/** @brief Allocated a `struct frame` and initializes it.
 *  @return Returns a pointer to the initialized `struct frame` on success;
 *    Retruns `NULL` on failure.
 */ 
struct frame  *frame_new();

/** @brief Free's `dest`s internal memory and alloctes new one to fit the
 *    data from `src`.
 *  @return Returns `dest` on success with the copied data from `src`; Returns
 *    `NULL` on failure to allocate memory for `dest`s data.
 *  @param `dest` The destination `struct frame` to copy data into. It's content
 *    get's freed by this function.
 *  @param `src` The source `struct frame` who's data will be copied into
 *    `dest`.
 */
struct frame  *frame_cpy(struct frame *dest, const struct frame *src);

/** @brief Allocates a new `struct frame` and copies the data from `src` into
 *    it.
 *  @return Returns the new `struct frame` on success; Returns `NULL` on failure
 *    to allocate memory for the `struct frame` or it's data.
 */
struct frame  *frame_clone(const struct frame *src);

/** @brief Frees the internal memory of the given `struct frame` and
 *    re-initializes it. 
 *  @return Returns the re-initialized `struct frame`.
 */
struct frame  *frame_clear(struct frame *f);

/** @brief Frees the memory used by a `struct frame` and it's internal data.
 *  @param A pointer to the `struct frame` to be freed.
 */
void          frame_del(struct frame *f);

/** @brief Reads the data from `stream` into the `struct frame *f`.
 *  @return Returns `true` on successfull read; `false` on failure to read a frame
 */
b8            frame_read(struct frame *f, FILE *stream);

/** @brief Writes the information from `struct frame *f` into the stream `stream`.
 *  @return Returns the stream.
 *  
 *  The data written to the stream is not the actual frame data but the human
 *    readable representation of it.
 *
 *  The data fields shown are:
 *    - The frames id (4 Characters)
 *    - The frames data (byte string until a '\0' character is encountered,
 *
 *  The data is not cleanedup and can mess with the output since a frames data can
 *    be binary data.
 */
FILE          *frame_stream(FILE *stream, const struct frame *f);

/** @brief Writes the information from `struct frame *f` into the stream `stream`.
 *  @return Returns the stream.
 *  
 *  The data written to the stream is not the actual frame data but the human
 *    readable representation of it.
 *
 *  The data fields shown are:
 *    - The frames id (4 Characters)
 *    - The frames position in the file.
 *    - The size of the frames data in bytes.
 *    - The frames status:
 *      - Whether the tag is supposed to be preserved (True/False)
 *      - Whether the file is supposed to be preserved (True/False)
 *      - Whether the frame is read only (True/False)
 *    - The frames format:
 *      - Whether the frame is grouped (True/False)
 *      - Whether the frame is compressed (True/False)
 *      - Whether the frame is encrypted (True/False)
 *      - Whether the frame is unsynchronisation (True/False)
 *      - Whether the frames length is indicated (True/False)
 *    - The frames data (byte string until a '\0' character is encountered,
 *
 *  The data is not cleanedup and can mess with the output since a frames data can
 *    be binary data.
 */
FILE          *frame_stream_full(FILE *stream, const struct frame *f);

#endif
