CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -pthread

# Directories
SRC_DIR = src
OBJ_DIR = obj

# Files
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/threadpool.c $(SRC_DIR)/http.c
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = server

# Build Rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean