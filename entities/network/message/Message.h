#ifndef __MESSAGE__
#define __MESSAGE__

#include <stdio.h>

#include "../../game/data/Data.h"
#include "../../../config/gameDefs.h"
#include "../../../config/networkDefs.h"

/* data = bastão; ACK = info */
enum MessageType {
    ACK,
    DATA
};

// Dto simples para identificar dados do destinatario/origem da mensagem
// aqui são os dados para transferencia do bastão, não transferencias de re-transmissões !
struct EndpointDto {
    int EndpointID;
    const char *Ip;
    int Port;
};

struct Message {
    // Identificador único do bastão
    // servidor é o único que pode gerar Ids
    // Id é fixo para cada evento da jogada, o q permite identificar retransmissões
    char *Id;
    // contador de retransmissões de um bastão
    int Retry;
    // tipo da mensagem: bastão, ack
    enum MessageType Type;
    // dados simples da maquina que precisa receber o bastão
    struct EndpointDto Destination;    
    // dados simples da mquina que está enviando o bastão
    struct EndpointDto Origin;
    // criador da mensagem - sempre o servidor
    int OwnerId;
    // info sobre como a messagem será transportada
    // pode ser igual ou diferente do envio direto do bastão
    // ou seja, aqui podemos ter destinatario original da mensagem (bastao)
    // ou intermediarios (endpoints mensajeros)
    struct Transfer *Transfer;
    // controle de entrega da mensagem
    // exclusiva para transferencia do bastão
    struct Control *Control;
    // info do tamanho dos dados
    int TamData;
    // dados
    struct Data *Data;
};

struct Message *DecodeMessage (size_t data);

struct Response *CreateMessage (int currentRodada, struct Endpoint *endpoint, enum MessageType type, struct Data *data);

void DestroiMessage (struct Message *message);

#endif