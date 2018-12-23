#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "rom.h"

int main(int argc, char *argv[]) {
    char *path = "mario.nes";
    FILE *rom_file = fopen(path, "rb");

    if (rom_file == NULL) {
        fprintf(stderr, "Error: unable to open file\n");
        return -1;
    }

    Rom rom;

    if (!parse_rom(rom_file, &rom.prg, &rom.chr)) {
        fprintf(stderr, "Error: rom unrecognized format\n");
        return -1;
    }

    if (rom_file != NULL) {
        fclose(rom_file);
    }

    close_rom(&rom);

    return 0;
}
