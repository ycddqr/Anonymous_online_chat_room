CC=gcc
STD=-std=gnu99
FLAG=-Wall -Werror
OBJ=chat_server.o network.o queue.o threadpool.o
BIN=server
LIB=-pthread

all:$(OBJ)
	$(CC) $(OBJ) $(LIB) -o $(BIN) && ./$(BIN)

%.o:%.c
	$(CC) $(STD) $(FLAG) -c $< -o $@

clean:
	rm -rf $(OBJ) $(BIN)
	rm -rf *.h.gch *~
