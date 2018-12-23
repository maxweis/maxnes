CC=clang
OUTPUT=maxnes

FILES=main.c rom.c

all:
	$(CC) $(FILES) -o $(OUTPUT)
