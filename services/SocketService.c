#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "SocketService.h"


static int server_fd = -1;
static int connected_socket = -1;

struct Response CreatingServer(struct Network *network) {
    struct Response response;

    if(server_fd >= 0){
        // mostra erro porém não fecha server
        response.Message = "[Error]: an existent server found";
        response.State = false;
        return response;
    }

    struct sockaddr_in address = {0};
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0)) < 0) {
        ErrorAndClose(&response, "[Socket]: Error creating");
        return response;
    }

    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        ErrorAndClose(&response, "[Socket]: Error set socket opt");
        return response;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(network->Port);

    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0){
        ErrorAndClose(&response, "[Socket]: Error bind");
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
        response.Message = "[ERROR]: Failed in convert ip";
        response.State = false;
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
            
        }
    }
}

struct Message *ReceiveMessage() {

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

// para cada erro crítico mostra mensagem e fecha conexão
void ErrorAndClose(struct Response *response, char *msm) {
    response->Message = msm;
    response->State = false;

    StopAllSocketConnection();
}

