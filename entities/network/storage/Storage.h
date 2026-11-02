#ifndef __STORAGE__
#define __STORAGE__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../../../config/networkDefs.h"

struct Storage {
    // cada endpoint tem um histórico local de transferencias para identificar transferencias já processadas.
    // para identificar cada transferencia processada usa o <controlID>
    char ReceivedControlTransfers[MAX_TRANSFERS][MAX_TRANSFER_ID_SIZE];
    int ControlReceivedCount;

    // histórico local de transferencias enviadas
    // necessário para identificar mensagens que ainda não receberam acks 
    char SendedControlTransfers[MAX_TRANSFERS][MAX_TRANSFER_ID_SIZE];
    int ControlSendedCount;

    // matriz local de caminhos disponíveis
    uint8_t AvailablePaths[TOTAL_ENDPOINTS][TOTAL_ENDPOINTS];
    // versão da matriz (ajuda identificar mudanças)
    int PathVersion;
};


struct Storage *CreateStorage();

void DestroyStorage(struct Storage *storage);

// adiciona um controlID no array selecionado no parametro
bool AddControlTransfer(char elements[][MAX_TRANSFER_ID_SIZE], int *count, char *controlId);

// remove um controlID do array selecionadp
bool RemoveControlTransfer(char elements[][MAX_TRANSFER_ID_SIZE], int *count, char *controlId);

// verifica se existe control no array selecionado
bool FindElement(const char elements[][MAX_TRANSFER_ID_SIZE], int count, char *controlId);


#endif

