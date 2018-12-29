#pragma once

#include <stdint.h>
#include "nes.h"

#define NES_RAM_SIZE 2048
#define ZERO_PAGE_SIZE 256 

typedef struct NES NES;

typedef enum ADDR_MODE {
    IMPLIED,
    ACCUMULATOR,
    IMMEDIATE,
    ZERO_PAGE,
    ZERO_PAGE_X,
    ZERO_PAGE_Y,
    RELATIVE,
    ABSOLUTE,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    INDIRECT,
    INDIRECT_X,
    INDIRECT_Y
} ADDR_MODE;

uint8_t ram_read(uint8_t *ram, uint16_t addr);
