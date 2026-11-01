#ifndef __SERVER__
#define __SERVER__

#include "../data/Data.h"

struct PlayerServer{
    int ServerId; // mesmo do endpoint
    
    // contador de rodadas locais
    int LocalRound; // inicializar em 1 SEMPRE
};

struct PlayerServer *CreatePlayerServer (int serverID);

// importante retornar o mesmo objeto do parametro alterado !!!!
struct Data *GenerateServerData (struct Data *data, struct PlayerServer *server); 

bool UpdateServerData (struct Data *data, struct PlayerServer *server);

void DestroiServer (struct PlayerServer *server);

#endif