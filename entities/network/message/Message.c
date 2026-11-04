#include "Message.h"
#include "../../../config/messageDefs.h"
#include "../bufferWriter/Writer.h"
#include "../control/Control.h"

/**
 * AUX FUNCTIONS:
 *  - Para códificar e decodificar
 *  - A ordem é fundamental!! 
 *  - ler docs para mais informação
*/

#define VERSION 1


bool EncodeBet(struct Bet bet, struct BufferWriter *writer){
    if(writer == NULL) return false;

    if(!CanWrite(writer, BYTE_BET)) return false;

    if(!WriteU8(writer, (uint8_t)bet.type)) return false;
    if(!WriteU32(writer, (uint32_t)bet.value)) return false;
    
    return true;
}

bool EncodeBetDto(const struct BetDto *betDto, struct BufferWriter *writer){
    if(betDto == NULL || writer == NULL) return false;

    if(!CanWrite(writer, BYTE_BETDTO)) return false;

    if(!WriteU8(writer, (uint8_t)betDto->PlayerId)) return false;

    if(!EncodeBet(betDto->Bet, writer)) return false;

    return true;
}

bool EncodeEndpointDto(struct EndpointDto endpointDto, struct BufferWriter *writer){
    if(writer = NULL || writer->buffer == NULL || endpointDto.Ip == NULL) return NULL;

    size_t stringLenght = strlen(endpointDto.Ip);
    if(stringLenght > UINT8_MAX) return false;

    // como já sei os bytes restantes posso usuar o BYTE_ENDPOINT aqui
    if(!CanWrite(writer, BYTE_ENDPOINTDTO + stringLenght)) return false;

    if(!WriteU8(writer, (uint8_t)endpointDto.EndpointID)) return false;

    if(!WriteString(writer, endpointDto.Ip)) return false;

    if(!WriteU16(writer, (uint16_t)endpointDto.Port)) return false;

    return true;
}

bool EncodeAvailablePaths(uint8_t paths[TOTAL_ENDPOINTS][TOTAL_ENDPOINTS], struct BufferWriter *writer){
    if(paths == NULL || writer == NULL) return;

    if(!CanWrite(writer, BYTE_AVAILABLEPATH)) return false;

    uint16_t encoded = 0;

    for(int i = 0; i < TOTAL_ENDPOINTS; i++){
        for(int j = 0; j < TOTAL_ENDPOINTS; j++){
            // matriz bianria (só 1 e 0)
            if(paths[i][j] > 1) return false;

            if(paths[i][j] == 1){
                int bit = i * TOTAL_ENDPOINTS + j;
                encoded |= (uint16_t)(1u << bit);
            }
        }
    }

    return WriteU16(writer, encoded);
}

bool EncodeTransfer(struct Transfer *transfer, struct BufferWriter *writer){
    if(transfer == NULL || writer == NULL || writer->buffer == NULL) return false;

    size_t stringLenght = strlen(transfer->TransferId);
    if(stringLenght > UINT8_MAX) return false;

    if(!WriteString(writer, transfer->TransferId)) return false; 

    if(!EncodeEndpointDto(transfer->DestinationEndpointId, writer)) return false;

    if(!EncodeEndpointDto(transfer->OriginEndpointId, writer)) return false;

    if(!EncodeAvailablePaths(transfer->AvailablePaths, writer));

    if(!WriteU8(writer, transfer->PathVersion)) return false;

    if(!WriteU8(writer, transfer->Status)) return false;

    return true;
}

bool EncodeControl(struct Control *control, struct BufferWriter *writer){
    if(control == NULL || writer == NULL) return NULL;

    size_t stringLenght = strlen(control->ControlId);
    if(stringLenght > UINT8_MAX) return false;

    // como já sei os bytes restantes posso usuar o BYTE_CONTROL aqui
    if(!CanWrite(writer, BYTE_CONTROL + stringLenght)) return false;

    if(!WriteString(writer, control->ControlId)) return false;

    if(!WriteU8(writer, control->State)) return false;

    return true;
}

bool EncodeDtoArray(struct BetDto betDtoArray[TOTAL_ENDPOINTS], struct BufferWriter *writer){
    if(betDtoArray == NULL || writer == NULL) return false;

    if(!CanWrite(writer, TOTAL_ENDPOINTS * BYTE_BETDTO)) return false;

    // betDto é um array de Bets, então percorro o array e codifico cada item
    for(int i = 0; i < TOTAL_ENDPOINTS; i++){
        if(!EncodeBetDto(&betDtoArray[i], writer)) return false;
    }

    return true;
}

bool EncodeData(struct Data *data, struct BufferWriter *writer){
    if(data == NULL || writer == NULL || writer->buffer == NULL) return false;

    if(data->Round < 0 || data->Round > UINT32_MAX || !WriteU32(writer, data->Round)) return false;

    if(data->MaxRound < 0 || data->MaxRound > UINT8_MAX || !WriteU8(writer, data->MaxRound)) return false;

    if(data->Event < INIT || data->Event > END || !WriteU8(writer, data->Event)) return false;

    if(!EncodeDtoArray(writer, data->Bets)) return false;

    if(!EncodeDtoArray(writer, data->Reward)) return false;

    return true;
}

/**/

size_t EncodeMessage (struct Message *message, uint8_t *data) {
    if(!message) return -1;

    struct BufferWriter *writer = CreateWriter(TAM_BUFFER);
    if(!writer){
        printf("[MESSAGE_ERROR] Creating writer for encodeMesage");
        return -1;
    }

    if(!WriteU8(writer, (uint8_t)VERSION)) return -1;

    if(!WriteString(writer, message->Id)) return -1;

    if(!WriteU8(writer, message->Retry)) return -1; 

    if(!WriteU8(writer, message->Type)) return -1;

    if(!EncodeEndpointDto(message->Destination, writer)) return -1;

    if(!EncodeEndpointDto(message->Origin, writer)) return -1;

    if(!WriteU8(writer, message->OwnerId)) return -1;

    if(!EncodeTransfer(message->Transfer, writer)) return -1;

    if(!EncodeControl(message->Control, writer)) return -1;

    if(message->Data){
        if(!WriteU8(writer, 1)) return -1;

        if(!EncodeData(message->Data, writer)) return -1;
    }

    size_t bytesWritten = writer->offset;

    memcpy(data, writer->buffer, writer->offset);
    
    DestroyWriter(writer);

    return bytesWritten;
}

struct Message *DecodeMessage (char buffer[TAM_BUFFER], size_t data) {

}

struct Response *CreateMessage (char *messageId, int messageOwner, int currentRodada, struct Endpoint *endpoint, enum MessageType type, struct Data *data) {

}

struct Message *UpdateMessage (struct Message *message, struct Endpoint *endpoint, enum MessageType type, struct Data *data){

}

void DestroiMessage (struct Message *message){

}