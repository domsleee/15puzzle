#ifndef
ifdef ENV_DEBUG
	CONDITIONAL_CXX = -g
else
	CONDITIONAL_CXX = -O2
endif

CXX = g++-11
CXXFLAGS = -std=c++17 -Wall $(CONDITIONAL_CXX) -m64

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -m64 #-L/opt/homebrew/Cellar/gperftools/2.9.1_1/lib -lprofiler

$(BIN_DIR)/puzzle: $(OBJ_FILES)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(ENV_CXXFLAGS) -c -o $@ $<

clean:
	- rm -r $(OBJ_DIR)/* $(BIN_DIR)/*
