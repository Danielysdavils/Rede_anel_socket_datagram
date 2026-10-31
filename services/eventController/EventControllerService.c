#include <poll.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "EventControllerService.h"
#include "../socket/SocketService.h"
#include "../../entities/utils/Input.h"


struct EventController *CreateEventController() {
    struct EventController * ev = malloc(sizeof(struct EventController));
    if(!ev){
        printf("[RUN_ERRO]: Creating event controller");
        return NULL;
    }

    ev->CurrentEvent = INIT;
    ev->MaxRounds = 0;
    ev->IsWaitingInput = false;

    return ev;
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

    // configuro input
    ConfigureInput();
    
    if(endpoint->IsServer){
        printf("[RUN] Parabéns! você será server nessa jogada!");
        // pergunta só uma vez antes de iniciar o poll para começar de fato a jogada
        // depois pergunta novamente só se tiver transferencia de player->servidor
        AskUserInput("[Entry] Quantas rodadas deseja realizar?", &ctr->IsWaitingInput, SERVER_ASK_ROUNDS, MAX_ROUNDS_PER_USER);
    
    }else 
        printf("[RUN] Parabéns! você será player nessa jogada!");

    bool endWithoutErro = true;
    while (1) {
        // para que ignore terminal quando nao há pergunta do usuário
        fds[1].fd = ctr->IsWaitingInput ? STDIN_FILENO : -1;

        int result = poll(fds, 2, 1000);

        if(result < 0){
            if(errno == EINTR)
                continue;
            
            printf("[RUN_ERRO] Poll error");
            endWithoutErro = false;
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

                // conforme mensagem recebida pergunta ao usuário
                // precisa mandar o evento da pergunta
                // para que ao chegar ao [controll] saiba que evento executar
            }
        }

        // [Controll]: Usuário digita resposta
        if(ctr->IsWaitingInput && fds[1].revents & POLLIN){
            // aqui chegam todas as entradas do usuário
            // importante distruibuir respostas e executar ações conforme resposta
            struct InputResponse response = HandleUserInput(&ctr->IsWaitingInput);
            if(response.data < 0) {
                printf("[RUN_ERRO] Erro ao ler entrada do usuário");
                // adicionar uma outra tentativa ? da mesma pergunta ? 
                endWithoutErro = false;
                break;

            }else if (response.data > 0){
                switch (response.event) {
                    case SERVER_ASK_ROUNDS:
                        ctr->MaxRounds = response.data;
                        // cria mensagem 
                        // manda bastão
                        break;
                    
                    case SERVER_FINISH_ROUND:
                        // manda evento fim
                        // 
                        break;
                    
                    // (*) MARIA: COLOCAR AQUI DO JOGADOR DEPENDENDO DO EVENTO
                    case PLAYER_ASK_BET_TYPE:
                        // tipo da aposta
                        // 
                        break;

                    case PLAYER_ASK_BET_VALUE:
                        // valor da aposta
                        //
                        break;

                    case PLAYER_ASK_WANNA_PLAY:
                        // se quer se juntar a essa rodada
                        break;

                    default:
                        break;
                }
            }
        }
    }

    return endWithoutErro;
}



void StopEventController (struct Endpoint *endpoint) {

}