#ifndef __ENDPOINT__
#define __ENDPOINT__

enum Event{
    INIT,
    BET,
    RESULT,
    END
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

};


#endif