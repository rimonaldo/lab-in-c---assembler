CC = gcc

# Directories
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
OUT := $(BIN_DIR)/am

# Source and object files
SRC := $(shell find $(SRC_DIR) -name '*.c')
OBJ := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

# Build flags
BASE_CFLAGS = -std=c90 -Wall -Wextra -pedantic -g
BASE_LDFLAGS =

ifeq ($(SANITIZE),1)
	CFLAGS = $(BASE_CFLAGS) -fsanitize=address
	LDFLAGS = $(BASE_LDFLAGS) -fsanitize=address
else
	CFLAGS = $(BASE_CFLAGS)
	LDFLAGS = $(BASE_LDFLAGS)
endif

# Default target (with sanitizer)
all: $(OUT)

# Link final executable
$(OUT): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Compile each source file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) output

# Run the assembler on all or specific programs
run: all
	@mkdir -p output
ifeq ($(PROG),)
	@echo "🔁 Running on all programs in input/* ..."
	@find input -type f -name '*.as' | while read f; do \
		echo "🔧 Processing $$f..."; \
		./$(OUT) "$$f"; \
	done
else
	@echo "▶️  Running only on input/$(PROG) ..."
	@find input/$(PROG) -type f -name '*.as' | while read f; do \
		echo "🔧 Processing $$f..."; \
		./$(OUT) "$$f"; \
	done
endif

# Debug mode (no sanitizer)
debug:
	@echo "🛠 Building in debug mode (no sanitizers)..."
	$(MAKE) clean
	$(MAKE) SANITIZE=0
	@echo "✅ Debug binary ready: $(OUT)"
