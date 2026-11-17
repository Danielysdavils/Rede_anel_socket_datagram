#ifndef __BET__
#define __BET__

#define MAX_BET_TYPE 6
#define MIN_BET_VALUE 10
#define MAX_BET_VALUE 100

// Enum de opções de aposta 
enum BetType {
    NONE,
    NUMBER,
    BLACK,
    RED,
    EVEN,
    ODD
};

struct Bet {
    enum BetType type; // tipo de aposta
    int betTip; // Numero da aposta
    int tokens; // Fichas apostadas
};

#endif