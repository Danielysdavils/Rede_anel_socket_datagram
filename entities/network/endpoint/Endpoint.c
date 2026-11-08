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


struct Endpoint* CreateAndPrepareEndpoint (){
    struct Endpoint *endpoint = malloc(sizeof(struct Endpoint));
    if (!endpoint) {
        printf("[ENDPOINT_ERROR]: Error in CreateEndpoint function. Creating malloc");
        return NULL;
    }

    // recupero meus dados locais
    struct EndpointDto local = GetEndpointConfig(LOCAL_ID);

    endpoint->Id = local.EndpointID;
    // primeira maquina sempre começa como server
    endpoint->IsServer = endpoint->Id == 0 ? true : false;

    int nextId = 0;
    int previousId = 0;
    if(endpoint->Id == 0){
        nextId = endpoint->Id++;
        previousId = TOTAL_ENDPOINTS - 1;

        endpoint->EndpointNext = nextId;
        endpoint->EndpointPrevious = previousId;
    
    }else if(endpoint->Id == TOTAL_ENDPOINTS - 1){
        nextId = 0;
        previousId = endpoint->Id--;

        endpoint->EndpointNext = nextId;        
        endpoint->EndpointPrevious = previousId;
    
    }else{
        nextId = endpoint->Id++;
        previousId = endpoint->Id--;

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

