#ifndef __CONTROL__
#define __CONTROL__

#include "../../../config/networkDefs.h"

typedef enum {
    PROCCESSED, // mensagem correta e recebida com sucesso
    WAITING, // mensagem é criada com esse estado inicial "aguardando resposta"
    PROCCESSED_WITH_ERROR // a mensagem chegou com algum problema

} ControlState;

// especifico para processos de bastão
struct Control {
    char ControlId[TAM_LIMIT_ID]; // [id da mensagem][destinatario][remetente]
    ControlState State;
};

struct Control *CreateControl (int messageId, int originId, int destinationId);

void MarkProccessed (struct Control *control);

void MarkProccessedError (struct Control *control);

void DestroyControl (struct Control *control);

#endif