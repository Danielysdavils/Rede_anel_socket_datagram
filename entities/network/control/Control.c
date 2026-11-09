#include "Control.h"
#include "../../../config/networkDefs.h"
#include "../../utils/String.h"


char *GenerateControlId(char *messageId, enum Event event, int dest, int or){
    int len = snprintf(NULL, 0, "%s-%d-%d-%d", messageId, (int)event, or, dest);
    if(len < 0){
        printf("[CONTROL] Error calculating controlId leng");
        return NULL;
    }

    char *id = malloc((size_t)len+1);
    if(!id) return NULL;

    snprintf(id, (size_t)len+1, "%s-%d-%d-%d", messageId, (int)event, or, dest);
    return id;
}

// É usado para identificar uma transferencia do 'bastão lógico'
struct Control *CreateAndInitControl (char *messageId, enum Event event, int originId, int destinationId) {
    struct Control *control = calloc(1, sizeof(struct Control));
    if (!control) {
        printf("[CONTROL_ERROR]: Error in CreateControl function. Creating malloc");
        return NULL;
    }

    // gera o id do controle
    // sempre corresponderá ao id da mensagem, origem e destino
    control->ControlId = GenerateControlId(messageId, event, destinationId, originId);
    if(!control->ControlId){
        free(control);
        return NULL;
    }

    control->State = CONTROL_WAITING;
    return control;
}

struct Control *CreateControl (char *controlID, enum ControlState state) {
    struct Control *control = calloc(1, sizeof(struct Control));
    if (!control) {
        printf("[CONTROL_ERROR]: Error in CreateControl function. Creating malloc");
        return NULL;
    }

    control->ControlId = DuplicateString(controlID);
    if(!control->ControlId){
        free(control);
        return NULL;
    }

    control->State = state;
    return control;
}

void MarkProccessed (struct Control *control) {
    control->State = CONTROL_PROCCESSED;
}

void MarkProccessedError (struct Control *control) {
    control->State = CONTROL_PROCCESSED_WITH_ERROR;
}

void DestroyControl (struct Control *control) {
    if(!control) return;
    if(control->ControlId) free(control->ControlId);
    free(control);
}

bool CompareControl (const char *controlA, const char *controlB){
    if(controlA == NULL || controlB == NULL)
        return false;
    
    return strcmp(controlA, controlB);
}