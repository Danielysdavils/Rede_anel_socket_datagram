#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "SocketService.h"

#define TAM_BUFFER 1024

static int server_fd = -1;
static int connected_socket = -1;


struct Response CreatingServer(struct Network *network) {
    struct Response response;

    if(server_fd >= 0){
        // mostra erro porém não fecha server
        ResponseError(&response, "[Error]: an existent server found");
        return response;
    }

    struct sockaddr_in address = {0};
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0)) < 0) {
        ResponseError(&response, "[Socket]: Error creating");
        StopServer();
        return response;
    }

    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        ResponseError(&response, "[Socket]: Error set socket opt");
        StopServer();
        return response;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(network->Port);

    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0){
        ResponseError(&response, "[Socket]: Error bind");
        StopServer();
        return response;
    }

    response.Message = "[Socket]: Server created successfully!";
    response.State = true;

    return response;
}

struct Response SendMessage(struct Message *message) {
    struct Response response;

    struct sockaddr_in destination = {0};
    
    int destination_port = message->Destination.Port;
    int destination_ip = message->Destination.Ip;

    destination.sin_family = AF_INET;
    destination.sin_port = htons(destination_port);

    if(inet_pton(AF_INET, destination_ip, &destination.sin_addr) != 1){
        ResponseError(&response, "[ERROR]: Failed in convert ip");
        return response;
    }

    size_t size = strlen(message);
    ssize_t sent = sendto(
        connected_socket,
        message,
        size,
        0,
        (struct sockaddr *)&destination,
        sizeof(destination)
    );

    if(sent < 0){
        if(errno == EAGAIN || errno == EWOULDBLOCK){
            ResponseErrorCode(&response, "[ERRO]: Socket temp indisponivel", CODE_UNAVAILABLE);
            return response;
        }

        ResponseError(&response, "[ERRO]: In try to send");
        return response;
    }

    ResponseAccert(&response, "[SOCKET]: Message sended!");
    return response;
}

struct Response ReceiveMessage() {
    struct Response response;
    char buffer[TAM_BUFFER];

    struct sockaddr_in sender = {0};
    socklen_t senderLenght = size_of(sender);

    ssize_t received = recvfrom(
        connected_socket,
        buffer,
        sizeof(buffer),
        0,
        (struct sockaddr *)&sender,
        &senderLenght
    );

    if(received < 0){
        if(errno == EAGAIN || errno == EWOULDBLOCK){
            // nao ha dados disponíveis
            ResponseAccertData(&response, NULL, CODE_ANY_DATA);
            return response;
        }

        ResponseErrorData(&response, "[ERRO]: Erro no recvfrom", CODE_GENERAL_ERROR);
        return response;
    }

    struct Message *data = DecodeMessage(received);

    ResponseAccertData(&response, data, CODE_GENERAL_ACCERT);
    return response;    
}

void StopConnection() {
    if(connected_socket >= 0) close(connected_socket);
}

void StopServer() {
    if(server_fd >= 0) close(server_fd);
}

void StopAllSocketConnection() {
    StopConnection();
    StopServer();
}


