CC = gcc
CFLAGS = -Wall -g
INCLUDES :=
LINKS :=
DEBUGFLAGS := -Og -DDEBUG
RELEASEFLAGS := -O3

SRC := src
INC := include
OBJ := obj
BIN := bin
EXE := $(shell basename $(CURDIR)).exe
DLL := lib$(shell basename $(CURDIR)).dll

# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

SOURCEFILES := $(call rwildcard, $(SRC)/,*.c)
HEADERFILES := $(call rwildcard, $(SRC)/,*.h)
INCLUDEFILES := $(call rwildcard, $(INC)/,*.h)
OBJECTFILES := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCEFILES))

all: $(BIN)/$(DLL)

exe: $(BIN)/$(EXE)

$(BIN)/%: $(OBJECTFILES) $(HEADERFILES) $(INCLUDEFILES)
	mkdir -p $(@D);
	$(CC) $(CFLAGS) $(INCLUDES) $(LINKS) $(DEBUGFLAGS) $^ -o $@

$(OBJ)/%.o : $(SRC)/%.c
	mkdir -p $(@D);
	$(CC) $(CFLAGS) -I$(SRC) $(INCLUDES) $(LINKS) $(DEBUGFLAGS) -c $^ -o $@

clean:
	rm -r $(OBJ)
	rm -r $(BIN)