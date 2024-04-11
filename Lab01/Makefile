CC = gcc
CFLAGS = -Wall -Wextra
BUILD_DIR = build
CLIENT_SRC_DIR = src/client
SERVER_SRC_DIR = src/server
EXTERNAL_SRC_DIR = src/external_files
MUSIC_SRC_DIR = src/music
UTILS_SRC_DIR = src/utils

all: $(BUILD_DIR)/client/client $(BUILD_DIR)/server/server

$(BUILD_DIR)/client/client: $(CLIENT_SRC_DIR)/client.c $(CLIENT_SRC_DIR)/client_operations.c $(BUILD_DIR)/external_files/config_handler.o $(BUILD_DIR)/external_files/storage_handler.o $(BUILD_DIR)/music/music.o $(BUILD_DIR)/utils/utils.o | $(BUILD_DIR)/client
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/server/server: $(SERVER_SRC_DIR)/server.c $(SERVER_SRC_DIR)/server_operations.c $(BUILD_DIR)/external_files/storage_handler.o $(BUILD_DIR)/external_files/config_handler.o $(BUILD_DIR)/music/music.o $(BUILD_DIR)/utils/utils.o | $(BUILD_DIR)/server
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/external_files/config_handler.o: $(EXTERNAL_SRC_DIR)/config_handler.c $(EXTERNAL_SRC_DIR)/config_handler.h | $(BUILD_DIR)/external_files
	$(CC) $(CFLAGS) -c -o $@ $(EXTERNAL_SRC_DIR)/config_handler.c

$(BUILD_DIR)/external_files/storage_handler.o: $(EXTERNAL_SRC_DIR)/storage_handler.c $(EXTERNAL_SRC_DIR)/storage_handler.h $(MUSIC_SRC_DIR)/music.h | $(BUILD_DIR)/external_files $(BUILD_DIR)/music
	$(CC) $(CFLAGS) -c -o $@ $(EXTERNAL_SRC_DIR)/storage_handler.c

$(BUILD_DIR)/music/music.o: $(MUSIC_SRC_DIR)/music.c $(MUSIC_SRC_DIR)/music.h | $(BUILD_DIR)/music
	$(CC) $(CFLAGS) -c -o $@ $(MUSIC_SRC_DIR)/music.c

$(BUILD_DIR)/utils/utils.o: $(UTILS_SRC_DIR)/utils.c $(UTILS_SRC_DIR)/utils.h | $(BUILD_DIR)/utils
	$(CC) $(CFLAGS) -c -o $@ $(UTILS_SRC_DIR)/utils.c

$(BUILD_DIR)/client:
	mkdir -p $(BUILD_DIR)/client

$(BUILD_DIR)/server:
	mkdir -p $(BUILD_DIR)/server

$(BUILD_DIR)/external_files:
	mkdir -p $(BUILD_DIR)/external_files

$(BUILD_DIR)/music:
	mkdir -p $(BUILD_DIR)/music

$(BUILD_DIR)/utils:
	mkdir -p $(BUILD_DIR)/utils

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
