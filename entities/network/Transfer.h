#ifndef __TRANSPORT__
#define __TRANSPORT__

#include <stdint.h>
#include "../../config/networkDefs.h"

enum TransferState{
    RECEIVED,
    ERROR, 
    SENDED,
};

struct Transfer
{
    /* 
        DADOS DE TRANSFERENCIA DA MENSAGEM 
    */
   // id da transferencia intermediária [id da mensagem][destinatario temp][remetente temp]
    char TransferId[TAM_LIMIT_ID];
    // id do destinatario da transferencia (pode ser o mesmo do bastão ou intermediário)
    int DestinationEndpointId;
    // id do origem da transferencia (pode ser o mesmo do bastão ou intermediário)
    int OriginEndpointId;
    // matriz dinamica de caminhos disponíveis entre os endpoints
    uint8_t AvailablePaths[TOTAL_ENDPOINTS][TOTAL_ENDPOINTS];
    // versão da matriz (ajuda identificar mudanças)
    int PathVersion;
    // status da transferencia (pensado para funcionar com os acks dos intermediários)
    enum TransferState Status;
};


#endif