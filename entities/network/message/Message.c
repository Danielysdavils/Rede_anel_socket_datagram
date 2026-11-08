#include "Message.h"
#include <limits.h>
#include "../../../config/messageDefs.h"
#include "../buffer/Writer.h"
#include "../buffer/Reader.h"
#include "../control/Control.h"

/**
 * AUX FUNCTIONS:
 *  - Para códificar e decodificar
 *  - A ordem é fundamental!! 
 *  - ler docs para mais informação
*/

#define VERSION 1


bool EncodeBet(struct Bet bet, struct Buffer *writer){
    if(writer == NULL) return false;

    if(bet.type < Numero || bet.type > Pares || bet.value < 0) return false;

    if(!Can(writer, BYTE_BET)) return false;

    if(!WriteU8(writer, (uint8_t)bet.type)) return false;
    if(!WriteU32(writer, (uint32_t)bet.value)) return false;
    
    return true;
}

bool DecodeBet(struct Buffer *buffer, struct Bet *bet){
    if(bet == NULL) return false;

    if(!Can(buffer, BYTE_BET)) return false;

    // var temp para fazer a conversão seg
    uint8_t type;
    uint32_t value;

    if(!ReaderU8(buffer, (uint8_t)&type)) return false;
    if(!ReaderU32(buffer, (uint32_t)&value)) return false;

    if(type > Pares || value > INT32_MAX)
        return false;

    bet->type = (enum BetType)type;
    bet->value = (int)value;

    return true;
}

bool EncodeBetDto(struct BetDto *betDto, struct Buffer *writer){
    if(betDto == NULL || writer == NULL) return false;

    if(!Can(writer, BYTE_BETDTO)) return false;

    if(!WriteU8(writer, (uint8_t)betDto->PlayerId)) return false;

    if(!EncodeBet(betDto->Bet, writer)) return false;

    return true;
}

bool DecodeBetDto(struct Buffer *buffer, struct BetDto *betDto){
    if(betDto == NULL) return false;

    if(!Can(buffer, BYTE_BETDTO)) return false;

    uint8_t playerId;
    struct Bet bet;
    
    if(!ReaderU8(buffer, (uint8_t)&playerId)) return false;

    if(!DecodeBet(buffer, &bet)) return false;

    betDto->PlayerId = (int)playerId;
    betDto->Bet = bet;

    return true;
}

// bool EncodeEndpointDto(struct EndpointDto endpointDto, struct Buffer *writer){
//     if(writer = NULL || writer->buffer == NULL || endpointDto.Ip == NULL) return NULL;

//     size_t stringLenght = strlen(endpointDto.Ip);
//     if(stringLenght > UINT8_MAX) return false;

//     // como já sei os bytes restantes posso usuar o BYTE_ENDPOINT aqui
//     if(!Can(writer, BYTE_ENDPOINTDTO + stringLenght)) return false;

//     if(!WriteU8(writer, (uint8_t)endpointDto.EndpointID)) return false;

//     if(!WriteString(writer, endpointDto.Ip)) return false;

//     if(!WriteU16(writer, (uint16_t)endpointDto.Port)) return false;

//     return true;
// }

// bool DecodeEndpointDto(struct Buffer *buffer, struct EndpointDto *endpointDto){
//     if(endpointDto == NULL) return false;
    
//     // como não posso medir o tamanho antes de deserealizar só estimo com os bytes conhecidos
//     if(!Can(buffer, BYTE_ENDPOINTDTO)) return false;

//     uint8_t endpointID;
//     if(!ReaderU8(buffer, &endpointID)) return false;

//     if(!ReaderString(buffer, &endpointDto->Ip)) return false;

//     uint16_t endpointPort;
//     if(ReaderU16(buffer, &endpointPort)) return false;

//     endpointDto->EndpointID = (int)endpointID;
//     endpointDto->Port = (int)endpointPort;

//     return true;
// }

bool EncodeAvailablePaths(uint8_t paths[TOTAL_ENDPOINTS][TOTAL_ENDPOINTS], struct Buffer *writer){
    if(paths == NULL || writer == NULL) return;

    if(!Can(writer, BYTE_AVAILABLEPATH)) return false;

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

bool DecodeAvailablePaths(struct Buffer *buffer, uint8_t paths[TOTAL_ENDPOINTS][TOTAL_ENDPOINTS]){
    if(paths == NULL) return false;

    if(!Can(buffer, BYTE_AVAILABLEPATH)) return false;

    uint16_t encodedMatriz;
    if(!ReaderU16(buffer, &encodedMatriz)) return false;

    for(int i = 0; i < TOTAL_ENDPOINTS; i++){
        for(int j = 0; j < TOTAL_ENDPOINTS; j++){
            int bit = i * TOTAL_ENDPOINTS + j;
            paths[i][j] = (uint8_t)((encodedMatriz >> bit) & 1u);
        }
    }

    return true;
}

bool EncodeTransfer(struct Transfer *transfer, struct Buffer *writer){
    if(transfer == NULL || writer == NULL || writer->buffer == NULL) return false;

    size_t stringLenght = strlen(transfer->TransferId);
    if(stringLenght > UINT8_MAX) return false;

    if(!WriteString(writer, transfer->TransferId)) return false; 

    // if(!EncodeEndpointDto(transfer->DestinationEndpointId, writer)) return false;
    if(!WriteU8(writer, transfer->DestinationEndpointId)) return false;

    // if(!EncodeEndpointDto(transfer->OriginEndpointId, writer)) return false;
    if(!WriteU8(writer, transfer->OriginEndpointId)) return false;

    if(!EncodeAvailablePaths(transfer->AvailablePaths, writer));

    if(!WriteU8(writer, transfer->PathVersion)) return false;

    if(!WriteU8(writer, transfer->Status)) return false;

    return true;
}

bool DecodeTransfer(struct Buffer *buffer, struct Transfer *transfer){
    if(transfer == NULL) return false;

    if(!ReaderString(buffer, &transfer->TransferId)) return false;

    // struct EndpointDto dest;
    // if(!DecodeEndpointDto(buffer, &dest)) return false;
    uint8_t dest;
    if(!ReaderU8(buffer, &dest)) return false;

    // struct EndpointDto or;
    // if(!DecodeEndpointDto(buffer, &or)) return false;
    uint8_t or;
    if(!ReaderU8(buffer, &or)) return false;

    if(!DecodeAvailablePaths(buffer, transfer->AvailablePaths)) return false;

    uint8_t pathVersion;
    if(!ReaderU8(buffer, &pathVersion)) return false;

    uint8_t status;
    if(!ReaderU8(buffer, &status)) return false;

    transfer->DestinationEndpointId = dest;
    transfer->OriginEndpointId = or;
    transfer->PathVersion = (int)pathVersion;
    transfer->Status = (enum TransferState)status;

    return true;
}

bool EncodeControl(struct Control *control, struct Buffer *writer){
    if(control == NULL || writer == NULL) return NULL;

    size_t stringLenght = strlen(control->ControlId);
    if(stringLenght > UINT8_MAX) return false;

    // como já sei os bytes restantes posso usuar o BYTE_CONTROL aqui
    if(!Can(writer, BYTE_CONTROL + stringLenght)) return false;

    if(!WriteString(writer, control->ControlId)) return false;

    if(!WriteU8(writer, control->State)) return false;

    return true;
}

bool DecodeControl(struct Buffer *buffer, struct Control *control){
    if(control == NULL) return false;

    if(!Can(buffer, BYTE_CONTROL)) return false;

    if(!ReaderString(buffer, &control->ControlId)) return false;

    uint8_t state;
    if(!ReaderU8(buffer, &state)) return false;

    control->State = (ControlState)state;
    return true;
}

bool EncodeDtoArray(struct BetDto betDtoArray[TOTAL_ENDPOINTS], struct Buffer *writer){
    if(betDtoArray == NULL || writer == NULL) return false;

    if(!Can(writer, TOTAL_ENDPOINTS * BYTE_BETDTO)) return false;

    // betDto é um array de Bets, então percorro o array e codifico cada item
    for(int i = 0; i < TOTAL_ENDPOINTS; i++){
        if(!EncodeBetDto(&betDtoArray[i], writer)) return false;
    }

    return true;
}

bool DecodeDtoArray(struct Buffer *buffer, struct BetDto betDtoArray[TOTAL_ENDPOINTS]){
    if(betDtoArray == NULL) return false;

    if(!Can(buffer, TOTAL_ENDPOINTS * BYTE_BETDTO)) return false;

    for(int i = 0; i < TOTAL_ENDPOINTS; i++){
        if(!DecodeBetDto(buffer, &betDtoArray[i])) return false;
    }

    return true;
}

bool EncodeData(struct Data *data, struct Buffer *writer){
    if(data == NULL || writer == NULL || writer->buffer == NULL) return false;

    if(data->Round < 0 || data->Round > UINT32_MAX || !WriteU32(writer, data->Round)) return false;

    if(data->MaxRound < 0 || data->MaxRound > UINT8_MAX || !WriteU8(writer, data->MaxRound)) return false;

    if(data->Event < INIT || data->Event > END || !WriteU8(writer, data->Event)) return false;

    if(!EncodeDtoArray(writer, data->Bets)) return false;

    if(!EncodeDtoArray(writer, data->Reward)) return false;

    return true;
}

bool DecodeData(struct Buffer *buffer, struct Data *data){
    if(data == NULL || buffer == NULL || buffer->buffer == NULL) return false;

    uint32_t round;
    if(!ReadU32(buffer, &round)) return false;

    // round é inteiro então para garantir que cabe
    if(round < 0 || round > INT_MAX) return false;

    uint8_t maxRound;
    if(!ReadU8(buffer, &maxRound)) return false;

    if(maxRound < 0) return false;

    uint8_t event;
    if(!ReadU8(buffer, &event)) return false;

    if(event < INIT || event > END) return false;

    if(!DecodeDtoArray(buffer, data->Bets)) return false;

    if(!DecodeDtoArray(buffer, data->Reward)) return false;

    data->Round = (int)round;
    data->MaxRound = (int)maxRound;
    data->Event = (enum Event)event;

    return true;
}   


/**/

size_t EncodeMessage (struct Message *message, uint8_t *data, size_t capacity) {
    if(!message) return -1;

    struct Buffer writer = {
        .buffer = data,
        .capacity = capacity,
        .offset = 0
    };

    if(!WriteU8(&writer, (uint8_t)VERSION)) return -1;

    if(!WriteString(&writer, message->Id)) return -1;

    if(!WriteU8(&writer, message->Retry)) return -1; 

    if(!WriteU8(&writer, message->Type)) return -1;

    // if(!EncodeEndpointDto(message->Destination, &writer)) return -1;
    if(!WriteU8(&writer, message->DestinationId)) return -1;

    // if(!EncodeEndpointDto(message->Origin, &writer)) return -1;
    if(!WriteU8(&writer, message->OriginId)) return -1;

    if(!WriteU8(&writer, message->OwnerId)) return -1;

    if(!EncodeTransfer(message->Transfer, &writer)) return -1;

    if(!EncodeControl(message->Control, &writer)) return -1;

    if(message->Data){
        if(!WriteU8(&writer, 1)) return -1;
        if(!EncodeData(message->Data, &writer)) return -1;
    }else
        if(!WriteU8(&writer, 0)) return -1;

    size_t bytesWritten = writer.offset;

    if(bytesWritten > capacity)
        return -1;

    return bytesWritten;
}

struct Message *DecodeMessage (uint8_t *data, size_t capacity) {
    if(data == NULL || capacity < 0) return NULL;

    struct Buffer reader = {
        .buffer = data,
        .capacity = capacity,
        .offset = 0
    };

    uint8_t version;
    if(!ReadU8(&reader, (uint8_t)&version)){
        printf("[DECODER]: Version not found!");
        return NULL;
    }

    if(version != VERSION){
        printf("[Decoder] Diferent  protocol versions!");
        return NULL;
    }

    char *messageId;
    if(!ReaderString(&reader, &messageId)){
        printf("[Decoder] Failed into read messageID");
        return NULL;
    }

    uint8_t retry;
    if(!ReaderU8(&reader, &retry)){
        printf("[DECODER] Failed into reade retry");
        return NULL;
    }

    uint8_t type;
    if(!ReaderU8(&reader, &type)){
        printf("[Decoder] Failed into reader type");
        return NULL;
    }

    uint8_t dest;
    if(!ReaderU8(&reader, &dest)){
        printf("[Decoder] Failed into reader destinationID");
        return NULL;
    }

    uint8_t or;
    if(!ReaderU8(&reader, &or)){
        printf("[Decoder] Failed into reader originID");
        return NULL;
    }

    uint8_t ownerID;
    if(!ReaderU8(&reader, &ownerID)){
        printf("[Decoder] failed inoto reader ownerID");
        return NULL;
    }

    struct Transfer transfer = {0};
    if(!DecodeTransfer(&reader, &transfer)){
        printf("[DEcoder] FAiled into decode transfer");
        return NULL;
    }

    struct Control control = {0};
    if(!DecodeControl(&reader, &control)){
        printf("[Decoder] Failed into reader control");
        return NULL;
    }

    uint8_t haveData; //bool
    if(!ReaderU8(&reader, &haveData)){
        printf("[DEcoder] Failed into reader haveData");
        return NULL;
    }

    struct Data data = {0};
    if(haveData == 1){
        if(!DecodeData(&reader, &data)){
            printf("[Decoder] Failed into reader data");
            return NULL;
        }
    }

    struct Message *message = CreateDecodeMessage(messageId, retry, type, dest, or, ownerID, &transfer, &control, haveData ? &data : NULL);
    if(!message) return NULL;

    return message;
}

/*
* Especifica para decodificar mensagens
    - Destino e Origem sempre correspondem aos do bastão
    - aloca e devolve uma copia da mensagem
    - nao altera nenhum dado, só verifica integridade
*/
struct Message *CreateDecodeMessage (char *messageId, int messageRetry, enum MessageType type, int dest, int ori, int messageOwnerId, struct Transfer *transfer, struct Control *control, struct Data *data) {
    if(!messageId|| !transfer || !control) return NULL;

    struct Message *message = calloc(1, sizeof(struct Message));
    if(!message) return NULL;

    message->Retry = messageRetry;
    message->OwnerId = messageOwnerId;
    message->Type = type;
    message->DestinationId = dest;
    message->OriginId = ori;

    message->Id = strdup(messageId);
    if(!message->Id) goto error;

    // copia transfer
    if(!transfer->TransferId) goto error;
    
    message->Transfer = CreateTransfer(transfer->TransferId, ori, dest, transfer->AvailablePaths, transfer->PathVersion, transfer->Status);
    if(!message->Transfer) goto error;

    // copia control
    if(!control->ControlId) goto error;

    message->Control = CreateControl(control->ControlId, control->State);
    if(!message->Control) goto error;

    // copia data
    if(data){
        message->Data = CreateCompleteData(data->Round, data->MaxRound, data->Event, data->Bets, data->Reward);
        if(!message->Data) goto error;
    }

    return message;

    error:
        DestroiMessage(message);
        return NULL;
}

/*
* Especifica para criar bastões
    - Endpoint para buscar vizinhos próx
    - data sempre atualizado, caso seja type data
    - caso não seja transferencia, destino e origem iguais ao da mensagem
*/
struct Message *CreateMessage (char *messageId, int messageOwner, int retry, struct Endpoint *endpoint, int transferEndpointDest, int transferEndpointOr, enum MessageType type, struct Data *data){
    if(!endpoint || !messageId) return NULL;

    struct Message *message = calloc(1, sizeof(struct Message));
    if(!message) return NULL;

    message->Retry = retry;
    message->Type = type;
    message->OwnerId = messageOwner;
    message->DestinationId = endpoint->EndpointNext; // o destino é sempre seu vizinho próx
    message->OriginId = endpoint->Id; // origem ele mesmo 

    message->Id = strdup(messageId);
    if(!message->Id) goto error;

    message->Control = CreateAndInitControl(messageId, message->OriginId, message->DestinationId);
    if(!message->Control) goto error;

    message->Transfer = CreateAndInitTransfer(message->Control->ControlId, transferEndpointOr, transferEndpointDest);
    if(!message->Transfer) goto error;

    if(data){
        message->Data = CreateCompleteData(data->Round, data->MaxRound, data->Event, data->Bets, data->Reward);
        if(!message->Data) goto error;
    }

    return message;

    error:
        DestroiMessage(message);
        return NULL;
}

struct Message *UpdateMessage (struct Message *message, struct Endpoint *endpoint, enum MessageType type, struct Data *data){

}

void DestroiMessage (struct Message *message){
    if(!message) return;

    if(message->Id) free(message->Id);
    if(message->Transfer) DestroiTransfer(message->Transfer);
    if(message->Control) DestroyControl(message->Control);
    if(message->Data) DestroiData(message->Data);

    free(message);
}


void LogMessage(struct Message *msm){
    printf("[LOGGER] MessageId: %s\n", msm->Id);
    printf("[LOGGER] MessageRetry: %d\n", msm->Retry);
    printf("[LOGGER] MessageDestination Id: %d\n", msm->DestinationId);
    printf("[LOGGER] MessageOrigin Id: %d\n", msm->OriginId);
    printf("[LOGGER] MessageOwner: %d\n", msm->Id);
    printf("[LOGGER] MessageTransfer Id: %s\n", msm->Transfer->TransferId);
    printf("[LOGGER] MessageTransfer State: %d\n", msm->Transfer->Status);
    printf("[LOGGER] MessageTransfer Destination Id: %s\n", msm->Transfer->DestinationEndpointId);
    printf("[LOGGER] MessageTransfer Origin Id: %s\n", msm->Transfer->OriginEndpointId);
    printf("[LOGGER] MessageTransfer PathVersion: %d\n", msm->Transfer->PathVersion);
    printf("[LOGGER] MessageControl Id: %s", msm->Control->ControlId);
    printf("[LOGGER] MessageControl State: %s", msm->Control->State);
}

