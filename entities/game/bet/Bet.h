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

enum ResultColor {
    RESULT_BLACK,
    RESULT_RED,
    RESULT_WHITE
};

enum ResultPariety {
    RESULT_EVEN,
    RESULT_ODD
};

struct BetResult {
    int Tip; // até 36
    enum ResultColor Color;
    enum ResultPariety Pariety;
};

struct Bet {
    enum BetType type; // tipo de aposta
    int betTip; // Numero da aposta
    int tokens; // Fichas apostadas
};

const char *GetBetType(enum BetType type);

const char *GetBetResultColor(enum ResultColor color);

const char *GetBetResultPariety(enum ResultPariety pariety);

#endif