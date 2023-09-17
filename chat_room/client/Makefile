CC=gcc
STD=-std=gnu99
FLAG=-Wall -Werror
OBJ=chat_client.o network.o
BIN=client
LIB=-pthread

all:$(OBJ)
	$(CC) $(OBJ) $(LIB) -o $(BIN) && ./$(BIN)

%.o:%.c
	$(CC) $(STD) $(FLAG) -c $< -o $@

clean:
	rm -rf $(OBJ) $(BIN)
	rm -rf *.h.gch *~
