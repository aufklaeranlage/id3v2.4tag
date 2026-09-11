#ifndef ID3V240UTILS_H
# define ID3V240UTILS_H

#include "typedef.h"

/** @brief Returns a pointer to a string representation of the boolean value
 *		provided.
 */
const u8 *boolstr(b8 val);

#define M_SYNCHSAFE 127       // 0b01111111
#define M_SYNCHSAFE35UPPER 15 // 0b00001111

/**	@brief Reads a synchsafe 32 bit number from the data segment `synchsafe` and
 *		returns it.
 *	@return The read synchsafe number.
 *	@param synchsafe The data segemtn the number should be read from. Needs to
 *		be at least 4 bytes long.
 */
u32 read_synchsafe_u32(const u8 *synchsafe);

/**	@brief Writes a 32 bit synchsafe number into the data segment `dest`.
 *	@param dest The data segment the number is supposed to be written to. Needs
 *		to be at least 4 bytes long.
 *	@param n The number to be written into `dest`.
 */
void write_synchsafe_u32(u8 *dest, u32 n);

/**	@brief Reads a synchsafe 35 bit number from the data segment `synchsafe` and
 *		returns it.
 *	@return The read synchsafe number.
 *	@param synchsafe The data segemtn the number should be read from. Needs to
 *		be at least 5 bytes long.
 */
u64 read_synchsafe_u35(u8 *synchsafe);

/**	@brief Writes a 35 bit synchsafe number into the data segment `dest`.
 *	@param dest The data segment the number is supposed to be written to. Needs
 *		to be at least 5 bytes long.
 *	@param n The number to be written into `dest`.
 */
void write_synchsafe_u35(u8 *dest, u64 n);

#endif
