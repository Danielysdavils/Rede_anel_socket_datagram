#ifndef __PLAYER__
#define __PLAYER__

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "../data/Data.h"

struct Player{
    int playerId; // mesmo do endpoint
    struct Bet Bet;
    int totalReward;
};

// retorna resposta do player dependendo do evento
// (*) Maria adicionar as demais props e mudar o nome se quiser
// o retorno deve ser o mesmo struct do parametro !!!!
struct Data *GeneratePlayerData (struct Data *data, struct Player *player);

bool UpdatePlayerData (struct Data *data, struct Player *player);

struct Player *CreatePlayer(int playerId);

void DestroiPlayer(struct Player *player);

#endif
