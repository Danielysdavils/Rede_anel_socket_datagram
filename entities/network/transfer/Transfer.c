#include "Transfer.h"
#include "../../utils/String.h"



struct Transfer *CreateAndInitTransfer (const char *transferId, int EndpointOrigin, int EndpointDestino) {
    struct Transfer *transfer = calloc(1, sizeof(struct Transfer));
    if (!transfer) {
        printf("[Transfer_ERROR]: Error in Transfer function. Creating malloc");
        return NULL;
    }

    transfer->DestinationEndpointId = EndpointDestino;
    transfer->OriginEndpointId = EndpointOrigin;

    // a matriz de caminhos precisa ser incializada com 1
    // que indicam caminhos ativos, porém a diagonal é com 0 pois uma maquina não manda
    // mensagem para ela mesma
    for(int i = 0; i < TOTAL_ENDPOINTS; i++){
        for(int j = 0; j < TOTAL_ENDPOINTS; j++){
            transfer->AvailablePaths[i][j] = (i != j);
        }
    }

    transfer->TransferId = DuplicateString(transferId);
    if(!transfer->TransferId){
        printf("[TRANSFER_ERROR] Transfer id error duplicaring!");
        free(transfer);
        return NULL;
    }

    transfer->PathVersion = 1;
    transfer->Status = TRANSFER_WAITING;
    return transfer;
}

struct Transfer *CreateTransfer(char *transferId, int EndpointOrigin, int EndpointDest, uint8_t paths[TOTAL_ENDPOINTS][TOTAL_ENDPOINTS], int pathVersion, enum TransferState state){
     if(transferId == NULL || paths == NULL){
        printf("[TRANSFER_ERROR] Transfer id cant be a null value!");
        return NULL;
    }
    
    struct Transfer *transfer = calloc(1, sizeof(struct Transfer));
    if (!transfer) {
        printf("[Transfer_ERROR]: Error in Transfer function. Creating malloc");
        return NULL;
    }

    transfer->TransferId = DuplicateString(transferId);
    if (!transfer->TransferId) {
        printf("[TRANSFER_ERROR] Transfer id error duplicaring!");
        free(transfer);
        return NULL;
    }

    transfer->DestinationEndpointId = EndpointDest;
    transfer->OriginEndpointId = EndpointOrigin;

    memcpy(transfer->AvailablePaths, paths, sizeof(transfer->AvailablePaths));

    transfer->PathVersion = pathVersion;
    transfer->Status = state;

    return transfer;
}

bool UpdateTransfer (struct Transfer *transfer, int EndpointOrigem, int EndpointDestino) {
    if(transfer == NULL) return false;

    transfer->DestinationEndpointId = EndpointDestino;
    transfer->OriginEndpointId = EndpointOrigem;

    return true;
}

void DestroiTransfer (struct Transfer *transfer) {
    if(!transfer) return;
    
    if(transfer->TransferId) free(transfer->TransferId);
    if(transfer) free(transfer);
}