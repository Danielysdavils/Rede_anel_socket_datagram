#ifndef __MESSAGE__
#define __MESSAGE__

#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "../../game/data/Data.h"
#include "../../../config/gameDefs.h"
#include "../../../config/networkDefs.h"
#include "../transfer/Transfer.h"
#include "../../../config/messageDefs.h"
#include "../buffer/Writer.h"
#include "../buffer/Reader.h"
#include "../control/Control.h"

struct Endpoint;

/* data = bastão; ACK = info */
enum MessageType {
    ACK,
    DATA
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
    int DestinationId;    
    // dados simples da mquina que está enviando o bastão
    int OriginId;
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
    // dados
    struct Data *Data;
};

void LogMessage(struct Message *msm);

char *GenerateMessageId(int round, int endpointId);

size_t EncodeMessage (struct Message *message, uint8_t *data, size_t capacity);

struct Message *DecodeMessage (uint8_t *data, size_t capacity);

struct Message *CreateMessage (char *messageId, int messageOwner, int retry, struct Endpoint *endpoint, int transferEndpointDest, int transferEndpointOr, enum MessageType type, struct Data *data, enum Event event);

struct Message *CreateDecodeMessage (char *messageId, int messageRetry, enum MessageType type, int dest, int ori, int messageOwnerId, struct Transfer *transfer, struct Control *control, struct Data *data);

// cria uma nova mensagem a partir da antiga (atualiza, porém cria registro novo apartir do velho)
struct Message *UpdateMessage (struct Message *message, int retry, struct Endpoint *endpoint, enum MessageType type, int transferEndpointDest, int transferEndpointOr, struct Data *data);

void DestroiMessage (struct Message *message);

#endif