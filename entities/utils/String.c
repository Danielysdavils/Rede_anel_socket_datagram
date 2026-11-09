#include "String.h"
#include <string.h>
#include <stdlib.h>


char *DuplicateString(const char *s){
    if(!s) return NULL;

    size_t len = strlen(s) + 1; // + '\0'
    char *string_copia = malloc(len);
    if(!string_copia) return NULL;

    memcpy(string_copia, s, len);
    return string_copia;
}