#include "Response.h"


void ResponseError(struct Response *response, char *msm) {
    response->Message = msm;
    response->State = false;
    response->Code = CODE_GENERAL_ERROR;
}

void ResponseErrorCode(struct Response *response, char *msm, int code) {
    response->Message = msm;
    response->State = false;
    response->Code = code;
}

void ResponseAccert(struct Response *response, char *msm) {
    response->Message = msm;
    response->State = true;
    response->Code = CODE_GENERAL_ACCERT;
}

void ResponseAccertData(struct Response *response, struct Message *data, int code) {
    response->State = true;
    response->Code = CODE_GENERAL_ACCERT;
    response->Data = data;
}

void ResponseErrorData(struct Response *response, char *msm, int code) {
    response->Message = msm;
    response->State = false;
    response->Data = NULL;
    response->Code = code;
}