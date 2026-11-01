#ifndef __BET__
#define __BET__

#define MAX_BET_TYPE 4
#define MAX_BET_VALUE 100

enum BetType {
    Numero,
    Preto,
    Vermelho,
    Pares
};

struct Bet {
    enum BetType type; // tipo de aposta
    int value; // valor da aposta
};

#endif