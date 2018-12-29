#pragma once

#include <stdint.h>
#include <stdbool.h>

#define CPU_CLOCK 21441960

typedef struct CPU {
    uint8_t acc_reg;    // accumulator register
    uint8_t x_reg;      // x tiling register
    uint8_t y_reg;      // y tiling register
    uint8_t status_reg; // status register [NEGATIVE | OVERFLOW | | BRK COMMAND | DECIMAL MODE (NOT USED) | IRQ DISABLE | ZERO | CARRY]
    uint16_t stack_p;   // stack pointer
    uint16_t program_c; // program counter
} CPU;

typedef enum STATUS_REG_BIT {
    CARRY = 0,
    ZERO = 1,
    IRQ_DISABLE = 2,
    BRK = 4,
    OVERFLOW = 6,
    NEGATIVE = 7
} STATUS_REG_BIT;

void set_cpu_status_bit(CPU *cpu, STATUS_REG_BIT bit_position, bool value);
bool get_cpu_status_bit(CPU *cpu, STATUS_REG_BIT bit_position);
bool get_bit(uint8_t byte, unsigned pos);
