#include "Storage.h"

struct Storage *CreateStorage() {
    // calloc porque já inicializa os campos com 0, útil para as matrizes
    // de received e sended
    struct Storage *storage = calloc(1, sizeof(struct Storage));
    if (!storage) {
        printf("[STORAGE_ERROR]: Error in Storage function. Creating calloc");
        return NULL;
    }

    // a matriz de caminhos é diferente, precisa ser incializada com 1
    // que indicam caminhos ativos, porém a diagonal é com 0 pois uma maquina não manda
    // mensagem para ela mesma
    for(int i = 0; i < TOTAL_ENDPOINTS; i++){
        for(int j = 0; j < TOTAL_ENDPOINTS; j++){
            storage->AvailablePaths[i][j] = (i != j);
        }
    }

    storage->PathVersion = 1;
    return storage;
}

void DestroyStorage(struct Storage *storage) {
    if(storage) free (storage);
}

// adiciona ou na received ou na sended
// quem chama a função decide a quem adicionar
bool AddControlTransfer(char elements[][MAX_TRANSFER_ID_SIZE], int *count, char *controlId) {
    if(elements == NULL || count == NULL || controlId == NULL) return false;

    if(*count >= MAX_TRANSFERS) return false;

    // tamanho do control id nao seja muti grante
    size_t len = strlen(controlId);
    if(len >= MAX_TRANSFER_ID_SIZE)
        return false;

    strcpy(elements[*count], controlId);

    (*count)++;
    printf("[STORAGE] ControlId %s added successfully!", controlId);
    return true;
}

// para remover fazemos deslocação à esquerda
bool RemoveControlTransfer(char elements[][MAX_TRANSFER_ID_SIZE], int *count, char *controlId) {
    if(elements == NULL || count == NULL || controlId == NULL) return false;

    if(*count < 0 || *count > MAX_TRANSFERS) return false;

    for (int i = 0; i < *count; i++){
        // paramos na posição a remover
        if(strcmp(elements[i], controlId) == 0) {
            for(int j = i; i < *count - 1; j++) {
                strcpy(elements[j], elements[j + 1]);
            }

            (*count)--;

            // limpamos a ultima posição
            // pois todos se deslocaram
            elements[*count][0] = '\0';
            return true;
        }
    }

    return false;
}

bool FindElement(const char elements[][MAX_TRANSFER_ID_SIZE], int count, char *controlId) {
    if(elements == NULL || controlId == NULL) return false;

    for(int i = 0; i < count; i++){
        if(strcmp(elements[i], controlId) == 0){
            return true;
        }
    }

    return false;
}