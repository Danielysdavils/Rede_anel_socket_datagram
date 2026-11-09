#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "./SocketService.h"

static int udp_socket = -1;

int GetServerSocket(void){
    return udp_socket;
}

void StopConnection() {
    if(udp_socket >= 0) close(udp_socket);
}

struct Response CreatingServer(struct Network *network) {
    printf("[RUN]: Creating server socket \n");
    struct Response response;

    if(udp_socket >= 0){
        // mostra erro porém não fecha server
        ResponseError(&response, "[Error]: an existent server found \n");
        return response;
    }

    struct sockaddr_in address = {0};
    int opt = 1;

    if ((udp_socket = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0)) < 0) {
        ResponseError(&response, "[Socket]: Error creating \n");
        StopConnection();
        return response;
    }

    if(setsockopt(udp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        ResponseError(&response, "[Socket]: Error set socket opt \n");
        StopConnection();
        return response;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(network->Port);

    if(bind(udp_socket, (struct sockaddr*)&address, sizeof(address)) < 0){
        ResponseError(&response, "[Socket]: Error bind \n");
        StopConnection();
        return response;
    }

    response.Message = "[Socket]: Server created successfully! \n";
    response.State = true;

    return response;
}

struct Response SendMessage(struct Message *message) {
    struct Response response = {0};

    printf("connected socjet %d \n", udp_socket);

    if(!message || udp_socket < 0){
        ResponseError(&response, "[ERROR]: Invalid message or socket");
        return response;
    }

    struct sockaddr_in destination = {0};

    struct EndpointDto dest = GetEndpointConfig(message->DestinationId);
    uint16_t destination_port = (uint16_t)dest.Port;
    char *destination_ip = dest.Ip;

    destination.sin_family = AF_INET;
    destination.sin_port = htons(destination_port);

    if(inet_pton(AF_INET, destination_ip, &destination.sin_addr) != 1){
        ResponseError(&response, "[ERROR]: Failed in convert ip");
        return response;
    }

    uint8_t data[TAM_BUFFER];
    size_t capacity = sizeof(data);

    size_t len = EncodeMessage(message, data, capacity);
    if(len <= 0){
        ResponseError(&response, "[ERROR]: Failed to encode message");
        return response;
    }

    printf("[SOCKET] Message to SEND size: %zu / %d bytes\n", len, TAM_BUFFER);

    ssize_t sent = sendto(
        udp_socket,
        data,
        len,
        0,
        (struct sockaddr *)&destination,
        sizeof(destination)
    );

    if(sent < 0){
        if(errno == EAGAIN || errno == EWOULDBLOCK){
            ResponseErrorCode(&response, "[ERRO]: Socket temp indisponivel", CODE_UNAVAILABLE);
            return response;
        }

        ResponseError(&response, "[ERROR]: In try to send. Invalid data received");
        return response;
    }

    if ((size_t)sent != len) {
        ResponseError(&response, "[ERROR]: Datagram not fully sent");
        return response;
    }

    ResponseAccert(&response, "[SOCKET]: Message sended!");
    return response;
}

struct Response ReceiveMessage() {
    struct Response response = {0};

    if(udp_socket < 0){
        ResponseError(&response, "[ERROR]: Invalid socket \n");
        return response;
    }

    uint8_t buffer[TAM_BUFFER];

    struct sockaddr_in sender = {0};
    socklen_t senderLenght = sizeof(sender);

    ssize_t received = recvfrom(
        udp_socket,
        buffer,
        sizeof(buffer),
        0,
        (struct sockaddr *)&sender,
        &senderLenght
    );

    if(received < 0){
        if(errno == EAGAIN || errno == EWOULDBLOCK){
            // nao ha dadtagrama disponíveis
            printf("[SOCKET]: ANY DATAGRAM \n");
            ResponseAccertData(&response, NULL, CODE_ANY_DATA);
            return response;
        }

        ResponseErrorData(&response, "[ERRO]: Erro no recvfrom \n", CODE_GENERAL_ERROR);
        return response;
    }

    if(received == 0){
        ResponseErrorData(&response, "[ERRO] Datagram empty \n", CODE_ANY_DATA);
        return response;
    }

    printf("[SOCKET] Message RECEIVED size: %zu / %d bytes\n", (size_t)received, TAM_BUFFER);

    struct Message *data = DecodeMessage(buffer, (size_t)received);
    if(data == NULL){
        ResponseErrorData(&response, "[ERRO] Error decoding data! \n", CODE_GENERAL_ERROR);
        return response;
    }

    ResponseAccertData(&response, data, CODE_GENERAL_ACCERT);
    return response;    
}


void StopAllSocketConnection() {
    StopConnection();
}


