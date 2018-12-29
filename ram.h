#pragma once
#include <stdint.h>
#include "main.h"

typedef struct NES NES;

typedef enum ADDR_MODE {
    IMMEDIATE,
    ABSOLUTE,
    ZERO_PAGE,
    ACCUMULATOR,
    IMPLIED,
    INDIRECT_X,
    INDIRECT_Y,
    ZERO_PAGE_X,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    RELATIVE,
    INDIRECT,
    ZERO_PAGE_Y
} ADDR_MODE;

uint8_t ram_read(NES *nes, uint16_t addr);
