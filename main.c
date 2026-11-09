#include <stdio.h>
#include <stdlib.h>
#include "./entities/utils/Response.h"
#include "./entities/network/endpoint/Endpoint.h"
#include "./services/eventController/EventControllerService.h"

int main (int argc, char *argv[]) {
    /**
     * PARA TESTE LOCAL !!
     *  - Usuário digita id da maquina que quer ser
    */
    if (argc < 2) {
        printf("Uso: %s <endpointId>\n", argv[0]);
        return 1;
    }
    int endpointId = atoi(argv[1]);
    if (endpointId < 0 || endpointId >= TOTAL_ENDPOINTS) {
        printf("[ERROR] Endpoint ID invalido!\n");
        return 1;
    }

    printf("====== Bem-vindo ====== \n");
    printf("\n");
    printf("[INIT] Preparando a maquina %d. Aguarde! \n", endpointId);

    struct Endpoint *endpoint = CreateAndPrepareEndpoint(endpointId);    
    if(!endpoint) return 0;
    
    printf("[INIT] Maquina preparada com sucesso! \n");
    LoggerEndpoint(endpoint);

    printf("[INIT] Iniciando jogada! Aguarde enquanto configuramos a rede...\n");
    if(RunEventController(endpoint))
        printf("[EXEC] Não conseguimos terminar a jogada! Encerrando programa");
    
    else
        printf("[EXEC] Jogada encerrada com exito! Encerrando programa");

    StopEventController(endpoint);
    return 0; 
}