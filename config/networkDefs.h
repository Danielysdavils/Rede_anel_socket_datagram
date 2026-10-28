#ifndef __NETWORKDEFS__
#define __NETWORKDEFS__

/*
    Definições Globais de Execução
    
    Todas as máquinas vão seguir as mesmas especificações 
    contidas neste arquivo
*/

#define MAX_RETRYS 10

// timeout máximo para que alguma mensagem seja recebida por qualquer endpoint
// se uma máquina, após esse tempo, não recebe mensagem alguma
// se considera morta (fora de rede)
// endpoint.enabled = false
#define MAX_HEARTBEAT_MESSAGE_TIMEOUT 8000

// tempo máximo a aguardar por alguma mensagem
// se nenhuma mensagem é recebida por alguma máquina nesse tempo então
// marca erro no caminho
#define MAX_ANSWER_TIMEOUT 2000

// tempo máximo a aguardar por um ack de retorno sobre
// recebimento de mensagem
// Maquina tentará enviar mensagens indefinidamente até acabar o timeout
#define MAX_RETRYS 10

// tempo máx de retransferencia de uam mensagem
// * o tempo é maior porque considera outros caminhos entre endpoints
#define MAX_RETRANSFER_TIMEOUT 5000

// total de maquinas a receber/enviar mensagens
#define TOTAL_ENDPOINTS 4

// Identificador global para cada máquina
#define A 0
#define B 1
#define C 2
#define D 4

// para histórico local de transferencias recebidas
#define MAX_TRANSFERS 32

#define MAX_TRANSFER_ID_SIZE 64

// tamanho máximo para criação de ids 
#define TAM_LIMIT_ID 50

#endif
