# Makefile for Snake Game

# Compiler options
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lncurses

# Target executable name
TARGET = snake

# Source files
SRC = snake.c

# Object files
OBJ = $(SRC:.c=.o)

# Default target
all: $(TARGET)

# Compile the game
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Compile C source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJ) $(TARGET)

# Run the game
run: $(TARGET)
	./$(TARGET)

# Help information
help:
	@echo "Makefile for Snake Game"
	@echo "Targets:"
	@echo "  all    - Build the game (default)"
	@echo "  clean  - Remove object files and executable"
	@echo "  run    - Build and run the game"
	@echo "  help   - Display this help information"

.PHONY: all clean run help 