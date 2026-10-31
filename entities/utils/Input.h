#ifndef __INPUT__
#define __INPUT__

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <limits.h>

#include "../../config/gameDefs.h"

/**
 * Usado para receber dados do usuário
 *  - Com entradas não bloqueantes (para o while principal)
 *
 * IMPORTANTE: ENTRADAS VÁLIDAS SÓ MAIORES DO QUE 0
 * 0 É UTILIZADO PARA ENTRADAS QUE AINDA NAO TEM ENTRADAS VALIDAS (ESPERA) 
*/

#define BUFFER_USER_INPUT 64

#define MAX_CHUNKS 32

struct InputResponse {
    int data;
    enum ControlInputEvents event;
};

// configura entrada não bloquante
int ConfigureInput(void);

// quando poll deteta resposta do usuario chama essa função
// para desestruturar string e retornar dado
struct InputResponse HandleUserInput(bool *control);

// quando quer iniciar pergunta ao usuário
// passa como parametro uma flag de controle 
// (só para deixar mais generico, porém só o EventController usaria)
void AskUserInput(char *message, bool *control, enum ControlInputEvents inputEvent, int inputMaxLimit);

void DestroyResponse (struct InputResponse *input);

#endif