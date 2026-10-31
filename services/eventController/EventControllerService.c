#include <poll.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "EventControllerService.h"
#include "../socket/SocketService.h"
#include "../../entities/utils/Input.h"
#include "../../entities/game/player/Player.h"
#include "../../entities/network/message/Message.h"
#include "../../entities/game/server/Server.h"

struct EventController *CreateEventController() {
    struct EventController * ev = malloc(sizeof(struct EventController));
    if(!ev){
        printf("[RUN_ERRO]: Creating event controller");
        return NULL;
    }

    ev->CurrentEvent = INIT;
    ev->MaxRounds = 0;
    ev->IsWaitingInput = false;
    ev->currentRound = 1;

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

    // inicializo os dois, porém os preencho na hora de receber uma mensagem.
    struct Player *player = NULL;
    struct PlayerServer *server = NULL;

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
                // aqui já cria uma nova mensagem
                // com dados de destino e origem correspondente a essa maquina
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
                struct Message *message = (struct Message *)re.Data;
                
                if(message->Type == DATA){
                    // verifico se sou o dono
                    // se sou dono então sou server
                    if(message->OwnerId == endpoint->Id){
                        // sou server, cria uma vez no começo
                        if(!server) server = CreatePlayerServer(endpoint->Id);
                    
                        // gero as respostas do servidor
                        struct Data *data = GenerateServerData(message->Data, server);
                        
                        // crio novo bastão, com novos evs, etc
                        ctr->currentRound++;
                        ctr->CurrentEvent = GetNextEvent(message->Data->Event);
                        
                        // atualizo a nova data com o novo evento próximo
                        data->Event = ctr->CurrentEvent;
                        data->Round = ctr->currentRound;

                        message->Id = GenerateId(ctr->currentRound, endpoint->Id);
                        message->Data = data;

                        // marco o controll

                        // envio
                        re = SendMessage(message);
                        if(!re.State){
                            // mensagem não pode ser enviada
                            // fazer uma outra tentativa
                        }

                        printf("[RUN] Message enviada: %s", message->Id);
                        
                        DestroiData(data);
                        // lembrar destruir de novo no EVENT Fin
                    
                    }else{
                        // nao sou dono da mensagem

                        // preciso ser o novo dono? ou seja, passar a ser servidor?
                        // preciso ser o novo dono se 
                        //   - antes era jogador 
                        //   - dono da mensagem sou eu
                        //   - evento da mensagem [INIT]
                        if(!endpoint->IsServer && message->OwnerId == endpoint->Id){
                            // como preciso ser o novo dono, destruo player existente
                            // crio server
                            endpoint->IsServer = true;

                            if(player){
                                DestroiPlayer(player);
                                player = NULL;
                            }

                            if(!server) server = CreatePlayerServer(endpoint->Id);

                            ctr->CurrentEvent = INIT;
                            ctr->currentRound = 1;
                            
                            AskUserInput("[Entry] Quantas rodadas deseja realizar?", &ctr->IsWaitingInput, SERVER_ASK_ROUNDS, MAX_ROUNDS_PER_USER);
                            // manda mensagem no controle de respostas

                        }else {
                            // não preciso ser o novo dono, sou player, continuo player
                            if(!player) player = CreatePlayer(endpoint->Id);

                            // sou player, desestruturo msm
                            // importante, atualizo dados locais
                            ctr->CurrentEvent = message->Data->Event;
                            ctr->currentRound = message->Data->Round;

                            // peço dados
                            struct data *data = GeneratePlayerData(message->Data, player);
                        
                            // o unico que muda são dados e control
                            // envio bastão
                            SendMessage(message);

                            DestroiData(data);
                        }
                    }

                    continue;
                }

                if(message->Type == ACK){
                    continue;
                }

                // destruo a mensage
                DestroiMessage(message);
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
                        player->Bet.type = response.data;
                        break;

                    case PLAYER_ASK_BET_VALUE:
                        // valor da aposta
                        player->Bet.value = response.data;
                        // gera mensagem
                        // manda bastão 
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