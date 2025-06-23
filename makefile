CC = gcc
CFLAGS = -std=c90 -Wall -Wextra -pedantic -g
LDFLAGS =

SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
OUT := $(BIN_DIR)/am

# Find all .c files recursively under src/
SRC := $(shell find $(SRC_DIR) -name '*.c')

# Map .c → .o under build/ (preserving paths)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

# Default target
all: $(OUT)

# Link final executable
$(OUT): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Compile each .c file to corresponding .o file in build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean intermediate and output files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run default input file
run: all
	./$(OUT) input/prog1.as
