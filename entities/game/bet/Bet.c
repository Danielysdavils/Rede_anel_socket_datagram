#include "Bet.h"

const char *GetBetType(enum BetType type){
    switch (type)
    {
        case NONE: return "Nenhum";
        case NUMBER: return "Numero";
        case BLACK: return "Preto";
        case RED: return "Vermelho";
        case EVEN: return "Par";
        case ODD: return "Ímpar";
        default: return "Nenhum";
    }
}