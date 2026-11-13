<<<<<<< HEAD
#include "Server.h"
#include "../../../config/env.h"


struct PlayerServer *CreatePlayerServer (int serverID){
    struct PlayerServer *server = calloc(1, sizeof(struct PlayerServer));
    if (!server) {
        printf("[PLAYER_ERROR]: Error in creating server. Creating malloc");
        return NULL;
    }

    server->LocalRound = 1;
    server->ServerId = serverID;
    server->totalReward = 100;

    return server;
}


struct Data *GenerateServerData (struct Data *data, struct PlayerServer *server){
    if(!data || !server) return NULL;

    switch (data->Event)
    {
        case INIT: {
            printf("[SERVER] INIT EVENT START\n");
            for(int i = 0; i < TOTAL_ENDPOINTS; i++){
                if(data->Reward[i].PlayerId != LOCAL_ID){
                    data->Reward[i].Bet.value = 20;
                }
            }
            break;
        }
        case BET: {
            printf("[SERVER] BET EVENT START\n");
            for(int i = 0; i < TOTAL_ENDPOINTS; i++){
                server->totalReward -= 20; 
            }
            break;
        }
        case RESULT: {
            printf("[SERVER] RESULT EVENT START\n");
            for(int i = 0; i < TOTAL_ENDPOINTS; i++){
                if(data->Bets[i].PlayerId != LOCAL_ID){
                    data->Reward[i].Bet.type = 1;
                    data->Reward[i].Bet.value = 10;
                }
            }
            break;
        }
        case END: {
            printf("[SERVER] END EVENT START\n");
            for(int i = 0; i < TOTAL_ENDPOINTS; i++){
                if(data->Bets[i].PlayerId != LOCAL_ID){
                    data->Bets[i].Bet.type = 0;
                    data->Bets[i].Bet.value = 0;

                    data->Reward[i].Bet.type = 0;
                    data->Reward[i].Bet.value = 0;
                }
                if(i == LOCAL_ID){
                    server->totalReward += 100;
                }
            }
            break;
        }
        default:
            break;
    }

    return data;
} 


void DestroiServer (struct PlayerServer *server){
    if(!server) return;
    free(server);
}
