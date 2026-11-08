#include "./Reader.h"

bool ReaderString(struct Buffer *buffer, char **value){
    if(value == NULL) return false;

    uint8_t len;
    
    // decodifica tamanho da string
    if(!ReaderU8(buffer, &len)) return false;

    // verifica se há espaço no buffer para a string
    if(!Can(buffer, len)) return false;

    // aloco memoria para a string
    *value = malloc((size_t)len + 1); // +1 para o '\0'
    if(*value == NULL) return false;

    memcpy(*value, buffer->buffer + buffer->offset, len);
    buffer->offset += len;

    // finalizo string
    (*value)[len] = '\0';

    return true;
}

bool ReaderU8(struct Buffer *buffer, uint8_t *value){
    if(value == NULL) return false; 

    if(!Can(buffer, 1)) return false;

    *value = (uint8_t)buffer->buffer[buffer->offset];
    buffer->offset++;
    
    return true; 
}

bool ReaderU16(struct Buffer *buffer, uint16_t *value){
    if(value == NULL) return false;

    if(!Can(buffer, 2)) return false;

    *value = ((uint16_t)buffer->buffer[buffer->offset] << 8) | 
              (uint16_t)buffer->buffer[buffer->offset + 1];

    buffer->offset += 2;
    return true;
}

bool ReaderU32(struct Buffer *buffer, uint32_t *value){
    if(value == NULL) return false;

    if(!Can(buffer, 4)) return false;

    *value = ((uint32_t)buffer->buffer[buffer->offset] << 24) |
             ((uint32_t)buffer->buffer[buffer->offset + 1] << 16) |
             ((uint32_t)buffer->buffer[buffer->offset + 2] << 8) |
             (uint32_t)buffer->buffer[buffer->offset + 3];

    buffer->offset += 4;
    return true;
}