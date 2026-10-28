#ifndef __SOCKETSERVICE__
#define __SOCKETSERVICE__

#include "../config/networkDefs.h"
#include "../entities/utils/Response.h"
#include "../entities/network/network/Network.h"
#include "../entities/network/message/Message.h"

struct Response CreatingServer(struct Network *network);

struct Response SendMessage(struct Message *message);

struct Response ReceiveMessage();

void StopAllSocketConnection();

#endif