#ifndef __NETWORK__
#define __NETWORK__

#include <stdio.h>
#include <stdlib.h>

struct Network {
    // endereço Ipv4 da maquina
    char *Ip;
    // porta da maquina
    int Port;
};

struct Network* CreateNetwork(char *Ip, int port);

void DestroyNetwork(struct Network *net);

#endif