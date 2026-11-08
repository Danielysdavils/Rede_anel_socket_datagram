#include "Data.h"

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

void DestroiData (struct Data *data){
    if(!data) return; 
    if(data) free(data);
}