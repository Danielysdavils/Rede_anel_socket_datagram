CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11

TARGET = main

# Encontra todos os arquivos .c, exceto os testes
SRCS = $(shell find entities services -name '*.c' ! -name 'teste.c') main.c

# Gera os nomes dos arquivos objeto
OBJS = $(SRCS:.c=.o)

# Compilação principal
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compila cada arquivo .c individualmente
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Remove os arquivos gerados
clean:
	rm -f $(OBJS) $(TARGET)

# Limpa e recompila tudo
rebuild: clean all

.PHONY: all clean rebuild
