#include "Mmu.h"
#include <cassert>

Mmu::Mmu(Nes* nes) :
    nes_(nes)
{

}

// void Mmu::Init(Nes* nes) {
//    nes_ = nes;
// }

void Mmu::AddMemoryMap(IMemoryUnit* unit, uint16_t addr_start, uint16_t addr_end) {
    assert(addr_start < MEMORY_MAP_SIZE);
    for (int i = addr_start; i < addr_end; i++) {
        _mem_units[i] = unit;
    }
}

uint8_t Mmu::Read8(uint16_t addr) {
    uint8_t data;

    if (addr < 0x2000) {
        data = _ram[addr & 0x1FFF];
    } else {
        data = _mem_units[addr]->Read8(addr);
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
        _ram[addr & 0x1FFF] = data;
    } else {
        _mem_units[addr]->Write8(addr, data);
    }
}

uint16_t Mmu::Read16(uint16_t addr) {
    return (Read8(addr + 1) << 8) | Read8(addr);
}

void Mmu::Write16(uint16_t addr, uint16_t data) {
    Write8(addr, data & 0xFF);
    Write8(addr + 1, data >> 8);
}
