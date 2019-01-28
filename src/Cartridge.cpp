#include "Cartridge.h"
#include "Nes.h"
#include <cstdint>
#include <fstream>
#include <iostream>

using namespace std;

#define KB(x)   ((size_t) (x) << 10)

Cartridge::Cartridge(Nes* nes, Mmu* mmu) :
    nes_(nes), mmu_(mmu) {

}

uint8_t Cartridge::Read8(uint16_t addr) {
    if (0x8000 <= addr && addr <= 0xBFFF) {
        return prg_rom_[addr - 0x8000];
    } else if (0xC000 <= addr && addr <= 0xFFFF) {
        return prg_rom_[addr - 0xC000];
    }
    return 0;
}

void Cartridge::Write8(uint16_t addr, uint8_t data) {

}

void Cartridge::LoadRom(const char* rom) {
    ines_hdr header;

    ifstream file(rom, ios::in | ios::binary);
    if (file.is_open()) {
        file.read((char*)&header, sizeof(header));
        prg_rom_.resize(header.prg_size * KB(16), 0);
        chr_rom_.resize(header.chr_size * KB(16), 0);

        file.read((char*)prg_rom_.data(), prg_rom_.size());
        file.read((char*)chr_rom_.data(), chr_rom_.size());
    } else {
        cout << "File does not exists!" << endl;
    }
    file.close();
}

// uint16_t Cartridge::GetResetVector(void) {
//     uint16_t addr;
//     //memcpy(&addr, _, 2);
//     return addr;
// }
