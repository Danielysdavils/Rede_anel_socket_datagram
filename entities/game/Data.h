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
    // eventos
    enum Event Event;
};


#endif