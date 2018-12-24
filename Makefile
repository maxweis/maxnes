CC=clang
OUTPUT=maxnes

FILES=main.c rom.c instruction.c

all:
	$(CC) $(FILES) -o $(OUTPUT)
