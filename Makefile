CC=gcc
OUTPUT=maxnes

FILES=main.c rom.c instruction.c cpu.c ram.c nes.c

all:
	$(CC) $(FILES) -o $(OUTPUT)
