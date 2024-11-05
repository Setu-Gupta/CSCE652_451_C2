CC = g++
FLAGS = -m32 -std=c++23
CC_FLAGS = -Ofast -flto=auto -static -s
DBG_FLAGS = -Og -ggdb3 -fsanitize=address,leak,undefined
ERR_FLAGS = -Wall -Wextra -Wconversion -Wpedantic -Werror
LD_FLAGS = -lpthread -lutil -ldl -lm ./libssl.a ./libcrypto.a
INC_DIR = ./inc
SRC_DIR = ./src
OBJ_DIR = ./build
DBG_OBJ_DIR = ./build_dbg
BIN_DIR = ./bin
DBG_BIN_DIR = ./bin_dbg

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(SRCS)))
DBG_OBJS = $(patsubst %.cpp,$(DBG_OBJ_DIR)/%.o,$(notdir $(SRCS)))
TGTS = $(patsubst %.cpp,$(BIN_DIR)/%,$(notdir $(SRCS)))
DBG_TGTS = $(patsubst %.cpp,$(DBG_BIN_DIR)/%,$(notdir $(SRCS)))

.PHONY: all
all: $(TGTS)
$(BIN_DIR)/%: $(OBJ_DIR)/%.o
	$(CC) $(FLAGS) $(CC_FLAGS) $(ERR_FLAGS) $< $(LD_FLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -I$(INC_DIR) $(FLAGS) $(CC_FLAGS) $(ERR_FLAGS) -c $< -o $@

.PHONY: debug
debug: $(DBG_TGTS)
$(DBG_BIN_DIR)/%: $(DBG_OBJ_DIR)/%.o
	$(CC) $(FLAGS) $(DBG_FLAGS) $(ERR_FLAGS) $< $(LD_FLAGS) -o $@

$(DBG_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -I$(INC_DIR) $(FLAGS) $(DBG_FLAGS) $(ERR_FLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(DBG_TGTS) $(TGTS) $(DBG_OBJS) $(OBJS)

.PHONY: format
format:
	clang-format -i inc/* src/*

.PHONY: zip-student
zip-student:
	rm -f ../student.zip
	zip -u ../student.zip bin/decrypt
	zip -u ../student.zip bin/decrypt_key
	zip -u ../student.zip bin/image_fingerprint
	zip -u ../student.zip bin/main
	zip -u ../student.zip bin/time
	zip -ur ../student.zip data/images
	zip -u  ../student.zip data/secret
	zip -u  ../student.zip data/key

.PHONY: zip-ta
zip-ta:
	rm -f ../ta.zip
	zip -r ../ta.zip bin data src inc build sample.txt Gen_Secret.py Makefile README.md libssl.a libcrypto.a
