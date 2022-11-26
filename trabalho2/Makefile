CC          := gcc
DEBUG_FLAGS := -g3 -pedantic -Werror=implicit-function-declaration -fsanitize=address,undefined
FLAGS       := -Wall -lm -Werror -g #$(DEBUG_FLAGS)

SRC         := ./src
BIN         := ./bin
BINNAME     := bin
HEADERS     := ./headers
FILES 	 	:= $(wildcard ./files/*.bin)
EXEC        := prog.exe
ZIP         := exercicio.zip

# List of all expected $(BIN)/*.o
ALL         := $(wildcard $(SRC)/*.c)
ALL         := $(ALL:$(SRC)/%.c=$(BIN)/%.o)


all: $(EXEC)

compile: clean reset all

crun : compile run

run: all
	./$(EXEC)

clean:
	rm -f $(EXEC) $(BIN)/*.o

zip:
	rm -f $(ZIP)
	zip $(ZIP) Makefile $(HEADERS)/*.h $(SRC)/*.c ./*.bin ./*.csv

reset:
	rm -f $(ZIP) $(EXEC) ./*.bin
	cp $(FILES) ./

$(EXEC): $(ALL)
	$(CC) -o $(EXEC) $(BIN)/*.o $(FLAGS)

$(BIN)/%.o: $(SRC)/%.c $(BIN)
	$(CC) -o $@ -c $< -I $(HEADERS) $(FLAGS)

$(BINNAME):
	mkdir $(BINNAME)