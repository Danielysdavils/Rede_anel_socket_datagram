#include "Buffer.h"

// values representa a quantidade de bytes que preciso escrever
bool Can(struct Buffer *buffer, size_t value){
    if(buffer == NULL || buffer->buffer == NULL) return false;

    if(buffer->offset > buffer->capacity) return false;

    return (buffer->capacity - buffer->offset) >= value ;
}
