#include <stdio.h>
#include "Control.h"
#include "../../../config/networkDefs.h"

// É usado para identificar uma transferencia do 'bastão lógico'
struct Control *CreateControl (int messageId, int originId, int destinationId) {
    struct Control *control = malloc(sizeof(struct Control));
    if (!control) {
        printf("[CONTROL_ERROR]: Error in CreateControl function. Creating malloc");
        return NULL;
    }

    // gera o id do controle
    // sempre corresponderá ao id da mensagem, origem e destino
    int result = snprintf(control->ControlId, sizeof(control->ControlId), "%d-%d-%d", messageId, originId, destinationId);
    if(result < 0) {
        printf("[CONTROL_ERROR]: Error creating controlId");
        return NULL;
    }

    if(result >= sizeof(control->ControlId)){
        printf("[CONTROL_WARNING]: ID Truncated. Maybe will need increase charId limit");
    }

    control->State = WAITING;
    return control;
}

void MarkProccessed (struct Control *control) {
    control->State = PROCCESSED;
}

void MarkProccessedError (struct Control *control) {
    control->State = PROCCESSED_WITH_ERROR;
}

void DestroyControl (struct Control *control) {
    free (control);
}