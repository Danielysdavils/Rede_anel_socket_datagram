#ifndef __TRANSFER__
#define __TRANSFER__

#include <stdint.h>
#include "../../../config/networkDefs.h"
#include "../endpoint/Endpoint.h"


enum TransferState{
    TRANSFER_WAITING,
    TRASNFER_RECEIVED,
    TRANSFER_ERROR, 
    TRANSFER_SENDED,
};

struct Transfer
{
    /* 
        DADOS DE TRANSFERENCIA DA MENSAGEM 
    */
   // id da transferencia intermediária [id da mensagem][destinatario temp][remetente temp]
    char *TransferId;
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

// A transferencia é criada, a principio, com os dados de destiono e origem iguais
// aos da mensagem, depois, quando retrans, ai sim alterar para as maquinas seleciondas
struct Transfer *CreateTransfer(char *transferId, int Endpointorigin, int EndpointDest, uint8_t paths[TOTAL_ENDPOINTS][TOTAL_ENDPOINTS], int pathVersion, enum TransferState state);

struct Transfer *CreateAndInitTransfer (const char *transferId, int EndpointOrigin, int EndpointDestino);

// no update só alteramos origem e destino da retransmissão
// transfer id fica o mesmo (é usado controlID no transferID)
bool UpdateTransfer (struct Transfer *transfer, int EndpointOrigem, int EndpointDestino);

void DestroiTransfer (struct Transfer *transfer);
// por enquanto não vou mexer nas matrizes ***
// nem tmb nos acks intermediarios ***
#endif