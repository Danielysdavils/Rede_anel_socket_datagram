#include <stdio.h>
#include <stdlib.h>

#include "Server.h"

PlayerServer *CreatePlayerServer (int serverID){
    PlayerServer *ps = (PlayerServer *) malloc (sizeof(PlayerServer));

    if (!ps){
        return NULL;
    }

    ps->serverID = serverId;
    ps->LocalRound = 1;

    return ps;
}

// importante retornar o mesmo objeto do parametro alterado !!!!
Data *GenerateServerData (Data *data, PlayerServer *server); 

bool UpdateServerData (Data *data, PlayerServer *server);

void DestroyPlayerServer (PlayerServer *server){
    if (server != NULL){
        free(server);
    }    
}
