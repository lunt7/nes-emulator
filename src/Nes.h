#ifndef _NES_H
#define _NES_H

#include <memory>

class Cpu;
class Mmu;
class Cartridge;

class Nes {
public:
    enum emu_mode {
        EMU_MODE_NORMAL,
        EMU_MODE_AUTOMATED
    };
    Nes();
    ~Nes();

    void PowerOn(void);
    void Run(const char* rom, emu_mode mode=EMU_MODE_NORMAL);
    // void RunTestRom(const char* rom);

private:
    std::unique_ptr<Mmu> mmu_;
    std::unique_ptr<Cpu> cpu_;
    std::unique_ptr<Cartridge> cartridge_;
};


#endif
