#ifndef __RESPONSE__
#define __RESPONSE__

#include <stdbool.h>
#include "../../config/networkDefs.h"
#include "../network/message/Message.h"

struct Response {
    char *Message; // status, indicadores, etc
    bool State; // dois estados: erro | nenhum erro
    int Code; // para identificar erros e def compr esp
    struct Message *Data;
};

void ResponseError(struct Response *response, char *msm);

void ResponseErrorCode(struct Response *response, char *msm, int code);

void ResponseAccert(struct Response *response, char *msm);

void ResponseAccertData(struct Response *response, struct Message *data, int code);

void ResponseErrorData(struct Response *response, char *msm, int code);

#endif