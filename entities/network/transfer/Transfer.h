#ifndef __TRANSFER__
#define __TRANSFER__

#include <stdint.h>
#include "../../../config/networkDefs.h"
#include "../endpoint/Endpoint.h"


enum TransferState{
    WAITING,
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
    char *TransferId;
    // id do destinatario da transferencia (pode ser o mesmo do bastão ou intermediário)
    struct EndpointDto DestinationEndpointId;
    // id do origem da transferencia (pode ser o mesmo do bastão ou intermediário)
    struct EndpointDto OriginEndpointId;
    // matriz dinamica de caminhos disponíveis entre os endpoints
    uint8_t AvailablePaths[TOTAL_ENDPOINTS][TOTAL_ENDPOINTS];
    // versão da matriz (ajuda identificar mudanças)
    int PathVersion;
    // status da transferencia (pensado para funcionar com os acks dos intermediários)
    enum TransferState Status;
};

// A transferencia é criada, a principio, com os dados de destiono e origem iguais
// aos da mensagem, depois, quando retrans, ai sim alterar para as maquinas seleciondas
struct Transfer *CreateTransfer (const char *transferId, struct EndpointDto origin, struct EndpointDto destino);

// no update só alteramos origem e destino da retransmissão
// transfer id fica o mesmo (é usado controlID no transferID)
bool UpdateTransfer (struct Transfer *transfer, struct EndpointDto origem, struct EndpointDto destino);

void DestroiTransfer (struct Transfer *transfer);
// por enquanto não vou mexer nas matrizes ***
// nem tmb nos acks intermediarios ***
#endif