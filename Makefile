CC = gcc
CFLAGS = -Wall -g

SRC := src
INC := include
OBJ := obj
BIN := bin
EXE := $(shell basename $(CURDIR)).exe
DLL := lib$(shell basename $(CURDIR)).dll

INCLUDES := -I$(SRC)
LINKS :=
DEBUGFLAGS := -DDEBUG
RELEASEFLAGS := -O3
# -Og

ifeq ($(shell uname -s),) # not in a bash-like shell
	CLEANUP = del /F /Q
	MKDIR = if not exist "$(@D)" mkdir
else # in a bash-like shell, like msys
	CLEANUP = rm -r -f
	MKDIR = mkdir -p
endif

# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

TESTS := tests
TESTFILE := Tests
TESTEXE := $(BIN)/test_$(EXE)
TEST_SRC := $(call rwildcard, $(TESTS)/,*.c)
TEST_OBJS := $(patsubst $(TESTS)/%.c, $(TESTS)/$(OBJ)/%.o, $(TEST_SRC))

SOURCEFILES := $(call rwildcard, $(SRC)/,*.c)
HEADERFILES := $(call rwildcard, $(SRC)/,*.h)
INCLUDEFILES := $(call rwildcard, $(INC)/,*.h)
OBJECTFILES := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCEFILES))

all: $(BIN)/$(DLL)

exe: $(BIN)/$(EXE)

.PRECIOUS: obj/%.o

$(BIN)/$(DLL): $(OBJECTFILES) $(HEADERFILES) $(INCLUDEFILES)
	$(MKDIR) $(@D);
	$(CC) -shared $(CFLAGS) $(INCLUDES) $(LINKS) $(DEBUGFLAGS) $^ -o $@

$(BIN)/$(EXE): $(OBJECTFILES) $(HEADERFILES) $(INCLUDEFILES)
	$(MKDIR) $(@D);
	$(CC) $(CFLAGS) $(INCLUDES) $(LINKS) $(DEBUGFLAGS) $^ -o $@

$(OBJ)/%.o : $(SRC)/%.c
	$(MKDIR) $(@D);
	$(CC) $(CFLAGS) $(INCLUDES) $(LINKS) $(DEBUGFLAGS) -c $^ -o $@

test : $(TESTEXE)

$(TESTEXE): $(TESTS)/$(TESTFILE).o $(OBJECTFILES) $(HEADERFILES) $(INCLUDEFILES)
	$(MKDIR) $(BIN);
	$(CC) $(CFLAGS) $(INCLUDES) -I3rdParty\acutest\include $(LINKS) $(DEBUGFLAGS) $(TESTS)/$(TESTFILE).o $(filter-out %main.o,$(OBJECTFILES)) -o $(TESTEXE)

$(TESTS)/$(TESTFILE).o : $(OBJECTFILES) $(TEST_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -I3rdParty\acutest\include $(LINKS) $(DEBUGFLAGS) -c $(TESTS)\$(TESTFILE).c -o $(TESTS)/$(TESTFILE).o

clean:
	$(CLEANUP) $(OBJ) $(BIN) $(TESTS)/$(BIN) $(TESTS)/$(OBJ) $(TESTS)/$(TESTFILE).o