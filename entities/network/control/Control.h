#ifndef __CONTROL__
#define __CONTROL__

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "../../../config/networkDefs.h"

typedef enum {
    PROCCESSED, // mensagem correta e recebida com sucesso
    WAITING, // mensagem é criada com esse estado inicial "aguardando resposta"
    PROCCESSED_WITH_ERROR // a mensagem chegou com algum problema

} ControlState;

// especifico para processos de bastão
struct Control {
    char *ControlId; // [id da mensagem]-[destinatario]-[remetente]
    ControlState State;
};

// compara dois id e verifica se são iguais
bool CompareControl (const char *controlA, const char *controlB);

struct Control *CreateAndInitControl (int messageId, int originId, int destinationId);

struct Control *CreateControl (char *controlID, ControlState state);

void MarkProccessed (struct Control *control);

void MarkProccessedError (struct Control *control);

void DestroyControl (struct Control *control);

#endif