#ifndef __PLAYER__
#define __PLAYER__

#include "../data/Data.h"

struct Player{
    int playerId; // mesmo do endpoint
    struct Bet Bet;
};

// retorna resposta do player dependendo do evento
// (*) Maria adicionar as demais props e mudar o nome se quiser
// porém o retorno deve contér o Data
struct Data *GeneratePlayerData (struct Data *data, struct Player *player);

struct Player *CreatePlayer(int playerId);

void DestroiPlayer(struct Player *player);

#endif
