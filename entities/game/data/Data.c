#include "Data.h"

/**
 * Exclusivo para Decode de mensagens
*/
struct Data *CreateCompleteData (int round, int maxRound, enum Event event, struct BetDto bets[TOTAL_ENDPOINTS], struct BetDto rewards[TOTAL_ENDPOINTS]) {
    if(!bets || !rewards) return NULL;
    
    struct Data *data = calloc(1, sizeof(struct Data));
    if(!data){
        printf("[DATA] Error in calloc data");
        return NULL;
    }

    data->Event = event;
    data->Round = round;
    data->MaxRound = maxRound;

    memcpy(data->Bets, bets, sizeof(data->Bets));
    memcpy(data->Reward, rewards, sizeof(data->Reward));

    return data;
}

/**
 * Exlusivo para 1ra mensagem (bastao) do servidor 
*/
struct Data *CreateData (int round, int maxRound, enum Event event){
    struct Data *data = calloc(1, sizeof(struct Data));
    if(!data){
        printf("[DATA] Error in calloc data");
        return NULL;
    }

    data->Event = event;
    data->Round = round;
    data->MaxRound = maxRound;

    // só precisa inicializar os players pq os valores são alterados nas funções
    // generate server/player data
    for(int i = 0; i < TOTAL_ENDPOINTS; i++){
        data->Bets[i].PlayerId = i;
        data->Reward[i].PlayerId = i;     
    }

    return data;
}

void LoggerData(struct Data *data){
    printf("[DATA]: Data Round %d \n", data->Round);
    printf("[DATA]: Data MaxRound %d \n", data->MaxRound);
    printf("[DATA]: Data Event %d \n", data->Event);
    for(int i = 0; i < TOTAL_ENDPOINTS; i++){
        printf("[DATA]: Data Bets [%d] = {playerID: %d, BetType: %d, BetValue: %d}\n", i, data->Bets[i].PlayerId, data->Bets[i].Bet.type, data->Bets[i].Bet.value);
    }
    for(int i = 0; i < TOTAL_ENDPOINTS; i++){
        printf("[DATA]: Data Rewards [%d] = {playerID: %d, BetType: %d, BetValue: %d}\n", i, data->Reward[i].PlayerId, data->Reward[i].Bet.type, data->Reward[i].Bet.value);
    }
}

void DestroiData (struct Data *data){
    if(!data) return; 
    free(data);
}