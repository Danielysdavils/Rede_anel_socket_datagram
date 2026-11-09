#include "Player.h"
#include "../../../config/env.h"

/**
 * Só para teste (!) pode apagar tudo
*/

struct Data *GeneratePlayerData (struct Data *data, struct Player *player){
    if(!data || !player) return NULL;

    switch (data->Event)
    {
        case BET: {
            for(int i = 0; i < TOTAL_ENDPOINTS; i++){
                if(i == LOCAL_ID && data->Bets[i].PlayerId == LOCAL_ID){
                    data->Bets[i].Bet.type = 1;
                    data->Bets[i].Bet.value = 100;
                }
            }
            break;
        }
        default:
            break;
    }

    return data;
}

bool UpdatePlayerData (struct Data *data, struct Player *player){
    if(!data || !player) return false;

    switch (data->Event)
    {
        case INIT: {
            for(int i = 0; i < TOTAL_ENDPOINTS; i++){
                if(i == LOCAL_ID && data->Reward[i].PlayerId == LOCAL_ID){
                    player->totalReward += data->Reward[i].Bet.value;
                    data->Reward[i].Bet.value = 0; 
                }
            }
            break;
        }
        case RESULT: {
            for(int i = 0; i < TOTAL_ENDPOINTS; i++){
                if(i == LOCAL_ID && data->Bets[i].PlayerId == LOCAL_ID){
                    if(data->Bets[i].Bet.type == player->Bet.type)
                        player->totalReward += data->Reward[i].Bet.value;
                    
                    data->Reward[i].Bet.value = 0; 
                }
            }
            break;
        }
        default:
            break;
    }

    return true;
}

struct Player *CreatePlayer(int playerId){
    struct Player *player = calloc(1, sizeof(struct Player));
    if (!player) {
        printf("[PLAYER_ERROR]: Error in creating player. Creating malloc");
        return NULL;
    }

    player->playerId = playerId;
    player->totalReward = 0;
    player->Bet.type = 1;

    return player;
}

void DestroiPlayer(struct Player *player){
    if(!player) return NULL;
    free(player);
}