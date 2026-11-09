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
#include "../../entities/game/data/Data.h"
#include "../../entities/network/transfer/Transfer.h"


enum Event GetNextEvent(enum Event currentEvent){
    // contador circular que reinicia após os 4 valores possíveis
    return (enum Event)(((int)currentEvent + 1) % 4);
}

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

    int connected_socket = GetServerSocket();
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
        printf("[RUN_ERRO]: Não foi possível configurar entradas do usuário \n");
        
        StopAllSocketConnection();
        DestroiEventController(ctr);

        return false;
    }
    
    if(endpoint->IsServer){
        printf("[RUN] Parabéns! você será server nessa jogada! \n");
        // pergunta só uma vez antes de iniciar o poll para começar de fato a jogada
        // depois pergunta novamente só se tiver transferencia de player->servidor
        AskUserInput("[Entry] Quantas rodadas deseja realizar?", &ctr->IsWaitingInput, SERVER_ASK_ROUNDS, MAX_ROUNDS_PER_USER);
    
    }else 
        printf("[RUN] Parabéns! você será player nessa jogada! \n");

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
            
            printf("[RUN_ERRO] Poll error \n");
            endWithoutErro = false;
            break;
        }

        // [EV]: Mensagem recebida
        if(fds[0].revents & POLLIN){
            while (1) {
                printf("[RUN]: MESSAGE RECEIVED! \n");
                // aqui já cria uma nova mensagem
                // com dados de destino e origem correspondente a essa maquina
                re = ReceiveMessage();
                if(re.State && re.Code == CODE_ANY_DATA){
                    // fila ta vazia
                    printf("[RUN]: Empty Queue \n");
                    break;
                }
                
                if(!re.State){
                    printf("[RUN_ERRO] %s \n", re.Message);
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
                
                LogMessage(message);

                // bastão e é para mim
                if(message->Type == DATA && message->DestinationId == endpoint->Id){
                    // sou o dono e se sou server
                    if(message->OwnerId == endpoint->Id && endpoint->IsServer){
                        printf("[RUN]: SERVER - SOU DONO \n");
                        // sou server, cria uma vez no começo
                        if(!server) server = CreatePlayerServer(endpoint->Id);

                        // gero as respostas do servidor
                        // aqui não preciso fazer tratamento de evento porque em todas servidor gera dado
                        // o tratamento é interno na função
                        struct Data *data = GenerateServerData(message->Data, server);
                        if(!data){
                            printf("[RUN_ERROR]: Server - ERror creating data \n");
                            continue;
                            // verificar tratamento aqui
                        }
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
                            newMessageId = GenerateMessageId(ctr->currentRound, endpoint->Id);
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

                            newMessageId = GenerateMessageId(ctr->currentRound, endpoint->EndpointNext);
                            messageOwner = endpoint->EndpointNext;

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

                        int retry = 0;

                        printf("[RUN]: Server - Data generated: \n");
                        LoggerData(data);

                        // nenhuma transferencia por enquanto, então dest e or = aos da mensagem
                        struct Message *newMessage = CreateMessage(newMessageId,messageOwner,retry, endpoint, endpoint->EndpointNext, endpoint->Id, DATA, data, data->Event);
                        if(!newMessage){
                            // trato err
                            printf("[RUN_ERROR]: Error creting data \n");
                            continue;

                        }else{
                            printf("[RUN]: Serve - Message generated: \n");
                            LogMessage(newMessage);

                            message->Data = NULL;

                            // envio
                            re = SendMessage(newMessage);
                            if(!re.State){
                                // mensagem não pode ser enviada
                                // fazer uma outra tentativa
                                printf("[RUN_ERROR]: %s \n", re.Message);
                            }else
                                printf("[RUN] Message enviada: %s \n", message->Id);
                            
                            printf("[RUN] Cleaning new message \n");
                            DestroiMessage(newMessage);
                            printf("[RUn] new message cleaned! \n");
                        }
                        
                        printf("[RUN]: Cleaning messge \n");
                        // mensagem processada, posso destruir
                        DestroiMessage(message);
                        printf("[RUN] Message cleaned! \n");
                    }
                    // nao sou dono da mensagem sou player, não há transição
                    else if(message->OwnerId != endpoint->Id && !endpoint->IsServer){
                        printf("[RUN]: PLAYER - NAO SOU DONO, NAO HA TRANSICAO \n");
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
                                printf("[RUN] PLAYER - INIT \n");
                                // add message de inicio para o jogador
                                // (*) mostrar num menu dados
                                bool result = UpdatePlayerData(message->Data, player);
                                if(!result){
                                    // trata erro
                                }

                                printf("[RUN]: PLAYER - NOVA DATA GENERATED! EVENT INIT \n");
                                LoggerData(message->Data);

                                int retry = 0;
                                struct Message *newMessage = CreateMessage(message->Id, message->OwnerId, retry, endpoint, endpoint->EndpointNext, endpoint->Id, DATA, message->Data, message->Data->Event);
                                if(!newMessage){
                                    printf("[RUN_ERROR]: Error creating message! \n");
                                    break;
                                }else{
                                    printf("[RUN]: PLayer - Message to send: \n");
                                    LogMessage(newMessage);

                                    message->Data = NULL;

                                    re = SendMessage(newMessage);
                                    if(!re.State){
                                        // nova tent?
                                        printf("[RUN_ERROR] %s \n", re.Message); 
                                    }else 
                                        printf("[RUN] Sua mensagem foi enviada! %s \n", newMessage->Id);

                                    printf("[RUN]: Destroy new message! \n");
                                    DestroiMessage(newMessage);
                                    printf("[RUN]: New message cleaned! \n");
                                }
                                
                                printf("[RUN]: Destroy message! \n");
                                // já processada, pode destruir
                                DestroiMessage(message);
                                printf("[RUN]: message cleaned! \n");

                                break;  
                            
                            } case BET : {
                                printf("[RUN] PLAYER - BET \n");
                                // salvo message que irei usar quadno user retornar resposta
                                _temporaryMessage = message;
                                // peço dados
                                AskUserInput("[ENTRY] Qual o tipo de aposta", &ctr->IsWaitingInput, PLAYER_ASK_BET_TYPE, MAX_BET_TYPE);
                                // mando mensagem no controle de respostas
                                break;

                            } case RESULT: {
                                printf("[RUN] PLAYER - RESULT \n");
                                // resultado não gera data novo, só atualiza dados do player
                                bool result = UpdatePlayerData(message->Data, player);
                                if(!result){
                                    // mostra erro
                                    // retorna?
                                }

                                printf("[RUN]: Player - DAta generated: \n");
                                LoggerData(message->Data);

                                int retry = 0;
                                struct Message *newMessage = CreateMessage(message->Id, message->OwnerId, retry, endpoint, endpoint->EndpointNext, endpoint->Id, DATA, message->Data, message->Data->Event);
                                if(!newMessage){
                                    // tratar
                                    printf("[RUN_ERROR]: Error creating message! \n");
                                    break;
                                }else{                                    
                                    printf("[RUN] Player - MEssage to send: \n");
                                    LogMessage(newMessage);

                                    message->Data = NULL;

                                    re = SendMessage(newMessage);
                                    if(!re.State){
                                        // nova tent? 
                                        printf("[RUN_ERROR] %s \n", re.Message);
                                    }else 
                                        printf("[RUN] Sua mensagem foi enviada! %s", newMessage->Id);

                                    printf("[RUN]: Cleaning new message! \n");
                                    DestroiMessage(newMessage);
                                    printf("[RUN]: new message cleaned! \n");
                                }
                                
                                printf("[RUN]: Cleaning message! \n");
                                // já processada, pode destruir
                                DestroiMessage(message);
                                printf("[RUN]: message cleaned! \n");
                                break;  

                            } case END: {
                                printf("[RUN] PLAYER - END \n");
                                int retry = 0;
                                // só notifica o usuário que terminou
                                struct Message *newMessage = CreateMessage(message->Id, message->OwnerId, retry, endpoint, endpoint->EndpointNext, endpoint->Id, DATA, message->Data, message->Data->Event);
                                if(!newMessage){
                                    // tratar com retry?? (!)
                                    printf("[RUN_ERROR]: Creating new message! \n");
                                    break;
                                }else{
                                    printf("[RUN]: Player - Message to send \n");
                                    LogMessage(newMessage);

                                    message->Data = NULL;

                                    re = SendMessage(newMessage);
                                    if(!re.State){
                                        // nova ten?
                                        printf("[RUN_ERROR] %s \n", re.Message);
                                    }else
                                        printf("[RUN] Sua mensagem foi enviada! %s", newMessage->Id);

                                    printf("[RUN] Cleaning new message! \n");
                                    DestroiMessage(newMessage);
                                    printf("[RUN] new message cleaned! \n");
                                }
                                
                                printf("[RUN] Cleaning message! \n");
                                DestroiMessage(message); //(*) tenho q ver
                                printf("[RUN] message cleaned! \n");
                                break;
                            }
                        }
                    }
                    // preciso ser o novo dono, há transição
                    else if(message->OwnerId == endpoint->Id && !endpoint->IsServer && message->Data->Event == INIT) {
                        printf("[RUN] Player, preciso ser o novo dono \n");
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
                else if(message->Type == ACK && message->DestinationId == endpoint->Id){

                }
                // bastão que nao é para mim, porém vou retransmittir
                else if(message->Type == DATA && message->DestinationId != endpoint->Id && message->Transfer->DestinationEndpointId == endpoint->Id){

                } 
                // ack não é para mim, porém vou retransmittir
                else if(message->Type == ACK && message->DestinationId != endpoint->Id && message->Transfer->DestinationEndpointId == endpoint->Id){

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
                        printf("[CURRENT] SERVER_ACK_ROUNDS \n");
                        // cria mensagem 
                        // manda bastão
                        // aqui sempre é primeiro bastão desse servidor
                        // no caso de transferencias, o bastão já vem com o round e evento setado

                        ctr->MaxRounds = response.data;

                        printf("[RUN]: Gerando seu data \n");
                        struct Data *data = CreateData(ctr->currentRound, ctr->MaxRounds, ctr->CurrentEvent);
                        if(!data){
                            printf("[RUN_ERRO] Erro ao criar data");
                            endWithoutErro = false;
                        }

                        // debbug
                        LoggerData(data);

                        printf("[RUN]: Gerando sua mensagem! \n");
                        char *messageId = GenerateMessageId(ctr->currentRound, endpoint->Id);
                        if(!messageId){
                            endWithoutErro = true;
                            break;
                        }

                        int retry = 0;
                        struct Message *msm = CreateMessage(messageId, endpoint->Id, retry, endpoint, endpoint->EndpointNext, endpoint->Id, DATA, data, data->Event);
                        if(!msm){
                            // errp
                            printf("[RUN_ERROR]: Creating message! %s", messageId);
                            DestroiData(data);
                            break;
                            
                        }else{
                            LogMessage(msm);

                            re = SendMessage(msm);
                            if(!re.State){
                                printf("[RUN_ERROR]: %s \n", re.Message);
                                // tenta novamente?
                            }else
                                printf("[RUN] Message enviada com sucesso %s\n", msm->Id);
                        }
                        
                        printf("[RUN]: Destroy sended message. \n");
                        DestroiMessage(msm);

                        printf("[RUN]: Message cleaned \n");
                        break;
                    
                    } case SERVER_FINISH_ROUND: {
                        // manda evento fim
                        // 
                        break;
                    
                    } case PLAYER_ASK_BET_TYPE: {
                        printf("[RUN]: PLAYER ASK BET TYPE \n");
                        // tipo da aposta
                        player->Bet.type = response.data;
                        AskUserInput("[RUN] Qual o valor da aposta? ", &ctr->IsWaitingInput, PLAYER_ASK_BET_VALUE, MAX_BET_VALUE);
                        break;

                    } case PLAYER_ASK_BET_VALUE: {
                        printf("[RUN]: PLAYER ASK BET VALUE \n");
                        // valor da aposta
                        player->Bet.value = response.data;
                        
                        // gera mensagem
                        // manda bastão 
                        int retry = 0;
                        struct Data *data = GeneratePlayerData(_temporaryMessage->Data, player);
                        if(!data){
                            printf("[RUN_ERROR]: Error generating data! \n");
                            break;
                        }

                        printf("[RUN]: Player - DAta generated! \n");
                        LoggerData(data);

                        struct Message *msm = CreateMessage(_temporaryMessage->Id, _temporaryMessage->OwnerId, retry, endpoint, endpoint->EndpointNext, endpoint->Id, DATA, data, data->Event);
                        if(!msm){
                            printf("[RUN_ERROR]: Error creating message! \n");
                            break;
                        }else {
                            printf("[RUN]: Player - Message generated: \n");
                            LogMessage(msm);


                            _temporaryMessage->Data = NULL;

                            re = SendMessage(msm);
                            if(!re.State){
                                printf("[RUN_ERROR]: %s \n", re.Message);
                            }else
                                printf("[RUN] Message envivada com sucesso! %s", msm->Id);

                            printf("[RUN] Cleaning message \n");
                            DestroiMessage(msm);
                            printf("[RUN] Message cleaned \n");
                        }

                        printf("[RUN] CLeaning temporary message \n");
                        // destroi o q ta no endereço de temporary, ou seja, message original
                        DestroiMessage(_temporaryMessage);
                        _temporaryMessage = NULL;
                        printf("[RUN]: Temporary cleaned! \n");
                        
                        break;

                    } default:
                        break;
                }
            }
        }
    }

    printf("[RUN]: Cleanign all data \n");
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

void DestroiEventController (struct EventController *ctr) {
    if(!ctr) return;
    free(ctr);
}

void StopEventController (struct Endpoint *endpoint) {
    if(!endpoint) return;
    free(endpoint);
}