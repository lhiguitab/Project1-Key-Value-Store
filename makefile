# Compiler
CC = gcc

# Source files
SRC =  main.c models.c csv_loader.c top_calculations.c

# Executable name
EXEC = dataload

FOLDER_NAME = folder

# Compilation flags (-Wall enables warnings)
CFLAGS = -std=c11 -O3 -Wall 

# Default rule: compile the project
$(EXEC):
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)

# Run the program
run: $(EXEC)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)
	./$(EXEC) -f $(FOLDER_NAME)

# Remove the executable
clean:
	rm -f $(EXEC)

# Indicate that these are not files
.PHONY: clean run
