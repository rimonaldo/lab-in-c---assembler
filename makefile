CC = gcc
CFLAGS = -std=c90 -Wall -Wextra -pedantic -g
LDFLAGS =
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
OUT = $(BIN_DIR)/am

# Find all .c files under src/
SRC = $(shell find $(SRC_DIR) -name '*.c')

# Corresponding .o files in build/, preserving folder structure
OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

all: $(OUT)

$(OUT): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Compile each .c file to .o file inside build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	
run: all
	./$(OUT) input.as