#include "rom.h"
#include "instruction.h"

bool parse_rom(FILE *rom_file, ROM *rom) {
    if (rom_file == NULL) {
        return false;
    }

    unsigned prg_length = 0;
    fseek(rom_file, PRG_LEN_BYTE_LOC, SEEK_SET); // access prg length from rom header
    fread(&prg_length, 1, 1, rom_file);
    prg_length *= PRG_BLOCK_SIZE;

    unsigned chr_length = 0;
    fseek(rom_file, CHR_LEN_BYTE_LOC, SEEK_SET); // access chr length from rom header
    fread(&chr_length, 1, 1, rom_file);
    chr_length *= CHR_BLOCK_SIZE;

    rom->prg = (uint8_t*) calloc(prg_length + 1, sizeof(uint8_t));
    rom->chr = (uint8_t*) calloc(chr_length + 1, sizeof(uint8_t));

    fseek(rom_file, PRG_BLOCK_BEGIN_LOC, SEEK_SET);
    for (unsigned i = 0; i < prg_length; i++) {
        if (!fread(rom->prg + i, 1, 1, rom_file)) { // fill in string with binary prg data
            return false;
        }
    }
    rom->prg[prg_length] = '\0';  // null-terminate data

    for (unsigned i = 0; i < chr_length; i++) {
        if (!fread(rom->chr + i, 1, 1, rom_file)) { // fill in string with binary chr data
            return false;
        }
    }
    rom->chr[chr_length] = '\0'; // null-terminate data

    rom->prg_len = prg_length;
    rom->chr_len = chr_length;

    return true;
}

void close_rom(ROM *rom) {
    free(rom->prg);
    free(rom->chr);
    for (unsigned i = 0; i < rom->inst_amount; i++) {
        free(rom->prg_inst[i].body);
    }
    free(rom->prg_inst);
    free(rom);
}
