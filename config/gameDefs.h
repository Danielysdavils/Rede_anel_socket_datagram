#ifndef __GAMEDEFS__
#define __GAMEDEFS__

// numero máx de rodadas que um usuário pode executar por vez
#define MAX_ROUNDS_PER_USER 10

#define INITIAL_VALUE 12000 // Quantidade de fichas iniciais

#define MAX_TIPS 36

/**
 * Control input Events:
 *  -> Quando um server ou player pergunta é complexo saber a que evento essa resposta pertence
 *  -> pois temos só um controlador de respostas, então, para resolver isso, 
 *  -> ao criar uma pergunta já identifica a pergunta com um evento e ao retornar retorna o evento junto
 *  -> como temos só uma pergunta ativa por vez com uma unica flag funciona 
*/   
enum ControlInputEvents{
    ANY_EVENT,
    // [SERVER_EVENTS]
    // se o servidor quer finalizar a jogada
    SERVER_FINISH_ROUND,
    // quantas rodadas o servidor irá fazer
    SERVER_ASK_ROUNDS,

    // [PLAYER_EVENTS]
    // qual o valor da aposta
    PLAYER_ASK_BET_TOKENS,
    // qual o tipo da aposta
    PLAYER_ASK_BET_TYPE,
    // qual o numero escolhido (caso aposta NUMBER)
    PLAYER_ASK_BET_TIP
};

#endif