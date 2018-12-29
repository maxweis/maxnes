#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "instruction.h"
#include "nes.h"

int main(int argc, char *argv[]) {
    char *path = "mario.nes";
    FILE *rom_file = fopen(path, "rb");

    if (rom_file == NULL) {
        fprintf(stderr, "Error: unable to open file\n");
        return -1;
    }

    NES *nes = new_NES();

    if (!parse_rom(rom_file, nes->rom)) {
        fprintf(stderr, "Error: rom unrecognized format\n");
        return -1;
    } else {
        parse_insts(nes->rom);
    }

    if (rom_file != NULL) {
        fclose(rom_file);
    }

    delete_nes(nes);
    return 0;
}
