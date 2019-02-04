#include "Nes.h"
#include "Cpu.h"
#include "Mmu.h"
#include "Cartridge.h"

using namespace std;

Nes::Nes() {
    mmu_ = make_unique<Mmu>(this);
    cpu_ = make_unique<Cpu>(this, mmu_.get());
    cartridge_ = make_unique<Cartridge>(this, mmu_.get());

    mmu_->AddMemoryMap(mmu_.get(), 0x0000, 0x1FFF);
    // TODO: PPU
    // TODO: APU/IO
    mmu_->AddMemoryMap(cartridge_.get(), 0x4020, 0xFFFF);
}

Nes::~Nes() {
    cpu_ = nullptr;
    mmu_ = nullptr;
    cartridge_ = nullptr;
}

void Nes::PowerOn(void) {
    cpu_->PowerOn();
}

void Nes::Run(const char* rom, emu_mode mode) {
    cartridge_->LoadRom(rom);
    cpu_->Reset();
    if (mode == EMU_MODE_AUTOMATED) {
        cpu_->SetPC(0xC000);
    }
    for(int i = 0; i < 5000; i++) {
        cpu_->Step();
    }
}

// void Nes::RunTestRom(const char* rom) {
//     cartridge_->LoadRom(rom);
//     cpu_->Reset();
//     cpu_->SetPC(0xC000);
//     for(int i = 0; i < 50; i++) {
//         cpu_->Step();
//     }
// }
