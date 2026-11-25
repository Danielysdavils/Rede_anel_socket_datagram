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

const char *GetBetResultColor(enum ResultColor color){
    switch (color)
    {
        case RESULT_BLACK: return "Preto";
        case RESULT_RED: return "Vermelho";
        case RESULT_WHITE: return "Branco";
        default: return "Nenhum";
    }
}

const char *GetBetResultPariety(enum ResultPariety pariety){
    switch (pariety)
    {
        case RESULT_EVEN: return "Par";
        case RESULT_ODD: return "Impar";
        default: return "Nenhum";
    }
}