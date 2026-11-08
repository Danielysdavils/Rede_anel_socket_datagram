#ifndef READER_H
#define READER_H

#include "./Buffer.h"

bool ReaderString(struct Buffer *buffer, char **value);

bool ReaderU8(struct Buffer *buffer, uint8_t *value);

bool ReaderU16(struct Buffer *buffer, uint16_t *value);

bool ReaderU32(struct Buffer *buffer, uint32_t *value);


#endif