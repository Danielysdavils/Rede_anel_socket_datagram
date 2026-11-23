#ifndef __PLAYER__
#define __PLAYER__

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "../data/Data.h"
#include "../../../config/gameDefs.h"

struct Player{
    int playerId; // mesmo do endpoint
    bool isRoulette; // Flag indicadora de servidor
    struct Bet Bet;
    int totalReward;
};

struct Player *CreatePlayer (int playerId);

// Retorna o data com a aposta do jogador
struct Data *GeneratePlayerData (struct Data *data, struct Player *player);

// Jogador recebe os premios
bool UpdatePlayerData (struct Data *data, struct Player *player);

void DestroiPlayer (struct Player *player);

#endif
