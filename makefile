# Compiler
CC = gcc

# Source files
SRC = main.c

# Executable name
EXEC = main

# Compilation flags (-Wall enables warnings)
CFLAGS = -std=c11 -O3 -Wall

# Default rule: compile the project
$(EXEC):
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)

# Run the program
run: $(EXEC)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)
	./$(EXEC)

# Remove the executable
clean:
	rm -f $(EXEC)

# Indicate that these are not files
.PHONY: clean run
