#ifndef __WRITER__
#define __WRITER__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


struct BufferWriter {
    uint8_t *buffer;
    size_t capacity;
    size_t offset;
};


struct BufferWriter *CreateWriter(size_t capacity);

void DestroiWriter(struct BufferWriter *writer);

bool WriteString(struct BufferWriter *writer, char *string);

bool CanWrite(struct BufferWriter *writer, size_t value);

bool WriteU8(struct BufferWriter *writer, uint8_t value);

bool WriteU16(struct BufferWriter *writer, uint16_t value);

bool WriteU32(struct BufferWriter *writer, uint32_t value);

#endif