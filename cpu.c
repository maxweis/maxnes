#include "cpu.h"
#include "instruction.h"

void set_cpu_status_bit(CPU &cpu, STATUS_REG_BIT bit_position, bool value) {
    if (value == 0)
        cpu.status_reg &= ~(1 << bit_position);
    else
        cpu.status_reg |= (1 << bit_position);
}

int get_cpu_status_bit(CPU &cpu, STATUS_REG_BIT bit_position) {
    return (cpu.status_reg & (1 << bit_position));
}
