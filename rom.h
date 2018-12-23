#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define PRG_LEN_BYTE_LOC 4
#define PRG_BLOCK_SIZE 16384
#define PRG_BLOCK_BEGIN_LOC 16
#define CHR_LEN_BYTE_LOC 5
#define CHR_BLOCK_SIZE 8192

typedef struct Rom {
    char *path;
    uint8_t *prg;
    uint8_t *chr;
} Rom;

bool parse_rom(FILE *rom, uint8_t **prg, uint8_t **chr);
void close_rom(Rom *rom);
