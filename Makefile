CC=clang
OUTPUT=maxnes

FILES=main.c rom.c instruction.c cpu.c ram.c

all:
	$(CC) $(FILES) -o $(OUTPUT)
