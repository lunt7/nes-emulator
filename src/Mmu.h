#ifndef _MEMORY_H
#define _MEMORY_H

#include <cstdint>
#include "IMemoryUnit.h"

#define MEMORY_MAP_SIZE 0x10000
#define RAM_SIZE        0x800

class Nes;

class Mmu : public IMemoryUnit {
public:
    Mmu(Nes* nes);
    ~Mmu() = default;

    void AddMemoryMap(IMemoryUnit* unit, uint16_t addr_start, uint16_t addr_end);
    virtual uint8_t Read8(uint16_t addr);
    virtual void Write8(uint16_t addr, uint8_t data);
    uint16_t Read16(uint16_t addr);
    void Write16(uint16_t addr, uint16_t data);
    uint16_t Read16_S(uint16_t addr);

private:
    Nes* nes_;
    IMemoryUnit* mem_units_[MEMORY_MAP_SIZE];
    uint8_t ram_[RAM_SIZE];
};

#endif
