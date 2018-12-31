#pragma once

#include "cpu.h"
#include "rom.h"
#include "ram.h"

typedef struct CPU CPU;
typedef struct ROM ROM;

typedef struct NES {
        CPU *cpu;
        //PPU *ppu;
        //APU *apu;
        uint8_t *ram;
        ROM *rom;
} NES;

NES *new_NES();
void delete_nes(NES *nes);
