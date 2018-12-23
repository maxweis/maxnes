#include "rom.h"

bool parse_rom(FILE *rom, uint8_t **prg, uint8_t **chr) {
    if (rom == NULL) {
        return false;
    }

    unsigned prg_length = 0;
    fseek(rom, PRG_LEN_BYTE_LOC, SEEK_SET); // access prg length from rom header
    fread(&prg_length, 1, 1, rom);
    prg_length *= PRG_BLOCK_SIZE;

    unsigned chr_length = 0;
    fseek(rom, CHR_LEN_BYTE_LOC, SEEK_SET); // access chr length from rom header
    fread(&chr_length, 1, 1, rom);
    chr_length *= CHR_BLOCK_SIZE;

    *prg = (uint8_t*) malloc(prg_length * sizeof(uint8_t) + 1);
    *chr = (uint8_t*) malloc(chr_length * sizeof(uint8_t) + 1);

    fseek(rom, PRG_BLOCK_BEGIN_LOC, SEEK_SET);
    for (unsigned i = 0; i < prg_length; i++) {
        if (!fread(*prg + i, 1, 1, rom)) { // fill in string with binary prg data
            return false;
        }
    }
    (*prg)[prg_length] = '\0';  // null-terminate data

    for (unsigned i = 0; i < chr_length; i++) {
        if (!fread(*chr + i, 1, 1, rom)) { // fill in string with binary chr data
            return false;
        }
    }
    (*chr)[chr_length] = '\0'; // null-terminate data

    return true;
}

void close_rom(Rom *rom) {
    free(rom->prg);
    free(rom->chr);
}
