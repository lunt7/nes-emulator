#include "Mmu.h"
#include <cassert>

Mmu::Mmu(Nes* nes) :
    nes_(nes)
{

}

void Mmu::AddMemoryMap(IMemoryUnit* unit, uint16_t addr_start, uint16_t addr_end) {
    assert(addr_start < MEMORY_MAP_SIZE);
    for (int i = addr_start; i < addr_end; i++) {
        mem_units_[i] = unit;
    }
}

uint8_t Mmu::Read8(uint16_t addr) {
    uint8_t data;

    if (addr < 0x2000) {
        data = ram_[addr & 0x1FFF];
    } else {
        data = mem_units_[addr]->Read8(addr);
    }
    // else if (addr < 0x4000) {
    //     // TODO: PPU
    //     addr = 0x2000 + (addr & 7);
    // }
    // else if (addr < 0x4018) {
    //     // TODO: APU/IO
    // }
    // else if (addr < 0x4020) {
    //     // Cpu test mode
    // }
    // else {
    //     _cartr
    // }

    return data;
}

void Mmu::Write8(uint16_t addr, uint8_t data) {
    if (addr < 0x2000) {
        ram_[addr & 0x1FFF] = data;
    } else {
        if (mem_units_[addr] != nullptr) {
            mem_units_[addr]->Write8(addr, data);
        }
    }
}

uint16_t Mmu::Read16(uint16_t addr) {
    return (Read8(addr + 1) << 8) | Read8(addr);
}

void Mmu::Write16(uint16_t addr, uint16_t data) {
    Write8(addr, data & 0xFF);
    Write8(addr + 1, data >> 8);
}

uint16_t Mmu::Read16_S(uint16_t addr) {
    uint16_t addr_l = addr & 0x00FF;
    uint16_t addr_h = addr & 0xFF00;
    uint16_t val_l = Read8(addr);
    uint16_t val_h = Read8(addr_h | ((addr_l + 1) & 0xFF)) ;
    return (val_h << 8) | val_l;
}
