#include "ram.h"

uint8_t ram_read(NES *nes, uint16_t addr) {
    if (addr <= 0x07ff) {
        return nes->ram[addr]; // ram range
    } else if (addr <= 0x0fff) {
        return nes->ram[addr - 0x0800]; // mirrored ram range
    } else if (addr <= 0x17ff) {
        return nes->ram[addr - 0x0800 * 2]; // mirrored ram range
    } else if (addr <= 0x1fff) {
        return nes->ram[addr - 0x0800 * 3]; // mirrored ram range
    } else if (addr <= 0x3fff) {
        // implement ppu memory-mapped registers
    } else if (addr <= 0x4017) {
        // implement apu and i/o registers
    } else if (addr <= 0x401f) {
        // apu, i/o functionality which is normally disabled
    } else if (addr <= 0xffff) {
        // PRG ROM, PRG RAM, mapper registers
    }
        return 0;
}
