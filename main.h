#pragma once
#include "rom.h"
#include "cpu.h"
#include "ram.h"

typedef struct NES {
        CPU *cpu;
        //PPU *ppu;
        //APU *apu;
        uint8_t *ram;
        ROM *rom;
} NES;
