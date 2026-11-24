#include "Player.h"
#include "../../../config/env.h"

struct Player *CreatePlayer (int playerId){
    struct Player *player = calloc(1, sizeof(struct Player));
    if (!player){
        printf("[PLAYER_ERROR]: Error in creating player. Creating malloc");
        return NULL;
    }

    player->playerId = playerId;
    player->isRoulette = false;
    player->totalReward = INITIAL_VALUE;
    player->Bet.type = 0;
    player->Bet.betTip = 0;
    player->Bet.tokens = 0;

    return player;
}


// Atribui as apostas do jogador 
struct Data *GeneratePlayerData (struct Data *data, struct Player *player){
    if(!data || !player){
      return NULL;
    } 

    switch (data->Event){
        case BET: {
            for(int i = 0; i < TOTAL_ENDPOINTS; i++){
                if(data->Bets[i].PlayerId == player->playerId){
                    data->Bets[i].Bet.type = player->Bet.type;
                    data->Bets[i].Bet.betTip = player->Bet.betTip;
                    data->Bets[i].Bet.tokens = player->Bet.tokens;

                    player->totalReward -= player->Bet.tokens;
                    break;
                }
            }
            break;
        }
        default:
            break;
    }
    return data;
}

// Atualização do jogador após apostas
bool UpdatePlayerData (struct Data *data, struct Player *player){
    if(!data || !player){
      return false;
    } 

    switch (data->Event){
        case RESULT: 
            for(int i = 0; i < TOTAL_ENDPOINTS; i++){
                if(data->Reward[i].PlayerId == player->playerId){
                    player->totalReward += data->Reward[i].Bet.tokens;
                }
                break;
            }
                
        default:
            break;
    }
    return true;
}

void DestroiPlayer (struct Player *player){
    if(!player){ 
      return;
    }
    free(player);
}