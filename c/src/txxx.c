#include "txxx.h"

#include <string.h>
#include <stdlib.h>

struct txxx *txxx_init(struct txxx *txxx)
{
  memset(txxx, 0, sizeof(*txxx));
  return txxx;
}

struct txxx *txxx_new()
{
  struct txxx *new = malloc(sizeof(*new));
  if (new == NULL)
    return NULL;
  return txxx_init(new);
}

b8 txxx_cpy(struct txxx *dest, const struct txxx *src)
{
  txxx_clear(dest);
  dest->desc = calloc(sizeof(*dest->desc), src->desc_len + 1);
  dest->val = calloc(sizeof(*dest->val), src->val_len + 1);
  if (dest->desc == NULL || dest->val == NULL) {
    txxx_clear(dest);
    return false;
  }
  dest->txtenc = src->txtenc;
  dest->desc_len = src->desc_len;
  dest->val_len = src->val_len;
  memcpy(dest->desc, src->desc, dest->desc_len + 1);
  memcpy(dest->val, src->val, dest->val_len + 1);
  return true;
}

struct txxx *txxx_clone(struct txxx *src)
{
  struct txxx *dest = txxx_new();
  if (dest == NULL)
    return NULL;
  if (!txxx_cpy(dest, src)) {
    txxx_del(dest);
    return NULL;
  }
  return dest;
}

struct txxx *txxx_clear(struct txxx *txxx)
{
  free(txxx->desc);
  free(txxx->val);
  return txxx_init(txxx);
}

void txxx_del(struct txxx *txxx)
{
  txxx_clear(txxx);
  free(txxx);
}

b8 txxx_read(struct txxx *txxx, const struct frame *frame)
{
  txxx_clear(txxx);
  u32 pos = 0;
  txxx->txtenc = frame->data[pos] > 3 ? 4 : frame->data[pos];
  ++pos;
  if (frame->size < pos) {
    txxx_clear(txxx);
    return false;
  }
  txxx->desc_len = strlen(frame->data + pos);
  pos += txxx->desc_len + 1;
  if (frame->size < pos) {
    txxx_clear(txxx);
    return false;
  }
  txxx->val_len = frame->size - pos;
  txxx->desc = calloc(sizeof(*txxx->desc), txxx->desc_len + 1);
  txxx->val = calloc(sizeof(*txxx->val), txxx->val_len + 1);
  if (txxx->desc == NULL || txxx->val == NULL) {
    txxx_clear(txxx);
    return false;
  }
  memcpy(txxx->desc, frame->data + 1, txxx->desc_len + 1);
  memcpy(txxx->val, frame->data + 2 + txxx->desc_len, txxx->val_len);
  return true;
}

b8 txxx_write(struct frame *frame, const struct txxx *txxx)
{
  frame_clear(frame);
  frame->size = 2 + txxx->desc_len + txxx->val_len;
  frame->data = calloc(sizeof(*frame->data), frame->size + 1);
  if (frame->data == NULL) {
    frame_clear(frame);
    return false;
  }
  u32 pos = 0;
  frame->data[pos++] = (u8)txxx->txtenc;
  memcpy(frame->data + pos, txxx->desc, txxx->desc_len + 1);
  pos += txxx->desc_len + 1;
  memcpy(frame->data + pos, txxx->val, txxx->val_len);
  return true;
}

FILE  *txxx_stream(FILE *stream, const struct txxx *txxx)
{
  fprintf(stream, "txxx: {description: %s, val: %s}", txxx->desc, txxx->val);
  return stream;
}

