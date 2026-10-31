#ifndef __BET__
#define __BET__

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