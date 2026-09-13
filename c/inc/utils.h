#ifndef ID3V240UTILS_H
# define ID3V240UTILS_H

#include "typedef.h"

#include <stdio.h>

/** @brief Returns a pointer to a string representation of the boolean value
 *    provided.
 */
const u8 *boolstr(b8 val);

#define M_SYNCHUNSAFE 128    // 0b10000000
#define M_SYNCHSAFE 127       // 0b01111111
#define M_SYNCHSAFE35UPPER 15 // 0b00001111

/** @brief Checks if the 32 bit number in the data segment `synchsafe` is synchsafe.
 *  @return Returns `true` if the number is synchsafe; `false` if it is not.
 *  @param synchsafe The data segement the number should be read from. Needs to
 *    be at least 4 bytes long.
 */
b8  is_synchsafe_28(const u8 *synchsafe);

/** @brief Reads a synchsafe 28 bit number from the data segment `synchsafe` and
 *    returns it.
 *  @return The read synchsafe number.
 *  @param synchsafe The data segement the number should be read from. Needs to
 *    be at least 4 bytes long.
 */
u32 read_synchsafe_u28(const u8 *synchsafe);

/** @brief Reads a not synchsafe 32 bit number from the data segment `notsynchsafe`
 *    and returns it.
 *  @return The read not synchsafe number.
 *  @param synchsafe The data segement the number should be read from. Needs to
 *    be at least 4 bytes long.
 */
u32 read_synchunsafe_u32(const u8 *notsynchsafe);

/** @brief Writes a 32 bit synchsafe number into the data segment `dest`.
 *  @param dest The data segment the number is supposed to be written to. Needs
 *    to be at least 4 bytes long.
 *  @param n The number to be written into `dest`.
 */
void write_synchsafe_u28(u8 *dest, u32 n);

/** @brief Checks if the 35 bit number in the data segment `synchsafe` is synchsafe.
 *  @return Returns `true` if the number is synchsafe; `false` if it is not.
 *  @param synchsafe The data segement the number should be read from. Needs to
 *    be at least 5 bytes long.
 */
b8  is_synchsafe_35(const u8 *synchsafe);

/** @brief Reads a synchsafe 35 bit number from the data segment `synchsafe` and
 *    returns it.
 *  @return The read synchsafe number.
 *  @param synchsafe The data segment the number should be read from. Needs to
 *    be at least 5 bytes long.
 */
u64 read_synchsafe_u35(u8 *synchsafe);

/** @brief Reads a not synchsafe 40 bit number from the data segment `notsynchsafe`
 *    and returns it.
 *  @return The read not synchsafe number.
 *  @param synchsafe The data segement the number should be read from. Needs to
 *    be at least 5 bytes long.
 */
u64 read_synchunsafe_u40(const u8 *notsynchsafe);

/** @brief Writes a 35 bit synchsafe number into the data segment `dest`.
 *  @param dest The data segment the number is supposed to be written to. Needs
 *    to be at least 5 bytes long.
 *  @param n The number to be written into `dest`.
 */
void write_synchsafe_u35(u8 *dest, u64 n);

b8 move_file_contents(const char *filename, u32 start, u32 size_diff);

#endif
