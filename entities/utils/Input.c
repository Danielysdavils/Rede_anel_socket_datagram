#include "./Input.h"

static char _inputBuffer[BUFFER_USER_INPUT];
static size_t _inputLength = 0;

static int flags = -1; 
static bool _discartingLine = false;

static enum ControlInputEvents _currentEvent = ANY_EVENT;
// cada pergunta pode ter um numero máximo diferente
static int _currentInputMaxLimit = 1;

int ConfigureInput(void) {
    flags = fcntl(STDIN_FILENO, F_GETFL);
    if(flags == -1) return -1;

    return fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

struct InputResponse CreateInputResponse(int data, enum ControlInputEvents event) {
    struct InputResponse res;
    res.data = data;
    res.event = event;
    return res;
}

// como as entradas vão ser inputs numericos
// parametro para limite máx de entrada
struct InputResponse HandleUserInput(bool *control) {
    struct InputResponse response = CreateInputResponse(0, _currentEvent);
    char chunk[MAX_CHUNKS];
    long data;

    ssize_t count = read(STDIN_FILENO, chunk, sizeof(chunk));
    if(count < 0){
        // nao há dados no momento
        if (errno == EAGAIN || errno == EWOULDBLOCK){
            return response;
        }
            
        perror("handle user input \n");
        response.data = -1;
        return response;
    }   

    if(count == 0){
        // entrada encerrada
        response.data = -1;
        return response;
    }

    for(ssize_t i = 0; i < count; i++){
        char charactere = chunk[i];

        // chegou ao final da string
        if(charactere == '\n'){
            // zera antes da conversao
            if(_discartingLine){
                _discartingLine = false; 
                _inputLength = 0;
                continue;
            }

            _inputBuffer[_inputLength] = '\0';
            char *end;
            errno = 0;

            // converte string a int base 10
            data = strtol(_inputBuffer, &end, 10);

            // numero inteiro na janela 0 até maxLimit
            if(errno == ERANGE || end == _inputBuffer
                || *end != '\0' || data <= 0 || data > _currentInputMaxLimit){
                _inputLength = 0;

                printf("[ENTRY_ERRO] Entrada inválida. Digite valor de 1 a %d \n", _currentInputMaxLimit);
                fflush(stdout);
                continue;
                
            } else {
                *control = false;
                _inputLength = 0;
                _currentEvent = ANY_EVENT;
                
                printf("[ENTRY]: Processando sua entrada: %d \n", (int)data); 
                
                response.data = (int)data;
                return response;
            }

        }else {
            // vai preenchendo o buffer a cada it
            if((_inputLength < sizeof(_inputBuffer) - 1) && !_discartingLine){
                _inputBuffer[_inputLength++] = charactere;

            }else{
                // primeira chamada
                if(!_discartingLine)
                    printf("[ENTRY] Entrada muito longa! Tente novamente. \n");

                _inputLength = 0;
                _discartingLine = true;
            }
        }
    }

    // resposta indefinida
    return response;
}


void AskUserInput(char *message, bool *control, enum ControlInputEvents inputEvent, int inputMaxLimit) {
    if(*control){
        printf("[ENTRY] Já existe uma pergunta atva. aguarde finalizar");
        return;
    }

    _inputLength = 0;
    _discartingLine = false;

    *control = true;
    _currentEvent = inputEvent;
    _currentInputMaxLimit = inputMaxLimit;

    // da para usar writte tmb... verificar
    printf("[ENTRY] %s \n", message);
    fflush(stdout);
}
