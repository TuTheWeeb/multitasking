SRC = src
BUILD = build
OBJ = objs

# 1. Grab ONLY .c files, ignoring .h files
SRC_FILES = $(wildcard $(SRC)/*.c)

# 2. Safely translate src/filename.c to objs/filename.o
OBJ_FILES = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRC_FILES))

CC = gcc
CFLAGS = -Wall -Wextra

TARGET = $(BUILD)/main

# Default target
all: $(TARGET)

# 3. Link all individual .o files into the final executable
$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@

# 4. Pattern rule: Compile EACH .c file into its own .o file separately
$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

# Standard cleanup rule
clean:
	rm -rf $(OBJ) $(BUILD)
