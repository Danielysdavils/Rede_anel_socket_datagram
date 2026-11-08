#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


struct Buffer {
    uint8_t *buffer;
    size_t capacity;
    size_t offset;
};


// values representa a quantidade de bytes que preciso escrever
bool Can(struct Buffer *writer, size_t value);


#endif

