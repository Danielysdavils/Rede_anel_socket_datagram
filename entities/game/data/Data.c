#include "Data.h"

// Criação do pacote
struct Data *CreateData (int round, int maxRound, enum Event event){
    struct Data *data = calloc(1, sizeof(struct Data));
    if(!data){
        printf("[DATA] Error in calloc data");
        return NULL;
    }

    data->Event = event;
    data->Round = round;
    data->MaxRound = maxRound;

    // Inicialização dos players
    for(int i = 0; i < TOTAL_ENDPOINTS; i++){
        data->Bets[i].PlayerId = i;
        data->Reward[i].PlayerId = i;     
    }

    return data;
}

/* Exclusivo para Decode de mensagens */
struct Data *CreateCompleteData (int round, int maxRound, enum Event event, struct BetDto bets[TOTAL_ENDPOINTS], struct BetDto rewards[TOTAL_ENDPOINTS]) {
    if(!bets || !rewards){
      return NULL;
    } 
    
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

void LoggerData (struct Data *data){
    if(!data){
        return;
    }
    
    printf("[LOGGER]: Data Round %d \n", data->Round);
    printf("[LOGGER]: Data MaxRound %d \n", data->MaxRound);
    printf("[LOGGER]: Data Event %d \n", data->Event);
    
    /*for(int i = 0; i < TOTAL_ENDPOINTS; i++){
        printf("[LOGGER]: Data Bets [%d] = {playerID: %d, BetType: %d, BetTip: %d, BetTokens: %d}\n", i, data->Bets[i].PlayerId, data->Bets[i].Bet.type, data->Bets[i].Bet.betTip, data->Bets[i].Bet.tokens);
    } */
    
    /*for(int i = 0; i < TOTAL_ENDPOINTS; i++){
        printf("[LOGGER]: Data Rewards [%d] = {playerID: %d, BetType: %d, BetTip: %d, BetTokens: %d}\n", i, data->Reward[i].PlayerId, data->Reward[i].Bet.type, data->Bets[i].Bet.betTip, data->Reward[i].Bet.tokens);
    } */
}

void DestroiData (struct Data *data){
    if(!data){
        return;
    }  
    free(data);
}