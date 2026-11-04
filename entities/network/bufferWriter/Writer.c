#include "Writer.h"

struct BufferWriter *CreateWriter(size_t capacity){
    if(capacity <= 0) return NULL;

    struct BufferWriter *writer = malloc(sizeof(struct BufferWriter));
    if(!writer){
        printf("[WRITER] Erro criating buffer writer");
        return NULL;
    }

    writer->buffer = malloc(capacity);
    if(!writer->buffer){
        free(writer);
        return NULL;
    }

    writer->capacity = capacity;
    writer->offset = 0;
    return writer;
}

void DestroiWriter(struct BufferWriter *writer){
    if(!writer) return;

    free(writer->buffer);
    free(writer);
}

// values representa a quantidade de bytes que preciso escrever
bool CanWrite(struct BufferWriter *writer, size_t value){
    if(writer == NULL || writer->buffer == NULL) return false;

    if(writer->offset > writer->capacity) return false;

    return (writer->capacity - writer->offset) >= value ;
}

bool WriteU8(struct BufferWriter *writer, uint8_t value) {
    if(!CanWrite(writer, 1)) return false; 

    writer->buffer[writer->offset] = value;
    writer->offset++;

    return true;
}

bool WriteU16(struct BufferWriter *writer, uint16_t value) {
    if(!CanWrite(writer, 2)) return false; 

    writer->buffer[writer->offset] = (uint8_t)(value >> 8);
    writer->buffer[writer->offset + 1] = (uint8_t)value;

    writer->offset += 2;

    return true;
}

bool WriteU32(struct BufferWriter *writer, uint32_t value) {
    if(!CanWrite(writer, 4)) return false; 

    writer->buffer[writer->offset] = (uint8_t)(value >> 24);
    writer->buffer[writer->offset + 1] = (uint8_t)(value >> 16);
    writer->buffer[writer->offset + 2] = (uint8_t)(value >> 8);
    writer->buffer[writer->offset + 3] = (uint8_t)value;

    writer->offset += 4;
    return true;
}

// Estamos usando 1 byte para guardar o tamanho da string
// não acho que passe de 255
bool WriteString(struct BufferWriter *writer, char *string){
    if(writer == NULL || writer->buffer == NULL || string == NULL) return false;

    size_t stringLenght = strlen(string);
    if(stringLenght > UINT8_MAX) return false;

    // verificamos se temos espaço para a string e +1 do byte que
    // guarda o tamanho da s
    if(!CanWrite(writer, stringLenght + 1)) return false;

    // escreve o tamanho da s
    if(!WriteU8(writer, (uint8_t)stringLenght)) return false;

    // precisamos manualmente copiar a s ao buffer
    memcpy(writer->buffer + writer->offset, string, stringLenght);

    // atualizar tmb manualmente o offset
    writer->offset += stringLenght;

    return true;
}