CC = gcc
CFLAGS = -Wall -g

SRC := src
OBJ := obj
BIN := bin
EXE := $(shell basename $(CURDIR))

# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

SOURCEFILES := $(call rwildcard, $(SRC)/,*.c)
HEADERFILES := $(call rwildcard, $(SRC)/,*.h)
OBJECTFILES := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCEFILES))

all: $(BIN)/$(EXE).exe

$(BIN)/$(EXE).exe: $(OBJECTFILES) $(HEADERFILES)
	mkdir -p $(@D);
	$(CC) $^ -o $@

$(OBJ)/%.o : $(SRC)/%.c
	mkdir -p $(@D);
	$(CC) -I$(SRC) -c $^ -o $@

clean:
	rm -r $(OBJ)
	rm -r $(BIN)