#include "Endpoint.h"

struct Endpoint* CreateAndPrepareEndpoint (){
    struct Endpoint *endpoint = malloc(sizeof(struct Endpoint));
    if (!endpoint) {
        printf("[ENDPOINT_ERROR]: Error in CreateEndpoint function. Creating malloc");
        return NULL;
    }

    struct Network *net = CreateNetwork(LOCAL_IP, PORT);
    if(net == NULL){
        DestroyEndpoint(endpoint);
        return NULL;
    }

    endpoint->Id = LOCAL_ID;
    // primeira maquina sempre começa como server
    endpoint->IsServer = endpoint->Id == 0 ? true : false;

    if(endpoint->Id == 0){
        endpoint->Next_endpoint_id = endpoint->Id++;
        endpoint->Previous_endpoint_id = TOTAL_ENDPOINTS - 1;
    
    }else if(endpoint->Id == TOTAL_ENDPOINTS - 1){
        endpoint->Next_endpoint_id = 0;        
        endpoint->Previous_endpoint_id = endpoint->Id--;
    
    }else{
        endpoint->Next_endpoint_id = endpoint->Id++;
        endpoint->Previous_endpoint_id = endpoint->Id--;
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
    if (endpoint->Storage) free(endpoint->Storage);
    if (endpoint->Network) free(endpoint->Network);

    if(endpoint) free(endpoint);
    endpoint = NULL;
}

