#include "nes.h"
#include <stdlib.h>
#include "ram.h"

NES *new_NES() {
    NES *nes = (NES*) calloc(1, sizeof(NES));
    nes->cpu = (CPU*) calloc(1, sizeof(CPU));
    nes->ram = (uint8_t*) calloc(NES_RAM_SIZE, sizeof(uint8_t));
    nes->rom = (ROM*) calloc(1, sizeof(ROM));

    return nes;
}

void delete_nes(NES *nes) {
    free(nes->cpu);
    free(nes->ram);
    close_rom(nes->rom);
    free(nes);
}
