#include "Writer.h"
#include "Reader.h"
#include "Buffer.h"

int main () {
    uint8_t data[100] = {0};

    struct Buffer writer = {
        .buffer = data,
        .capacity = sizeof(data),
        .offset = 0
    };

    //WriteU32(&writer, 0x12345678);
    WriteString(&writer, "123-2-1");
    
    struct Buffer reader = {
        .buffer = data,
        .capacity = writer.offset,
        .offset = 0
    };

    char *result = NULL;
    if(UnpackString(&reader, &result)){
        printf("String: %s\n", result);
        printf("Bytes lidos: %zu\n", reader.offset);
    
        free(result);
    }
    return 0;
}