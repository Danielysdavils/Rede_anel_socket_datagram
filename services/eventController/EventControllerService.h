#ifndef __EVENTCONTROLLERSERVICE__
#define __EVENTCONTROLLERSERVICE__

#include <stdbool.h>
#include "../../entities/network/endpoint/Endpoint.h";
#include "../../config/gameDefs.h"
#include "../../entities/game/data/Data.h"

struct EventController {
    // num máx de jogadas para servidor ativo
    int MaxRounds;
    // rodada atual
    int currentRound;
    // evento atual ativo
    enum Event CurrentEvent;
    // flag para saber se aguarda entrada do usuário
    bool IsWaitingInput;
};


// por enquanto só bool. 
// conferir depois se vale a pena mudar para response
bool RunEventController (struct Endpoint *endpoint);

void StopEventController (struct Endpoint *endpoint);

#endif