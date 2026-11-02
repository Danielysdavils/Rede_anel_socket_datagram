#include "Message.h"

struct Message *DecodeMessage (char buffer[TAM_BUFFER], size_t data) {

}

struct Response *CreateMessage (char *messageId, int messageOwner, int currentRodada, struct Endpoint *endpoint, enum MessageType type, struct Data *data) {

}

struct Message *UpdateMessage (struct Message *message, struct Endpoint *endpoint, enum MessageType type, struct Data *data){

}

void DestroiMessage (struct Message *message){

}