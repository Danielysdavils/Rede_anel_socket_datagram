#ifndef __SERVER__
#define __SERVER__

#include "../data/Data.h"

struct PlayerServer{
    int serverId; // mesmo do endpoint
};

struct PlayerServer *CreatePlayerServer (int serverID);

// importante que o valor retornado seja nova struct, não reutilizar do parametro
struct Data *GenerateServerData (struct Data *data, struct PlayerServer *server); 

void DestroiServer (struct PlayerServer *server);

#endif