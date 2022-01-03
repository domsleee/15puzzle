#ifndef
ifdef ENV_DEBUG
	CONDITIONAL_CXX = -g
else
	CONDITIONAL_CXX = -O2
endif


UNAME := $(shell uname)
CXX = g++-11
CXXFLAGS = -std=c++17 -Wall $(CONDITIONAL_CXX)
LIBS := -ltbb -lprocps
ifeq ($(UNAME), Darwin)
LIBS := #-ltcmalloc -L/opt/homebrew/Cellar/gperftools/2.9.1_1/lib
endif

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := $(LIBS)

$(BIN_DIR)/puzzle: $(OBJ_FILES)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(ENV_CXXFLAGS) -c -o $@ $< $(LIBS)

clean:
	- rm -r $(OBJ_DIR)/* $(BIN_DIR)/*
