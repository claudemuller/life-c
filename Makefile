CC = gcc
DBG_BIN = lldb
CFLAGS = #-D_GNU_SOURCE
CFLAGS += -std=c99
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -Werror
CFLAGS += -Wmissing-declarations

INCS = $(shell pkg-config --cflags raylib)
LIBS = $(shell pkg-config --libs raylib)
LIBS += -Llib

SRC_FILES = ./src/*.c
BIN_DIR = ./bin
BIN = $(BIN_DIR)/life
TEST_DIR = ./tests
TEST_SRC = $(filter-out ./src/main.c, $(wildcard ./src/*.c)) $(TEST_DIR)/*.c

build:
	$(CC) $(CFLAGS) $(INCS) $(LIBS) -o $(BIN) $(SRC_FILES)

debug: debug-build
	$(DBG_BIN) $(BIN) $(ARGS)

debug-build:
	$(CC) $(CFLAGS) $(INCS) $(LIBS) -g -o $(BIN) $(SRC_FILES)

run: build
	@$(BIN) $(ARGS)

test:
	$(CC) $(CFLAGS) $(INCS) $(LIBS) -o $(TEST_DIR)/tests $(TEST_SRC) && $(TEST_DIR)/tests

test-debug:
	$(CC) $(CFLAGS) $(INCS) $(LIBS) -g -o $(TEST_DIR)/tests $(TEST_SRC) && lldb $(TEST_DIR)/tests $(ARGS)

clean:
	rm -rf $(BIN_DIR)/* $(TEST_DIR)/tests*

gen-compilation-db:
	bear -- make build

gen-compilation-db-make:
	make --always-make --dry-run \
	| grep -wE 'gcc|g\+\+' \
	| grep -w '\-c' \
	| jq -nR '[inputs|{directory:".", command:., file: match(" [^ ]+$").string[1:]}]' \
	> compile_commands.json
