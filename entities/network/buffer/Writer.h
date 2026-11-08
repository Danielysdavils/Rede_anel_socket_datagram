#ifndef WRITER_H_
#define WRITER_H_

#include "./Buffer.h"



struct Buffer *CreateWriter(size_t capacity);

void DestroiWriter(struct Buffer *writer);

bool WriteString(struct Buffer *writer, char *string);

bool WriteU8(struct Buffer *writer, uint8_t value);

bool WriteU16(struct Buffer *writer, uint16_t value);

bool WriteU32(struct Buffer *writer, uint32_t value);

#endif