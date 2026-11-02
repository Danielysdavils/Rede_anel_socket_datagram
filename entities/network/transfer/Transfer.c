#include "Transfer.h"

struct Transfer *CreateTransfer (const char *transferId, struct EndpointDto origin, struct EndpointDto destino) {
    struct Transfer *transfer = malloc(sizeof(struct Transfer));
    if (!transfer) {
        printf("[Transfer_ERROR]: Error in Transfer function. Creating malloc");
        return NULL;
    }

    transfer->DestinationEndpointId = destino;
    transfer->OriginEndpointId = origin;

    // a matriz de caminhos precisa ser incializada com 1
    // que indicam caminhos ativos, porém a diagonal é com 0 pois uma maquina não manda
    // mensagem para ela mesma
    for(int i = 0; i < TOTAL_ENDPOINTS; i++){
        for(int j = 0; j < TOTAL_ENDPOINTS; j++){
            transfer->AvailablePaths[i][j] = (i != j);
        }
    }

    transfer->PathVersion = 1;
    if(transferId == NULL){
        printf("[TRANSFER_ERROR] Transfer id cant be a null value!");
        return NULL;
    }

    transfer->TransferId = transferId;
    transfer->Status = WAITING;
    return transfer;
}

bool UpdateTransfer (struct Transfer *transfer, struct EndpointDto origem, struct EndpointDto destino) {
    if(transfer == NULL) return false;

    transfer->DestinationEndpointId = destino;
    transfer->OriginEndpointId = origem;

    return true;
}

void DestroiTransfer (struct Transfer *transfer) {
    if(transfer) free(transfer);
    transfer = NULL;
}