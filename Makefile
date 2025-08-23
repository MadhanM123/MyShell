CC := gcc
CFLAGS := -std=c11 -O2 -Wall -Wextra -Wpedantic

SRC := src/myshell.c
OBJ := myshell.o 
TARGET := myshell

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
