#include "utils.h"

#include <stdlib.h>

const u8 *boolstr(b8 val)
{
  static const u8 strs[2][6] = {
    "True",
    "False",
  };
  return strs[val == 0];
}

inline b8 is_synchsafe_u28(const u8 *synchsafe)
{
  return (!(synchsafe[0] & M_SYNCHUNSAFE ||
        synchsafe[1] & M_SYNCHUNSAFE ||
        synchsafe[2] & M_SYNCHUNSAFE ||
        synchsafe[3] & M_SYNCHUNSAFE));
}

u32 read_synchsafe_u28(const u8 *synchsafe)
{
  u32 byte0 = synchsafe[0] & M_SYNCHSAFE;
  u32 byte1 = synchsafe[1] & M_SYNCHSAFE;
  u32 byte2 = synchsafe[2] & M_SYNCHSAFE;
  u32 byte3 = synchsafe[3] & M_SYNCHSAFE;
  return (byte0 << 21 | byte1 << 14 | byte2 << 7 | byte3);
}

u32 read_synchunsafe_u32(const u8 *synchsafe)
{
  u32 byte0 = synchsafe[0];
  u32 byte1 = synchsafe[1];
  u32 byte2 = synchsafe[2];
  u32 byte3 = synchsafe[3];
  return (byte0 << 24 | byte1 << 16 | byte2 << 8 | byte3);
}

void write_synchsafe_u28(u8 *dest, u32 n)
{
  dest[0] = (n >> 21) & M_SYNCHSAFE;
  dest[1] = (n >> 14) & M_SYNCHSAFE;
  dest[2] = (n >> 7) & M_SYNCHSAFE;
  dest[3] = n & M_SYNCHSAFE;
}

inline b8 is_synchsafe_u35(const u8 *synchsafe)
{
  return (!(synchsafe[0] & M_SYNCHUNSAFE ||
        synchsafe[1] & M_SYNCHUNSAFE ||
        synchsafe[2] & M_SYNCHUNSAFE ||
        synchsafe[3] & M_SYNCHUNSAFE ||
        synchsafe[4] & M_SYNCHUNSAFE));
}

u64 read_synchsafe_u35(u8 *synchsafe)
{
  u32 byte0 = synchsafe[0] & M_SYNCHSAFE35UPPER;
  u32 byte1 = synchsafe[1] & M_SYNCHSAFE;
  u32 byte2 = synchsafe[2] & M_SYNCHSAFE;
  u32 byte3 = synchsafe[3] & M_SYNCHSAFE;
  u32 byte4 = synchsafe[4] & M_SYNCHSAFE;
  return (byte0 << 28 | byte1 << 21 | byte2 << 14 | byte3 << 7 | byte4);
}

u64 read_synchunsafe_u40(const u8 *synchsafe)
{
  u64 byte0 = synchsafe[0];
  u64 byte1 = synchsafe[1];
  u64 byte2 = synchsafe[2];
  u64 byte3 = synchsafe[3];
  u64 byte4 = synchsafe[4];
  return (byte0 << 32 | byte1 << 24 | byte2 << 16 | byte3 << 8 | byte4);
}

void write_synchsafe_u35(u8 *dest, u64 n)
{
  dest[0] = (n >> 28) & M_SYNCHSAFE35UPPER;
  dest[1] = (n >> 21) & M_SYNCHSAFE;
  dest[2] = (n >> 14) & M_SYNCHSAFE;
  dest[3] = (n >> 7) & M_SYNCHSAFE;
  dest[4] = n & M_SYNCHSAFE;
}

b8 move_file_contents(const char *filename, u32 start, u32 size_diff) {
  FILE *stream = fopen(filename, "r+");
  if (stream == NULL)
    return false;
  char  *buf[2];
  buf[0] = calloc(sizeof(*buf), size_diff);
  buf[1] = calloc(sizeof(*buf), size_diff);
  if (buf[0] == NULL || buf[1] == NULL)
    goto free_failure;

  fseek(stream, start, SEEK_SET);
  u8  bufpos = 0;
  i32 buffill[2] = {size_diff, size_diff};
  while (feof(stream) == false && buffill[(bufpos + 1) % 2] != 0) {
    u32 cur = ftell(stream);
    buffill[bufpos] = fread(buf[bufpos], sizeof(*buf[bufpos]), size_diff, stream);
    if (ferror(stream) == true)
      goto free_failure;
    fseek(stream, cur, SEEK_SET);
    bufpos = (bufpos + 1) % 2;
    fwrite(buf[bufpos], sizeof(*buf[bufpos]), buffill[bufpos], stream);
    if (ferror(stream) == true)
      goto free_failure;
    buffill[bufpos] = 0;
  }

  free(buf[0]);
  free(buf[1]);
  fclose(stream);
  return true;
free_failure:
  free(buf[0]);
  free(buf[1]);
  fclose(stream);
  return false;
}
