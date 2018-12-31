#include "cpu.h"

// initialization of cpu upon NES boot
CPU *new_CPU() {
    CPU *cpu = (CPU*) calloc(1, sizeof(CPU)); // zero-initialize values
    cpu->stack_p = 0xfd; // initialize descending, empty stack
    cpu->status_reg = 0x0034;

    return cpu;
}

inline void set_cpu_status_bit(CPU *cpu, STATUS_REG_BIT bit_position, bool value) {
    if (value == 0)
        cpu->status_reg &= ~(1 << bit_position);
    else
        cpu->status_reg |= (1 << bit_position);
}

inline bool get_cpu_status_bit(CPU *cpu, STATUS_REG_BIT bit_position) {
    return (cpu->status_reg & (1 << bit_position)) >> bit_position;
}

inline bool get_bit(uint8_t byte, unsigned pos) {
    return byte & (1 << pos);
}

void stack_push(NES *nes, uint8_t value) {
    nes->ram[nes->cpu->stack_p--] = value;
}

void stack_push16(NES *nes, uint16_t value) {
    stack_push(nes, (uint8_t) value); // truncate for least significant byte
    stack_push(nes, (uint8_t) (value >> 8)); // push most significant byte
}

uint8_t stack_pull(NES *nes) { // pull = pop in 6502 lingo
    return nes->ram[++nes->cpu->stack_p];
}

uint16_t stack_pull16(NES *nes) {
    uint8_t pull0 = stack_pull(nes);
    uint8_t pull1 = stack_pull(nes);
    uint16_t result = (pull0 << 8) | pull1;
    return result;
}

uint8_t negate_byte(uint8_t byte) {
    return (~byte) + 1;
}
