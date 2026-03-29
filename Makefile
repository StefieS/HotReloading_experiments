# Compiler and flags
CC = gcc
CFLAGS = -Wall -fPIC
LDFLAGS = -ldl

# Directories
BUILD_DIR = build

# Files
PLUGIN_SRC = plugin.c
PLUGIN_SO  = $(BUILD_DIR)/plugin.so
PLUGIN_TEMP = $(BUILD_DIR)/plugin_temp.so
MAIN_SRC   = main.c
MAIN_EXE   = main

.PHONY: all clean run plugin

# Default: build everything
all: $(BUILD_DIR) $(PLUGIN_SO) $(MAIN_EXE)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build plugin shared library inside build/
$(PLUGIN_SO): $(PLUGIN_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -shared -o $@ $<

# Build main executable in root
$(MAIN_EXE): $(MAIN_SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Clean everything
clean:
	rm -rf $(BUILD_DIR) $(MAIN_EXE)

# Run main program
run: all
	./$(MAIN_EXE)

# Rebuild plugin only
plugin: $(PLUGIN_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -shared -o $(PLUGIN_SO) $(PLUGIN_SRC)