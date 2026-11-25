#include "Server.h"
#include "../../../config/env.h"


struct PlayerServer *CreatePlayerServer (int serverID){
    struct PlayerServer *server = calloc(1, sizeof(struct PlayerServer));
    if (!server){
        printf("[PLAYER_ERROR]: Error in creating server. Creating malloc");
        return NULL;
    }

    server->LocalRound = 1;
    server->ServerId = serverID;
    server->totalReward = INITIAL_VALUE;

    return server;
}


struct Data *GenerateServerData (struct Data *data, struct PlayerServer *server){
    if(!data || !server){
      return NULL;  
    } 

    switch (data->Event){
        case INIT: {
            // Limpeza do Rewards
            if(ACTIVE_LOGS) printf("[SERVER] INIT EVENT START\n");
            for(int i = 0; i < TOTAL_ENDPOINTS; i++){
                data->Reward[i].Bet.betTip = 0;
                data->Reward[i].Bet.tokens = 0;
            }
            break;
        }
        case BET: {
            if(ACTIVE_LOGS) printf("[SERVER] BET EVENT START\n");
            // Valores das apostas são guardados no servidor
            for(int i = 0; i < TOTAL_ENDPOINTS; i++){
                if(data->Bets[i].PlayerId != server->ServerId){
                    server->totalReward += data->Bets[i].Bet.tokens; 
                }
            }

            // Apostas são realizadas
            int numberDraw = (rand() % (MAX_TIPS + 1)); // [0 ate 36]
            data->Result.Tip = numberDraw;

            if(ACTIVE_LOGS) printf("O RESULTADO DA APOSTA: \n");

            if(numberDraw == 0){
                if(ACTIVE_LOGS) printf("NUMERO: 0 - ZERO - BRANCO\n");
                data->Result.Color = RESULT_WHITE;
            } else if(numberDraw % 2 == 0){
                if(ACTIVE_LOGS) printf("NUMERO: %d - PAR - VERMELHO\n", numberDraw);
                data->Result.Pariety = RESULT_EVEN;
                data->Result.Color = RESULT_RED;
            } else {
                if(ACTIVE_LOGS) printf("NUMERO: %d - IMPAR - PRETO\n", numberDraw);
                data->Result.Pariety = RESULT_ODD;
                data->Result.Color = RESULT_BLACK;
            }

            for(int i = 0; i < TOTAL_ENDPOINTS; i++){
                if(data->Bets[i].PlayerId != server->ServerId){
                    int reward = 0;

                    switch (data->Bets[i].Bet.type){
                        case NONE: 
                            break;

                        case NUMBER:
                            if(numberDraw == data->Bets[i].Bet.betTip){
                                reward = data->Bets[i].Bet.tokens * 36; 
                            }
                            break;
                        case EVEN: 
                            if (numberDraw % 2 == 0 && numberDraw != 0){
                                reward = data->Bets[i].Bet.tokens * 2;
                            }
                            break;

                        case ODD:
                            if (numberDraw % 2 != 0 && numberDraw != 0){
                                reward = data->Bets[i].Bet.tokens * 2;
                            }
                            break;

                        case RED:
                            if (numberDraw % 2 == 0 && numberDraw != 0){
                                reward = data->Bets[i].Bet.tokens * 2;
                            }
                            break;

                        case BLACK: 
                            if (numberDraw % 2 != 0 && numberDraw != 0){
                                reward = data->Bets[i].Bet.tokens * 2;
                            }
                            break;
                    }
                    
                    data->Reward[i].PlayerId = data->Bets[i].PlayerId;
                    server->totalReward -= reward;
                    data->Reward[i].Bet.type = data->Bets[i].Bet.type;
                    data->Reward[i].Bet.tokens = reward;
                }
            }

            break;
        }
        case RESULT: {
            if(ACTIVE_LOGS) printf("[SERVER] RESULT EVENT START\n");
            break;
        }
        case END: {
            // Limpeza das apostas
            if(ACTIVE_LOGS) printf("[SERVER] END EVENT START\n");
            for(int i = 0; i < TOTAL_ENDPOINTS; i++){
                data->Bets[i].Bet.betTip = 0;
                data->Bets[i].Bet.tokens = 0;
                data->Bets[i].Bet.type = 0;

                data->Reward[i].Bet.betTip = 0;
                data->Reward[i].Bet.tokens = 0;
                data->Reward[i].Bet.type = 0;
            }

            data->Result.Color = 0;
            data->Result.Pariety = 0;
            data->Result.Tip = 0;
            
            break;
        }
        default:
            break;
    }
    return data;
} 


void DestroiServer (struct PlayerServer *server){
    if(!server){
      return;
    } 
    free(server);
}
