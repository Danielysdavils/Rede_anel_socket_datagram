#ifndef __DATA__
#define __DATA__

#include "../bet/Bet.h"
#include "../../../config/networkDefs.h";

enum Event{
    INIT,
    BET,
    RESULT,
    END
};

struct BetDto {
    int PlayerId; // mesmo do endPointID
    struct Bet Bet;
};

struct Data
{
    /* data */
    // rodada: cada rodada é 4 eventos
    int Round;
    // rodadas máximas, é o usuário servidor que escolhe a cada nova jogada
    // importante todas as maquinas souberem a qtd de jogadas que irão fazer
    int MaxRound;
    // eventos
    enum Event Event;

    // array de player de apostas {idPlayer, valor aposta}
    // no caso do player que esteja sendo server valor aposta = 0
    struct BetDto Bets[TOTAL_ENDPOINTS];

    // array de player de premios {idPlayer, valor premio}
    // aqueles que perderam tem premio = 0
    struct BetDto Reward[TOTAL_ENDPOINTS];
};

// preencher bets automaticamente com valor 0 a partir dos ids das maquinas no networkDefs
struct Data *CreateData (int round, int maxRound, enum Event event);

void DestroiData (struct Data *data);

#endif