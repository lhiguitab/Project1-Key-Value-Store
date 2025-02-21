# Compiler
CC = gcc

# Source files
SRC = main.c hash_table.c

# Object files
OBJ = $(SRC:.c=.o)

# Executable name
EXEC = main

# Compilation flags (-Wall enables warnings)
CFLAGS = -std=c11 -O2 -Wall

# Default rule: compile the project
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: $(EXEC)
	./$(EXEC)

# Remove object files and the executable
clean:
	rm -f $(OBJ) $(EXEC)

# Indicate that these are not files
.PHONY: clean run rm
