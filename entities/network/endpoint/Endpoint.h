#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <stdbool.h>
#include "../../../config/networkDefs.h"
#include "../network/Network.h"
#include "../../utils/Response.h"

struct Endpoint
{
    // identificador único para o endpoint
    int Id;
    // bool para identificar se é player ou servidor
    bool IsServer; 
    // configuração de rede do endpoint (socket)
    struct Network *Network;
    // endpoint seguinte
    int Next_endpoint_id;
    // endpoint posterior
    int Previous_endpoint_id;
    // identificador se está vivo/activo (na rede)
    bool enabled;
    // dados locais do endpoint
    struct Storage *Storage;
};

struct Response PrepareEndpoint();

struct Endpoint* CreateEndpoint (struct Network *net, int id, bool isServer, struct Storage *storage);

void DestroyEndpoint (struct Endpoint *endpoint);

#endif