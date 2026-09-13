#ifndef ID3V240_FILE_H
# define ID3V240_FILE_H

#include "header.h"
#include "ext_header.h"
#include "footer.h"
#include "frame.h"

#include <stdio.h>

struct file {
  enum state state;
  char  *name;

  struct header     header;
  struct ext_header ext_header;
  struct footer     footer;
  
  struct frame  **frames;
  u32           nframes;

  u32   padding;
};

/** @brief Initializes a `struct file`.
 *  @return Returns a pointer to the initialized `struct file`.
 *  @param `f` A pointer to the `struct file` to be initialized.
 *
 *  `NULL` pointer guarded. Will return `NULL` if given `NULL`.
 */
struct file *file_init(struct file *f);

/** @brief Allocated a `struct file` and initializes it.
 *  @return Returns a pointer to the initialized `struct file` on success;
 *    Retruns `NULL` on failure.
 */ 
struct file *file_new();

/** @brief Makes `dest` a copy of `src`.
 *  @return Returns a pointer to `dest`.
 *  @param `dest` The destination `struct file` to become a copy of `src`.
 *  @param `src` The source `struct file` who's data will be copied into
 *    `dest`.
 */
struct file *file_cpy(struct file *dest, const struct file *src);

/** @brief Allocates a new `struct file` and copies the data from `src` into
 *    it.
 *  @return Returns the new `struct file` on success; Returns `NULL` on failure
 *    to allocate memory for the `struct file` or it's data.
 */
struct file *file_clone(const struct file *src);

/** @brief Reinitializes a `struct file`
 *  @return Returns the re-initialized `struct file`.
 */
struct file *file_clear(struct file *f);

/** @brief Frees the memory used by a `struct file` and it's internal data.
 *  @param A pointer to the `struct file` to be freed.
 */
void          file_del(struct file *f);

/** @brief Reads the data from `stream` into the `struct frame *f`.
 *  @return Returns `true` on successfull read; `false` on failure to read a frame
 */
b8 file_read(struct file *h, const char *filename);

/** @brief Writes the information from `struct file *h` into the stream `stream`.
 *  @return Returns the stream.
 *  
 *  The data written to the stream is not the actual file data but the human
 *    readable representation of it.
 *
 *  The data shown is:
 *    - The version string in the forma "ID3v{major}.{revision}"
 *    - The size of the file in bytes
 */
FILE *file_stream(FILE *stream, const struct file *h);

/** @brief Writes the information from `struct file *h` into the stream `stream`.
 *  @return Returns the stream.
 *  
 *  The data written to the stream is not the actual file data but the human
 *    readable representation of it.
 *
 *  The data fields shown are:
 *    - The version:
 *      - The major part
 *      - The minor part
 *    - The file flags:
 *      - Whether the frames are unsynchronized (True/False)
 *      - Whether the file is extended (True/False)
 *      - Whether experimental features are used (True/False)
 *      - Whether a footer is present (True/False)
 *      - Whether the frames are uncleared (True/False)
 *    - The size of the file in bytes
 */
FILE *file_stream_full(FILE *stream, const struct file *h);

/** @brief Returns an array of `sturct frame`s with a matching id stored in
 *    `file`.
 *  @return Returns an array of `struct frame`s with a `NULL` pointer at the
 *    end on success; returns `NULL` on failure to allocate the array.
 *  @param f The `struct file` that is to be searched for matching `sturct frame`
 *    id's
 *  @param id The id of the `struct frame`s to be searched for.
 *
 *  The `struct frame` inside the array shouldn't be freed. They are pointers
 *    inside the memory used by the `struct file`. They can be used to remove
 *    a specific instance of frames with identical tags by passing their pointer
 *    to the appropriate function.
 */
struct frame **file_get_frames(const struct file *f, const char id[4]);

#endif

