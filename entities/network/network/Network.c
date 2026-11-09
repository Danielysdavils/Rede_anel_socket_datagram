#include "Network.h"

struct Network* CreateNetwork(char *Ip, int port){
    struct Network *net = malloc(sizeof(struct Network));
    if (!net) {
        printf("[Network_ERROR]: Error in CreateNetwork function. Creating malloc");
        return NULL;
    }

    net->Ip = Ip;
    net->Port = port;
    return net;
}


void DestroyNetwork(struct Network *net){
    if(!net) return;
    free(net);
}