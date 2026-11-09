#include "Endpoint.h"

// Retorna configuração de endpoint do paramatro 
// **configuração de cada endpoint é fixa no /config/networkConfig.h
struct EndpointDto GetEndpointConfig(int endpointId){
    struct EndpointDto endpoint;
    switch (endpointId)
    {
        case (int)A_ID : {
            endpoint.EndpointID = A_ID;
            endpoint.Ip = A_IP;
            endpoint.Port = A_PORT;
            break;
        }
        case (int)B_ID : {
            endpoint.EndpointID = B_ID;
            endpoint.Ip = B_IP;
            endpoint.Port = B_PORT;
            break;
        }
        case (int)C_ID : {
            endpoint.EndpointID = C_ID;
            endpoint.Ip = C_IP;
            endpoint.Port = C_PORT;
            break;
        }
        case (int)D_ID : {
            endpoint.EndpointID = D_ID;
            endpoint.Ip = D_IP;
            endpoint.Port = D_PORT;
            break;
        }
        default:
            break;
    }

    return endpoint;
}


struct Endpoint* CreateAndPrepareEndpoint (int localID){
    struct Endpoint *endpoint = malloc(sizeof(struct Endpoint));
    if (!endpoint) {
        printf("[ENDPOINT_ERROR]: Error in CreateEndpoint function. Creating malloc");
        return NULL;
    }

    // recupero meus dados locais
    struct EndpointDto local = GetEndpointConfig(localID);

    endpoint->Id = local.EndpointID;
  
    // primeira maquina sempre começa como server
    endpoint->IsServer = endpoint->Id == 0 ? true : false;

    int nextId = 0;
    int previousId = 0;
    if(endpoint->Id == 0){
        nextId = endpoint->Id + 1;
        previousId = TOTAL_ENDPOINTS - 1;

        endpoint->EndpointNext = nextId;
        endpoint->EndpointPrevious = previousId;

    }else if(endpoint->Id == TOTAL_ENDPOINTS - 1){
        nextId = 0;
        previousId = endpoint->Id - 1;

        endpoint->EndpointNext = nextId;        
        endpoint->EndpointPrevious = previousId;
    
    }else{
        nextId = endpoint->Id + 1;
        previousId = endpoint->Id - 1;

        endpoint->EndpointNext = nextId;
        endpoint->EndpointPrevious = previousId;
    }

    struct Network *net = CreateNetwork(local.Ip, local.Port);
    if(net == NULL){
        DestroyEndpoint(endpoint);
        return NULL;
    }

    struct Storage *storage = CreateStorage();
    if(storage == NULL){
        DestroyEndpoint(endpoint);
        DestroyNetwork(net);
        return NULL;
    }

    endpoint->Network = net;
    endpoint->Storage = storage;
    endpoint->enabled = true;

    return endpoint;
}


void DestroyEndpoint (struct Endpoint *endpoint) {
    if (endpoint == NULL) return;

    if(endpoint->Storage) free(endpoint->Storage);
    if(endpoint->Network) free(endpoint->Network);

    free(endpoint);
}

void LoggerEndpoint(struct Endpoint *endpoint){
    printf("[LOGGER_ENDPOINT]: Endpoint ID %d \n", endpoint->Id);
    printf("[LOGGER_ENDPOINT]: Endpoint Enable %d \n", endpoint->enabled);
    printf("[LOGGER_ENDPOINT]: Endpoint Next %d \n", endpoint->EndpointNext);
    printf("[LOGGER_ENDPOINT]: Endpoint Previous %d \n", endpoint->EndpointPrevious);
    printf("[LOGGER_ENDPOINT]: Endpoint IsServer %d \n", endpoint->IsServer);
    printf("[LOGGER_ENDPOINT]: Endpoint Ip %s \n", endpoint->Network->Ip);
    printf("[LOGGER_ENDPOINT]: Endpoint Port %d \n", endpoint->Network->Port);
}
