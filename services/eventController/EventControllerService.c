#include <poll.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "EventControllerService.h"
#include "../socket/SocketService.h"

struct EventController *CreateEventController() {
    struct EventController * ev = malloc(sizeof(struct EventController));
    if(!ev){
        printf("[RUN_ERRO]: Creating event controller");
        return NULL;
    }

    ev->CurrentEvent = INIT;
    ev->MaxRounds = 0;
}

void AskRoundData (struct EventController *ctr){
    if(ctr->IsWaitingInput) return;

    ctr->IsWaitingInput = true;

    printf("[RUN] Quantas jogadas deseja realizar?");
    fflush(stdout);
}

void HandleUserInput(struct EventController *ctr){
    char buffer[BUFFER_USER_INPUT];

    if(!fgets(buffer, sizeof(buffer), stdin)) return;

    char *end;
    long rounds = strtol(buffer, &end, 10);

    if(end == buffer || rounds <= 0 || rounds > MAX_ROUNDS_PER_USER) {
        printf("[RUN_ERRO] Entrada inválida. Número máx possível: %d", MAX_ROUNDS_PER_USER);
        fflush(stdout);
        return;
    }
}

bool RunEventController (struct Endpoint *endpoint) {
    // primeiro verificar se é servidor ou não
    // perguntar ao usuário quantas jogadas -> só servidor pergunta
    // ao final de cada jogada pode decidir se sair? verificar se posso
    
    // crio servidor socket datagram passando conf especificas do endpoint
    struct Response re = CreatingServer(endpoint->Network);
    if(!re.State){
        printf("[RUN_ERRO]: %s", re.Message);
        return false;
    }

    int connected_socket = (int)re.Data;
    struct pollfd fds[2] = {
        { .fd = connected_socket, .events = POLLIN }, // descritor para ev do socket
        { .fd = STDIN_FILENO, .events = POLLIN } // descritor para entradas do usuário
    };
    
    struct EventController *ctr = CreateEventController();
    if(!ctr) return false;

    if(endpoint->IsServer){
        printf("[RUN] Parabéns! você será server nessa jogada!");
        AskRoundData(ctr);
    }else 
        printf("[RUN] Parabéns! você será player nessa jogada!");

    while (1) {
        int result = poll(fds, 2, 1000);

        if(result < 0){
            if(errno == EINTR)
                continue;
            
            printf("[RUN_ERRO] Poll error");
            break;
        }

        // [EV]: Mensagem recebida
        if(fds[0].revents & POLLIN){
            while (1) {
                re = ReceiveMessage();
                
                if(re.State && re.Code == CODE_ANY_DATA){
                    // fila ta vazia
                    break;
                }
                
                if(!re.State){
                    printf("[RUN_ERRO] %s", re.Message);
                    break;
                }

                // messagem recebida
                // verifico sou dono?
                //   sou dono e teve transição de server? 
                //   nao sou dono teve transição de server?
                // altero dados locais conforme msm (matriz do storage, etc) 
            }
        }

        // [Controll]: Usuário digita resposta
        if(ctr->IsWaitingInput && fds[1].revents & POLLIN){
            HandleUserInput(ctr);
        }
    }
}


void StopEventController (struct Endpoint *endpoint) {

}