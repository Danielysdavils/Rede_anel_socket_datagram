#ifndef __STORAGE__
#define __STORAGE__

#include <stdint.h>
#include "../../config/networkDefs.h"

struct Storage {
    // cada endpoint tem um histórico local de transferencias para identificar transferencias já processadas.
    // para identificar cada transferencia processada usa o <controlID>
    char ReceivedControlTransfers[MAX_TRANSFERS][MAX_TRANSFER_ID_SIZE];
    int ControlTransferCount;

    // histórico local de transferencias enviadas
    // necessário para identificar mensagens que ainda não receberam acks 
    char SendedControlTransfers[MAX_TRANSFER_ID_SIZE][MAX_TRANSFER_ID_SIZE];
    int ControlSendedCount;

    // matriz local de caminhos disponíveis
    uint8_t AvailablePaths[TOTAL_ENDPOINTS][TOTAL_ENDPOINTS];
    // versão da matriz (ajuda identificar mudanças)
    int PathVersion;
};

#endif

