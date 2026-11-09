#ifndef __CONTROL__
#define __CONTROL__

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../../config/networkDefs.h"
#include "../../game/data/Data.h"

enum ControlState {
    CONTROL_PROCCESSED, // mensagem correta e recebida com sucesso
    CONTROL_WAITING, // mensagem é criada com esse estado inicial "aguardando resposta"
    CONTROL_PROCCESSED_WITH_ERROR // a mensagem chegou com algum problema
};

// especifico para processos de bastão
struct Control {
    char *ControlId; // [id da mensagem]-[destinatario]-[remetente]
    enum ControlState State;
};

// compara dois id e verifica se são iguais
bool CompareControl (const char *controlA, const char *controlB);

struct Control *CreateAndInitControl (char *messageId, enum Event event, int originId, int destinationId);

struct Control *CreateControl (char *controlID, enum ControlState state);

void MarkProccessed (struct Control *control);

void MarkProccessedError (struct Control *control);

void DestroyControl (struct Control *control);

#endif