#ifndef __RESPONSE__
#define __RESPONSE__

#include <stdbool.h>
#include "../../config/networkDefs.h"
#include "../network/message/Message.h"

struct Response {
    char *Message; // status, indicadores, etc
    bool State; // dois estados: erro | nenhum erro
    int Code; // para identificar erros e def compr esp
    void *Data;
};

void ResponseError(struct Response *response, char *msm);

void ResponseErrorCode(struct Response *response, char *msm, int code);

void ResponseAccert(struct Response *response, char *msm);

void ResponseAccertData(struct Response *response, void *data, int code);

void ResponseErrorData(struct Response *response, char *msm, int code);

#endif