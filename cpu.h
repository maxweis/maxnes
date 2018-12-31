#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "nes.h"
#include "instruction.h"

#define CPU_CLOCK 21441960

typedef struct NES NES;

typedef struct CPU {
    uint8_t acc_reg;    // accumulator register
    uint8_t x_reg;      // x tiling register
    uint8_t y_reg;      // y tiling register
    uint8_t status_reg; // status register [NEGATIVE | OVERFLOW | | BRK COMMAND | DECIMAL MODE (NOT USED) | IRQ DISABLE | ZERO | CARRY]
    uint8_t stack_p;   // stack pointer
    uint16_t program_c; // program counter
} CPU;

typedef enum STATUS_REG_BIT {
    CARRY = 0,
    ZERO = 1,
    IRQ_DISABLE = 2,
    DECIMAL = 3,
    BRK = 4,
    OVERFLOW = 5,
    NEGATIVE = 6
} STATUS_REG_BIT;

CPU *new_CPU();
void set_cpu_status_bit(CPU *cpu, STATUS_REG_BIT bit_position, bool value);
bool get_cpu_status_bit(CPU *cpu, STATUS_REG_BIT bit_position);
bool get_bit(uint8_t byte, unsigned pos);
void stack_push(NES *nes, uint8_t value);
void stack_push16(NES *nes, uint16_t value);
uint8_t stack_pull(NES *nes);
uint16_t stack_pull16(NES *nes);
uint8_t negate_byte(uint8_t byte);
