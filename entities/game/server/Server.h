#ifndef __SERVER__
#define __SERVER__

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "../data/Data.h"
#include "../../../config/gameDefs.h"

struct PlayerServer{
    int ServerId; // mesmo do endpoint
    int LocalRound; // contador de rodadas locais
    int totalReward; // Saldo roleta
};

struct PlayerServer *CreatePlayerServer (int serverID);

// Servidor realiza as apostas
struct Data *GenerateServerData (struct Data *data, struct PlayerServer *server); 

void DestroiServer (struct PlayerServer *server);

#endif