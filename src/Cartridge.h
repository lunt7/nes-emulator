#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <cstdint>
#include <vector>
#include "IMemoryUnit.h"

class Nes;
class Mmu;

// #define CARTRIDGE_ADDR_START    0x4020
// #define CARTRIDGE_ADDR_END      0xFFFF

typedef struct {
    uint8_t magic[4];   // 0x4E 0x45 0x53 0x1A ("NES" followed by DOS EOF)
    uint8_t prg_size;   // PRG ROM size, in 16 KB units
    uint8_t chr_size;   // CHR ROM size, in 8 KB units (value 0 means the board uses CHR RAM)
    uint8_t flag6;      // mapper, mirroring, battery, trainer
    uint8_t flag7;      // mapper, VS/Playchoice, NES 2.0
    uint8_t flag8;      // PRG RAM size
    uint8_t flag9;      // TV system
    uint8_t flag10;     // TV-system, PRG RAM presence
    uint8_t padding[5]; // unused padding
} ines_hdr;

class Cartridge : public IMemoryUnit {
public:
    Cartridge(Nes* nes, Mmu* mmu);
    ~Cartridge() = default;

    virtual uint8_t Read8(uint16_t addr);
    virtual void Write8(uint16_t addr, uint8_t data);

    void LoadRom(const char* rom);
    //uint16_t GetResetVector(void);

private:
    Nes* nes_;
    Mmu* mmu_;
    std::vector<uint8_t> prg_rom_;
    std::vector<uint8_t> chr_rom_;
};

#endif
