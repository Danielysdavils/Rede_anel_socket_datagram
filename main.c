#include <stdio.h>
#include "./entities/utils/Response.h"
#include "./entities/network/endpoint/Endpoint.h"
#include "./services/eventController/EventControllerService.h"

int main () {
    // apresenta
    // identifica maquina local
    // cria endpoint
    // na primeira incialização identifica server por ordem
    // inicia eventLoop con endpoint
    // termina eventLoop -> limpa 

    printf("====== Bem-vindo ======");

    printf("[INIT] Preparando sua maquina. Aguarde!");

    struct Response response = PrepareEndpoint();    
    if(!response.State){
        printf("[INIT-ERRO] Erro ao preparar sua maquina: %s", response.Message);
        ClearEndpoint();
        return 0;
    }

    struct Endpoint *endpoint = (struct Endpoint *)response.Data;

    printf("[INIT] Maquina preparada com sucesso!");

    printf("[INIT] - Seu IP: %s", endpoint->Network->Ip);
    printf("[INIT] - Sua porta: %d", endpoint->Network->Port);
    printf("[INIT] - Seu identificador: %d", endpoint->Id);

    printf("[INIT] Iniciando jogada! Aguarde enquanto configuramos a rede...");
    if(RunEventController(&endpoint))
        printf("[EXEC] Não conseguimos terminar a jogada! Encerrando programa");
    
    else
        printf("[EXEC] Jogada encerrada com exito! Encerrando programa");

    StopEventController(&endpoint);
    return 0; 
}