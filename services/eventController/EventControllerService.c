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
#include "../../entities/game/data/Data.h";
#include "../../entities/network/transfer/Transfer.h"


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


static struct Message *_temporaryMessage = NULL;

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
    if(!ctr){
        StopAllSocketConnection();
        return false;
    }

    // configuro input
    int result = ConfigureInput();
    if(result < 0){
        printf("[RUN_ERRO]: Não foi possível configurar entradas do usuário");
        
        StopAllSocketConnection();
        DestroiEventController(ctr);

        return false;
    }
    
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
                
                // bastão e é para mim
                if(message->Type == DATA && message->Destination.EndpointID == endpoint->Id){
                    // sou o dono e se sou server
                    if(message->OwnerId == endpoint->Id && endpoint->IsServer){
                        // sou server, cria uma vez no começo
                        if(!server) server = CreatePlayerServer(endpoint->Id);

                        // gero as respostas do servidor
                        // aqui não preciso fazer tratamento de evento porque em todas servidor gera dado
                        // o tratamento é interno na função
                        struct Data *data = GenerateServerData(message->Data, server);
                        
                        char *newMessageId = message->Id;
                        int messageOwner = message->OwnerId;

                        // Quer dizer que terminou rodada e ainda não cumpli o limite de rodadas do servidor
                        if(message->Data->Event == END && server->LocalRound < ctr->MaxRounds){
                            // crio uma nova mensagem com:
                            //  - novo id, pois é novo inicio de rodada
                            //  - mesmo owner, pois ainda to no limite de rodadas desse servidor
                            //  - incremento localRound
                                
                            ctr->currentRound++;
                            ctr->CurrentEvent = INIT;

                            // crio o id apontando para esse mesmo endpoint
                            newMessageId = GenerateId(ctr->currentRound, endpoint->Id);
                            server->LocalRound++;
                        }
                        // terminou a rodada e terminei minhas rodadas máximas do servidor
                        else if(message->Data->Event == END && server->LocalRound == ctr->MaxRounds){
                            // crio nova mensagem com:
                            //  - novo id, pois é novo inicio de rodada
                            //  - novo owner, pois há transferencia de servidor
                            //  - aumento currentRound para gerar message ao prox dono já atualizado
                            // também:
                            //  - destruo servidor atual
                            //  como não uso mais maxRounds posso deixar assim ou zera
                            ctr->currentRound++;
                            ctr->CurrentEvent = INIT;

                            newMessageId = GenerateId(ctr->currentRound, endpoint->Next_endpoint_id);
                            messageOwner = endpoint->Next_endpoint_id;

                            // Como deixo de ser servidor
                            if(server){
                                DestroiServer(server);
                                server = NULL;
                            }

                            endpoint->IsServer = false;
                        } 
                        // só transferencia de evento
                        else if(message->Data->Event != END) {
                            // crio nova mensagem com:
                            //  - mesmo id, pois é só novo evento
                            //  - mesmo owner
                            //  - currentRound iguak, current Event muda

                            ctr->CurrentEvent = GetNextEvent(message->Data->Event);
                        }

                        // atualizo a nova data com o novo evento próximo
                        data->Event = ctr->CurrentEvent;
                        data->Round = ctr->currentRound;

                        struct Message *newMessage = CreateMessage(newMessageId, messageOwner, ctr->currentRound, endpoint, DATA, data);
                        if(newMessage == NULL){
                            // trato err
                        }else{
                            message->Data = NULL;

                            // envio
                            re = SendMessage(newMessage);
                            if(!re.State){
                                // mensagem não pode ser enviada
                                // fazer uma outra tentativa
                            }else
                                printf("[RUN] Message enviada: %s", message->Id);
                            
                            DestroiMessage(newMessage);
                        }
                        
                        // mensagem processada, posso destruir
                        DestroiMessage(message);
                    }
                    // nao sou dono da mensagem sou player, não há transição
                    else if(message->OwnerId != endpoint->Id && !endpoint->IsServer){
                        
                        if(!player) player = CreatePlayer(endpoint->Id);

                        // sou player, desestruturo msm
                        // importante, atualizo dados locais

                        /*
                            Cuidar das retransmissões aqui: duplicatas, etc.
                        */
                        ctr->CurrentEvent = message->Data->Event;
                        ctr->currentRound = message->Data->Round;
                        
                        
                        switch (ctr->CurrentEvent)
                        {
                            case INIT: {
                                // add message de inicio para o jogador
                                // (*) mostrar num menu dados
                                bool result = UpdatePlayerData(message->Data, player);
                                if(!result){
                                    // trata erro
                                }

                                struct Message *newMessage = CreateMessage(message->Id, message->OwnerId, ctr->currentRound, endpoint, DATA, message->Data);
                                if(newMessage == NULL){

                                }else{
                                    message->Data = NULL;

                                    re = SendMessage(newMessage);
                                    if(!re.State){
                                        // nova tent? 
                                    }else 
                                        printf("[RUN] Sua mensagem foi enviada! %s", newMessage->Id);

                                    DestroiMessage(newMessage);
                                }
                                
                                // já processada, pode destruir
                                DestroiMessage(message);
                                break;  
                            
                            } case BET : {
                                // salvo message que irei usar quadno user retornar resposta
                                _temporaryMessage = message;
                                // peço dados
                                AskUserInput("[ENTRY] Qual o tipo de aposta", &ctr->IsWaitingInput, PLAYER_ASK_BET_TYPE, MAX_BET_TYPE);
                                // mando mensagem no controle de respostas
                                break;

                            } case RESULT: {
                                // resultado não gera data novo, só atualiza dados do player
                                bool result = UpdatePlayerData(message->Data, player);
                                if(!result){
                                    // mostra erro
                                    // retorna?
                                }

                                struct Message *newMessage = CreateMessage(message->Id, message->OwnerId, ctr->currentRound, endpoint, DATA, message->Data);
                                if(newMessage == NULL){
                                    // tratar
                                }else{
                                    message->Data = NULL;

                                    re = SendMessage(newMessage);
                                    if(!re.State){
                                        // nova tent? 
                                    }else 
                                        printf("[RUN] Sua mensagem foi enviada! %s", newMessage->Id);

                                    DestroiMessage(newMessage);
                                }
                                
                                // já processada, pode destruir
                                DestroiMessage(message);
                                break;  

                            } case END: {
                                // só notifica o usuário que terminou
                                struct Message *newMessage = CreateMessage(message->Id, message->OwnerId, ctr->currentRound, endpoint, DATA, message->Data);
                                if(newMessage == NULL){
                                    // tratar com retry?? (!)
                                
                                }else{
                                    message->Data = NULL;

                                    re = SendMessage(newMessage);
                                    if(!re.State){
                                        // nova ten?
                                    }else
                                        printf("[RUN] Sua mensagem foi enviada! %s", newMessage->Id);

                                    DestroiMessage(newMessage);
                                }
                                
                                DestroiMessage(message); //(*) tenho q ver
                                break;
                            }
                        }
                    }
                    // preciso ser o novo dono, há transição
                    else if(message->OwnerId == endpoint->Id && !endpoint->IsServer && message->Data->Event == INIT) {
                        // preciso ser o novo dono se 
                        //   - antes era jogador 
                        //   - dono da mensagem sou eu
                        //   - evento da mensagem [INIT]
                        
                        endpoint->IsServer = true;

                        // como preciso ser o novo dono, destruo player existente
                        if(player){
                            DestroiPlayer(player);
                            player = NULL;    
                        }

                        // crio server
                        if(!server) server = CreatePlayerServer(endpoint->Id);

                        // atualizo meus dados locais - mensagem já chega atualizada
                        ctr->CurrentEvent = message->Data->Event;
                        ctr->currentRound = message->Data->Round;
                            
                        AskUserInput("[Entry] Quantas rodadas deseja realizar?", &ctr->IsWaitingInput, SERVER_ASK_ROUNDS, MAX_ROUNDS_PER_USER);
                        // manda mensagem no controle de respostas
                        // uma nova mensagem é criada, então destruo a anterior
                        DestroiMessage(message);
                    }
                } 
                // ack para mim
                else if(message->Type == ACK && message->Destination.EndpointID == endpoint->Id){

                }
                // bastão que nao é para mim, porém vou retransmittir
                else if(message->Type == DATA && message->Destination.EndpointID != endpoint->Id && message->Transfer->DestinationEndpointId.EndpointID == endpoint->Id){

                } 
                // ack não é para mim, porém vou retransmittir
                else if(message->Type == ACK && message->Destination.EndpointID != endpoint->Id && message->Transfer->DestinationEndpointId.EndpointID == endpoint->Id){

                }
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
                    case SERVER_ASK_ROUNDS: {
                        // cria mensagem 
                        // manda bastão
                        // aqui sempre é primeiro bastão desse servidor
                        // no caso de transferencias, o bastão já vem com o round e evento setado

                        ctr->MaxRounds = response.data;

                        struct Data *data = CreateData(ctr->currentRound, ctr->MaxRounds, ctr->CurrentEvent);
                        char *messageId = GenerateId(ctr->currentRound, endpoint->Id);

                        struct Message *msm = CreateMessage(messageId, endpoint->Id, ctr->currentRound, endpoint, DATA, data);
                        if(msm == NULL){
                            // errp
                            DestroiData(data);
                            endWithoutErro = false;
                            break;
                            
                        }else{
                            re = SendMessage(msm);
                            if(!re.State){
                                // tenta novamente?
                            }else
                                printf("[RUN] Message enviada com sucesso %s", msm->Id);
                        }
                        
                        DestroiMessage(msm);
                        break;
                    
                    } case SERVER_FINISH_ROUND: {
                        // manda evento fim
                        // 
                        break;
                    
                    } case PLAYER_ASK_BET_TYPE: {
                        // tipo da aposta
                        player->Bet.type = response.data;
                        AskUserInput("[RUN] Qual o valor da aposta? ", &ctr->IsWaitingInput, PLAYER_ASK_BET_VALUE, MAX_BET_VALUE);
                        break;

                    } case PLAYER_ASK_BET_VALUE: {
                        // valor da aposta
                        player->Bet.value = response.data;
                        
                        // gera mensagem
                        // manda bastão 
                        struct Data *data = GeneratePlayerData(_temporaryMessage->Data, player);
                        struct Message *msm = UpdateMessage(_temporaryMessage, endpoint, DATA, data);
                        if(msm == NULL){

                        }else {
                            _temporaryMessage->Data = NULL;

                            re = SendMessage(msm);
                            if(!re.State){
                                // tenta nov? 
                            }else
                                printf("[RUN] Message envivada com sucesso! %s", msm->Id);

                            DestroiMessage(msm);
                        }

                        // destroi o q ta no endereço de temporary, ou seja, message original
                        DestroiMessage(_temporaryMessage);
                        _temporaryMessage = NULL;
                        
                        break;

                    } default:
                        break;
                }
            }
        }
    }

    // clear data
    if (player) DestroiPlayer(player);
    if (server) DestroiServer (server);
    if (ctr) DestroiEventController (ctr);
    if (_temporaryMessage) DestroiMessage (_temporaryMessage);

    // Restaurar configuração original do stdin
    if (RestoreInput() < 0) {
        printf("[RUN_ERROR] Não foi possível restaurar stdin\n");
        endWithoutErro = false;
    }

    StopAllSocketConnection();

    return endWithoutErro;
}



void StopEventController (struct Endpoint *endpoint) {

}