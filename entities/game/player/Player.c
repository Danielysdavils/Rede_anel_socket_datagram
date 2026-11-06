#include <stdio.h>
#include <stdlib.h>

#include "Player.h"

Player *CreatePlayer(int playerId){
    Player *p = (Player *) malloc (sizeof(struct Player));
    if (!p){
        return NULL;
    }

    p->id = playerId;
    p->is_roulette = false; /*VERIFICAR ONDE ALTERAR*/ 
    p->tokens = INITIAL_TOKENS;
    p->type = NoBet;
    p->tip = -1;
    p->betValue = 0;
    p->reward = 0;

    return p;
}

void DestroyPlayer(Player *playerCurrent){
    if (playerCurrent != NULL){
        free(playerCurrent);
    }
}