#include "Cpu.h"
#include "Nes.h"
#include "Mmu.h"
#include "Logging.h"
#include <cstring>
#include <iomanip>
#include <sstream>

Cpu::Cpu(Nes* nes, Mmu* mmu) :
    nes_(nes), mmu_(mmu)
{
    memset(&reg_, 0, sizeof(reg_));
    // memset(&op, 0, sizeof(op));typedef struct {

}

void Cpu::PowerOn(void) {
    // reg_.P = 0x34;
    reg_.P = 0x24;
    reg_.A = 0;
    reg_.X = 0;
    reg_.Y = 0;
    reg_.SP = 0xFD;
}

void Cpu::Reset(void) {
    reg_.PC = mmu_->Read16(reg_.PC);
    cycles_ = 0;
}

void Cpu::Step(void) {
    uint16_t operand, addr;
    bool pageCrossed = false;

    uint8_t opcode = mmu_->Read8(reg_.PC);
    mode_ = opcode_table_[opcode].mode;

    switch (mode_) {
        case MODE_ABSOLUTE:
            operand = mmu_->Read16(reg_.PC + 1);
            addr = operand;
            break;
        case MODE_ABSOLUTE_X_INDEXED:
            operand = mmu_->Read16(reg_.PC + 1);
            addr = operand + reg_.X;
            pageCrossed = IsPageCrossed(addr, operand);
            break;
        case MODE_ABSOLUTE_Y_INDEXED:
            operand = mmu_->Read16(reg_.PC + 1);
            addr = operand + reg_.Y;
            pageCrossed = IsPageCrossed(addr, operand);
            break;
        case MODE_IMMEDIATE:
            addr = reg_.PC + 1;
            operand = mmu_->Read8(addr);
            break;
        case MODE_INDIRECT:
            operand = mmu_->Read16(reg_.PC + 1);
            addr = mmu_->Read16(operand);
            break;
        case MODE_X_INDEXED_INDIRECT:
            operand = mmu_->Read8(reg_.PC + 1);
            addr = mmu_->Read16_S((operand + reg_.X) & 0xFF);
            break;
        case MODE_INDIRECT_Y_INDEXED:
            operand = mmu_->Read8(reg_.PC + 1);
            addr = mmu_->Read16(operand) + reg_.Y;
            pageCrossed = IsPageCrossed(addr, (mmu_->Read16(operand) & 0xFF00) | ((mmu_->Read16(operand) + reg_.Y) & 0xFF));
            break;
        case MODE_RELATIVE:
            operand = mmu_->Read8(reg_.PC + 1);
            addr = operand;
            break;
        case MODE_ZEROPAGE:
            operand = mmu_->Read8(reg_.PC + 1);
            addr = operand;
            break;
        case MODE_ZEROPAGE_X_INDEXED:
            operand = mmu_->Read8(reg_.PC + 1);
            addr = (operand + reg_.X) & 0xFF;
            break;
        case MODE_ZEROPAGE_Y_INDEXED:
            operand = mmu_->Read8(reg_.PC + 1);
            addr = (operand + reg_.Y) & 0xFF;
            break;
        default:
            break;
    }

    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::uppercase //<< std::hex
       << std::setw(4) << reg_.PC
       << "  "
       << std::setw(2) << (int)opcode
       << " ";
    if (opcode_table_[opcode].size > 1) {
        ss << std::setw(2) << (operand & 0xFF);
    } else {
        ss << "  ";
    }
    ss << " ";
    if (opcode_table_[opcode].size > 2) {
        ss << std::setw(2) << (operand >> 8);
    } else {
        ss << "  ";
    }
    ss << "  "
       << opcode_table_[opcode].name
       << "    "
       << "A:"    << std::setw(2) << (int)reg_.A << " "
       << "X:"    << std::setw(2) << (int)reg_.X << " "
       << "Y:"    << std::setw(2) << (int)reg_.Y << " "
       << "P:"    << std::setw(2) << (int)reg_.P << " "
       << "SP:"   << std::setw(2) << (int)reg_.SP  << " "
       << "CPUC:" << std::dec << cycles_;

    LOG_DEBUG(ss.str());


    reg_.PC += opcode_table_[opcode].size;
    cycles_ += opcode_table_[opcode].cycles;
    if (pageCrossed)
        ++cycles_;

    switch (opcode_table_[opcode].op) {
        case OP_ADC: ADC(addr); break;
        case OP_AND: AND(addr); break;
        case OP_ASL: ASL(addr); break;

        case OP_BCC: BCC(addr); break;
        case OP_BCS: BCS(addr); break;
        case OP_BEQ: BEQ(addr); break;
        case OP_BIT: BIT(addr); break;
        case OP_BMI: BMI(addr); break;
        case OP_BNE: BNE(addr); break;
        case OP_BPL: BPL(addr); break;
        case OP_BVC: BVC(addr); break;
        case OP_BVS: BVS(addr); break;

        case OP_CLC: CLC();     break;
        case OP_CLD: CLD();     break;
        case OP_CLV: CLV();     break;
        case OP_CMP: CMP(addr); break;
        case OP_CPX: CPX(addr); break;
        case OP_CPY: CPY(addr); break;

        case OP_DEC: DEC(addr); break;
        case OP_DEX: DEX();     break;
        case OP_DEY: DEY();     break;

        case OP_EOR: EOR(addr); break;

        case OP_INC: INC(addr); break;
        case OP_INX: INX();     break;
        case OP_INY: INY();     break;

        case OP_JMP: JMP(addr); break;
        case OP_JSR: JSR(addr); break;

        case OP_LDA: LDA(addr); break;
        case OP_LDX: LDX(addr); break;
        case OP_LDY: LDY(addr); break;
        case OP_LSR: LSR(addr); break;

        case OP_NOP:    // do nothing
            break;

        case OP_ORA: ORA(addr); break;

        case OP_PHA: PHA();     break;
        case OP_PHP: PHP();     break;
        case OP_PLA: PLA();     break;
        case OP_PLP: PLP();     break;

        case OP_ROL: ROL(addr); break;
        case OP_ROR: ROR(addr); break;
        case OP_RTI: RTI();     break;
        case OP_RTS: RTS();     break;

        case OP_SBC: SBC(addr); break;
        case OP_SEC: SEC();     break;
        case OP_SED: SED();     break;
        case OP_SEI: SEI();     break;
        case OP_STA: STA(addr); break;
        case OP_STX: STX(addr); break;
        case OP_STY: STY(addr); break;

        case OP_TAX: TAX();     break;
        case OP_TAY: TAY();     break;
        case OP_TSX: TSX();     break;
        case OP_TXA: TXA();     break;
        case OP_TXS: TXS();     break;
        case OP_TYA: TYA();     break;

        default:
            LOG_DEBUG("Illegal instructions");
            break;
    }
}

void Cpu::SetPC(uint16_t addr) {
    reg_.PC = addr;
}

inline uint8_t Cpu::GetFlag(int flag) {
    return (reg_.P & flag) ? 1 : 0;
}

inline void Cpu::SetFlag(int flag) {
    reg_.P |= flag;
}

inline void Cpu::ClearFlag(int flag) {
    reg_.P &= ~(flag);
}

void Cpu::Push8(uint8_t val) {
    mmu_->Write8(0x100 + reg_.SP, val);
    --reg_.SP;
}

uint8_t Cpu::Pop8(void) {
    ++reg_.SP;
    return mmu_->Read8(0x100 + reg_.SP);
}

void Cpu::Push16(uint16_t val) {
    Push8(val >> 8);
    Push8(val);
}

uint16_t Cpu::Pop16(void) {
    uint16_t val_l = Pop8();
    uint16_t val_h = Pop8() << 8;
    return (val_h | val_l);
}

bool Cpu::IsPageCrossed(uint16_t new_addr, uint16_t old_addr) {
    return (new_addr & 0xFF00) != (old_addr & 0xFF00);
}

void Cpu::SetNZFlag(uint8_t val) {
    SetNFlag(val);
    SetZFlag(val);
}

void Cpu::SetNFlag(uint8_t val) {
    if (val & 0x80) {
        SetFlag(F_NEGATIVE);
    } else {
        ClearFlag(F_NEGATIVE);
    }
}

void Cpu::SetZFlag(uint8_t val) {
    if (val == 0) {
        SetFlag(F_ZERO);
    } else {
        ClearFlag(F_ZERO);
    }
}

uint64_t Cpu::GetBranchCycles(uint8_t offset) {
    uint64_t branch_cycles = 0;
    ++branch_cycles;
    if (IsPageCrossed(reg_.PC, reg_.PC - offset)) {
        ++branch_cycles;
    }
    return branch_cycles;
}

void Cpu::Branch(int8_t offset, bool cond) {
    if (cond) {
        reg_.PC += offset;
        cycles_ += GetBranchCycles(offset);
    }
}

void Cpu::Compare(uint8_t a, uint8_t b) {
    uint8_t diff = a - b;
    SetNZFlag(diff);
    if (a >= b) {
        SetFlag(F_CARRY);
    } else {
        ClearFlag(F_CARRY);
    }
}


void Cpu::ADC(uint16_t addr) {
    uint8_t val = mmu_->Read8(addr);
    uint16_t sum = reg_.A + val + GetFlag(F_CARRY);
    uint8_t bit7_carry = ((reg_.A & 0x7F) + (val & 0x7F) + GetFlag(F_CARRY)) & 0x80 ? 1 : 0;

    reg_.A = sum;
    SetNZFlag(reg_.A);
    if (sum > 255) {
        SetFlag(F_CARRY);
    } else {
        ClearFlag(F_CARRY);
    }
    if (bit7_carry ^ GetFlag(F_CARRY)) {
        SetFlag(F_OVERFLOW);
    } else {
        ClearFlag(F_OVERFLOW);
    }
}

void Cpu::AND(uint16_t addr) {
    reg_.A &= mmu_->Read16(addr);
    SetNZFlag(reg_.A);
}

void Cpu::ASL(uint16_t addr) {
    if (mode_ == MODE_ACCUMULATOR) {
        if (reg_.A & 0x80) {
            SetFlag(F_CARRY);
        } else {
            ClearFlag(F_CARRY);
        }
        reg_.A <<= 1;
        SetNZFlag(reg_.A);
    } else {
        uint8_t val = mmu_->Read8(addr);
        if (val & 0x80) {
            SetFlag(F_CARRY);
        } else {
            ClearFlag(F_CARRY);
        }
        val <<= 1;
        mmu_->Write8(addr, val);
        SetNZFlag(val);
    }
}


void Cpu::BCC(int8_t offset) {
    Branch(offset, !GetFlag(F_CARRY));
}

void Cpu::BCS(int8_t offset) {
    Branch(offset, GetFlag(F_CARRY));
}

void Cpu::BEQ(int8_t offset) {
    Branch(offset, GetFlag(F_ZERO));
}

void Cpu::BIT(uint16_t addr) {
    uint8_t val = mmu_->Read8(addr);
    SetNFlag(val);
    if (val & 0x40) {
        SetFlag(F_OVERFLOW);
    } else {
        ClearFlag(F_OVERFLOW);
    }
    SetZFlag(val & reg_.A);
}

void Cpu::BMI(int8_t offset) {
    Branch(offset, GetFlag(F_NEGATIVE));
}

void Cpu::BNE(int8_t offset) {
    Branch(offset, !GetFlag(F_ZERO));
}

void Cpu::BPL(int8_t offset) {
    Branch(offset, !GetFlag(F_NEGATIVE));
}

void Cpu::BVC(int8_t offset) {
    Branch(offset, !GetFlag(F_OVERFLOW));
}

void Cpu::BVS(int8_t offset) {
    Branch(offset, GetFlag(F_OVERFLOW));
}


void Cpu::CLC(void) {
    ClearFlag(F_CARRY);
}

void Cpu::CLD(void) {
    ClearFlag(F_DECIMAL);
}

void Cpu::CLV(void) {
    ClearFlag(F_OVERFLOW);
}

void Cpu::CMP(uint16_t addr) {
    uint8_t val = mmu_->Read8(addr);
    Compare(reg_.A, val);
}

void Cpu::CPX(uint16_t addr) {
    uint8_t val = mmu_->Read8(addr);
    Compare(reg_.X, val);
}

void Cpu::CPY(uint16_t addr) {
    uint8_t val = mmu_->Read8(addr);
    Compare(reg_.Y, val);
}


void Cpu::DEC(uint16_t addr) {
    uint8_t val = mmu_->Read8(addr) - 1;
    mmu_->Write8(addr, val);
    SetNZFlag(val);
}

void Cpu::DEX(void) {
    SetNZFlag(--reg_.X);
}

void Cpu::DEY(void) {
    SetNZFlag(--reg_.Y);
}


void Cpu::EOR(uint16_t addr) {
    reg_.A ^= mmu_->Read8(addr);
    SetNZFlag(reg_.A);
}


void Cpu::INC(uint16_t addr) {
    uint8_t val = mmu_->Read8(addr) + 1;
    mmu_->Write8(addr, val);
    SetNZFlag(val);
}

void Cpu::INX(void) {
    SetNZFlag(++reg_.X);
}

void Cpu::INY(void) {
    SetNZFlag(++reg_.Y);
}


void Cpu::JMP(uint16_t addr) {
    reg_.PC = addr;
}

void Cpu::JSR(uint16_t addr) {
    Push16(reg_.PC - 1);
    reg_.PC = addr;
}


void Cpu::LDA(uint16_t addr) {
    reg_.A = mmu_->Read8(addr);
    SetNZFlag(reg_.A);
}

void Cpu::LDX(uint16_t addr) {
    reg_.X = mmu_->Read8(addr);
    SetNZFlag(reg_.X);
}

void Cpu::LDY(uint16_t addr) {
    reg_.Y = mmu_->Read8(addr);
    SetNZFlag(reg_.Y);
}

void Cpu::LSR(uint16_t addr) {
    if (mode_ == MODE_ACCUMULATOR) {
        if (reg_.A & 0x01) {
            SetFlag(F_CARRY);
        } else {
            ClearFlag(F_CARRY);
        }
        reg_.A >>= 1;
        SetNZFlag(reg_.A);
    } else {
        uint8_t val = mmu_->Read8(addr);
        if (val & 0x01) {
            SetFlag(F_CARRY);
        } else {
            ClearFlag(F_CARRY);
        }
        val >>= 1;
        mmu_->Write8(addr, val);
        SetNZFlag(val);
    }
}


void Cpu::ORA(uint16_t addr) {
    reg_.A |= mmu_->Read8(addr);
    SetNZFlag(reg_.A);
}


void Cpu::PHA(void) {
    Push8(reg_.A);
}

void Cpu::PHP(void) {
    Push8(reg_.P | F_BH | F_BL);
}

void Cpu::PLA(void) {
    reg_.A = Pop8();
    SetNZFlag(reg_.A);
}

void Cpu::PLP(void) {
    reg_.P = (reg_.P & (F_BH | F_BL)) |
             (Pop8() & ~(F_BH | F_BL));
}


void Cpu::ROL(uint16_t addr) {
    uint8_t c = GetFlag(F_CARRY);
    if (mode_ == MODE_ACCUMULATOR) {
        if (reg_.A & 0x80) {
            SetFlag(F_CARRY);
        } else {
            ClearFlag(F_CARRY);
        }
        reg_.A = (reg_.A << 1) | c;
        SetNZFlag(reg_.A);
    } else {
        uint8_t val = mmu_->Read8(addr);
        if (val & 0x80) {
            SetFlag(F_CARRY);
        } else {
            ClearFlag(F_CARRY);
        }
        val = (val << 1) | c;
        mmu_->Write8(addr, val);
        SetNZFlag(val);
    }
}

void Cpu::ROR(uint16_t addr) {
    uint8_t c = GetFlag(F_CARRY);
    if (mode_ == MODE_ACCUMULATOR) {
        if (reg_.A & 0x01) {
            SetFlag(F_CARRY);
        } else {
            ClearFlag(F_CARRY);
        }
        reg_.A = (c << 7) | (reg_.A >> 1);
        SetNZFlag(reg_.A);
    } else {
        uint8_t val = mmu_->Read8(addr);
        if (val & 0x01) {
            SetFlag(F_CARRY);
        } else {
            ClearFlag(F_CARRY);
        }
        val = (c << 7) | (val >> 1);
        mmu_->Write8(addr, val);
        SetNZFlag(val);
    }
}

void Cpu::RTI(void) {
    reg_.P = (reg_.P & (F_BH | F_BL)) |
             (Pop8() & ~(F_BH | F_BL));
    reg_.PC = Pop16();
}

void Cpu::RTS(void) {
    reg_.PC = Pop16() + 1;
}


void Cpu::SBC(uint16_t addr) {
    uint8_t val = ~mmu_->Read8(addr);
    uint16_t sum = reg_.A + val + GetFlag(F_CARRY);
    uint8_t bit7_carry = ((reg_.A & 0x7F) + (val & 0x7F) + GetFlag(F_CARRY)) & 0x80 ? 1 : 0;

    reg_.A = sum;
    SetNZFlag(reg_.A);
    if (sum > 255) {
        SetFlag(F_CARRY);
    } else {
        ClearFlag(F_CARRY);
    }
    if (bit7_carry ^ GetFlag(F_CARRY)) {
        SetFlag(F_OVERFLOW);
    } else {
        ClearFlag(F_OVERFLOW);
    }
}

void Cpu::SEC(void) {
    SetFlag(F_CARRY);
}

void Cpu::SED(void) {
    SetFlag(F_DECIMAL);
}

void Cpu::SEI(void) {
    SetFlag(F_INT_DISABLE);
}

void Cpu::STA(uint16_t addr) {
    mmu_->Write8(addr, reg_.A);
}

void Cpu::STX(uint16_t addr) {
    mmu_->Write8(addr, reg_.X);
}

void Cpu::STY(uint16_t addr) {
    mmu_->Write8(addr, reg_.Y);
}



void Cpu::TAX(void) {
    reg_.X = reg_.A;
    SetNZFlag(reg_.X);
}

void Cpu::TAY(void) {
    reg_.Y = reg_.A;
    SetNZFlag(reg_.Y);
}

void Cpu::TSX(void) {
    reg_.X = reg_.SP;
    SetNZFlag(reg_.X);
}

void Cpu::TXA(void) {
    reg_.A = reg_.X;
    SetNZFlag(reg_.A);
}

void Cpu::TXS(void) {
    reg_.SP = reg_.X;
}

void Cpu::TYA(void) {
    reg_.A = reg_.Y;
    SetNZFlag(reg_.A);
}

const Cpu::opcode_t Cpu::opcode_table_[256] = {
    /* 0x00 */ { OP_BRK,     "BRK",            MODE_IMPLIED,            1, 7 },
    /* 0x01 */ { OP_ORA,     "ORA",            MODE_X_INDEXED_INDIRECT, 2, 6 },
    /* 0x02 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x03 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x04 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x05 */ { OP_ORA,     "ORA",            MODE_ZEROPAGE,           2, 3 },
    /* 0x06 */ { OP_ASL,     "ASL",            MODE_ZEROPAGE,           2, 5 },
    /* 0x07 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x08 */ { OP_PHP,     "PHP",            MODE_IMPLIED,            1, 3 },
    /* 0x09 */ { OP_ORA,     "ORA",            MODE_IMMEDIATE,          2, 2 },
    /* 0x0A */ { OP_ASL,     "ASL",            MODE_ACCUMULATOR,        1, 2 },
    /* 0x0B */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x0C */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x0D */ { OP_ORA,     "ORA",            MODE_ABSOLUTE,           3, 4 },
    /* 0x0E */ { OP_ASL,     "ASL",            MODE_ABSOLUTE,           3, 6 },
    /* 0x0F */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },

    /* 0x10 */ { OP_BPL,     "BPL",            MODE_RELATIVE,           2, 2 },
    /* 0x11 */ { OP_ORA,     "ORA",            MODE_INDIRECT_Y_INDEXED, 2, 5 },
    /* 0x12 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x13 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x14 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x15 */ { OP_ORA,     "ORA",            MODE_ZEROPAGE_X_INDEXED, 2, 4 },
    /* 0x16 */ { OP_ASL,     "ASL",            MODE_ZEROPAGE_X_INDEXED, 2, 6 },
    /* 0x17 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x18 */ { OP_CLC,     "CLC",            MODE_IMPLIED,            1, 2 },
    /* 0x19 */ { OP_ORA,     "ORA",            MODE_ABSOLUTE_Y_INDEXED, 3, 4 },
    /* 0x1A */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x1B */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x1C */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x1D */ { OP_ORA,     "ORA",            MODE_ABSOLUTE_X_INDEXED, 3, 4 },
    /* 0x1E */ { OP_ASL,     "ASL",            MODE_ABSOLUTE_X_INDEXED, 3, 7 },
    /* 0x1F */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },

    /* 0x20 */ { OP_JSR,     "JSR",            MODE_ABSOLUTE,           3, 6 },
    /* 0x21 */ { OP_AND,     "AND",            MODE_X_INDEXED_INDIRECT, 2, 6 },
    /* 0x22 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x23 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x24 */ { OP_BIT,     "BIT",            MODE_ZEROPAGE,           2, 3 },
    /* 0x25 */ { OP_AND,     "AND",            MODE_ZEROPAGE,           2, 3 },
    /* 0x26 */ { OP_ROL,     "ROL",            MODE_ZEROPAGE,           2, 5 },
    /* 0x27 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x28 */ { OP_PLP,     "PLP",            MODE_IMPLIED,            1, 4 },
    /* 0x29 */ { OP_AND,     "AND",            MODE_IMMEDIATE,          2, 2 },
    /* 0x2A */ { OP_ROL,     "ROL",            MODE_ACCUMULATOR,        1, 2 },
    /* 0x2B */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x2C */ { OP_BIT,     "BIT",            MODE_ABSOLUTE,           3, 4 },
    /* 0x2D */ { OP_AND,     "AND",            MODE_ABSOLUTE,           3, 4 },
    /* 0x2E */ { OP_ROL,     "ROL",            MODE_ABSOLUTE,           3, 6 },
    /* 0x2F */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },

    /* 0x30 */ { OP_BMI,     "BMI",            MODE_RELATIVE,           2, 2 },
    /* 0x31 */ { OP_AND,     "AND",            MODE_INDIRECT_Y_INDEXED, 2, 5 },
    /* 0x32 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x33 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x34 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x35 */ { OP_AND,     "AND",            MODE_ZEROPAGE_X_INDEXED, 2, 4 },
    /* 0x36 */ { OP_ROL,     "ROL",            MODE_ZEROPAGE_X_INDEXED, 2, 6 },
    /* 0x37 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x38 */ { OP_SEC,     "SEC",            MODE_IMPLIED,            1, 2 },
    /* 0x39 */ { OP_AND,     "AND",            MODE_ABSOLUTE_Y_INDEXED, 3, 4 },
    /* 0x3A */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x3B */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x3C */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x3D */ { OP_AND,     "AND",            MODE_ABSOLUTE_X_INDEXED, 3, 4 },
    /* 0x3E */ { OP_ROL,     "ROL",            MODE_ABSOLUTE_X_INDEXED, 3, 7 },
    /* 0x3F */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },

    /* 0x40 */ { OP_RTI,     "RTI",            MODE_IMPLIED,            1, 6 },
    /* 0x41 */ { OP_EOR,     "EOR",            MODE_X_INDEXED_INDIRECT, 2, 6 },
    /* 0x42 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x43 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x44 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x45 */ { OP_EOR,     "EOR",            MODE_ZEROPAGE,           2, 3 },
    /* 0x46 */ { OP_LSR,     "LSR",            MODE_ZEROPAGE,           2, 5 },
    /* 0x47 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x48 */ { OP_PHA,     "PHA",            MODE_IMPLIED,            1, 3 },
    /* 0x49 */ { OP_EOR,     "EOR",            MODE_IMMEDIATE,          2, 2 },
    /* 0x4A */ { OP_LSR,     "LSR",            MODE_ACCUMULATOR,        1, 2 },
    /* 0x4B */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x4C */ { OP_JMP,     "JMP",            MODE_ABSOLUTE,           3, 3 },
    /* 0x4D */ { OP_EOR,     "EOR",            MODE_ABSOLUTE,           3, 4 },
    /* 0x4E */ { OP_LSR,     "LSR",            MODE_ABSOLUTE,           3, 6 },
    /* 0x4F */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },

    /* 0x50 */ { OP_BVC,     "BVC",            MODE_RELATIVE,           2, 2 },
    /* 0x51 */ { OP_EOR,     "EOR",            MODE_INDIRECT_Y_INDEXED, 2, 5 },
    /* 0x52 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x53 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x54 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x55 */ { OP_EOR,     "EOR",            MODE_ZEROPAGE_X_INDEXED, 2, 4 },
    /* 0x56 */ { OP_LSR,     "LSR",            MODE_ZEROPAGE_X_INDEXED, 2, 6 },
    /* 0x57 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x58 */ { OP_CLI,     "CLI",            MODE_IMPLIED,            1, 2 },
    /* 0x59 */ { OP_EOR,     "EOR",            MODE_ABSOLUTE_Y_INDEXED, 3, 4 },
    /* 0x5A */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x5B */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x5C */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x5D */ { OP_EOR,     "EOR",            MODE_ABSOLUTE_X_INDEXED, 3, 4 },
    /* 0x5E */ { OP_LSR,     "LSR",            MODE_ABSOLUTE_X_INDEXED, 3, 7 },
    /* 0x5F */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },

    /* 0x60 */ { OP_RTS,     "RTS",            MODE_IMPLIED,            1, 6 },
    /* 0x61 */ { OP_ADC,     "ADC",            MODE_X_INDEXED_INDIRECT, 2, 6 },
    /* 0x62 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x63 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x64 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x65 */ { OP_ADC,     "ADC",            MODE_ZEROPAGE,           2, 3 },
    /* 0x66 */ { OP_ROR,     "ROR",            MODE_ZEROPAGE,           2, 5 },
    /* 0x67 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x68 */ { OP_PLA,     "PLA",            MODE_IMPLIED,            1, 4 },
    /* 0x69 */ { OP_ADC,     "ADC",            MODE_IMMEDIATE,          2, 2 },
    /* 0x6A */ { OP_ROR,     "ROR",            MODE_ACCUMULATOR,        1, 2 },
    /* 0x6B */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x6C */ { OP_JMP,     "JMP",            MODE_INDIRECT,           3, 5 },
    /* 0x6D */ { OP_ADC,     "ADC",            MODE_ABSOLUTE,           3, 4 },
    /* 0x6E */ { OP_ROR,     "ROR",            MODE_ABSOLUTE,           3, 6 },
    /* 0x6F */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },

    /* 0x70 */ { OP_BVS,     "BVS",            MODE_RELATIVE,           2, 2 },
    /* 0x71 */ { OP_ADC,     "ADC",            MODE_INDIRECT_Y_INDEXED, 2, 5 },
    /* 0x72 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x73 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x74 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x75 */ { OP_ADC,     "ADC",            MODE_ZEROPAGE_X_INDEXED, 2, 4 },
    /* 0x76 */ { OP_ROR,     "ROR",            MODE_ZEROPAGE_X_INDEXED, 2, 6 },
    /* 0x77 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x78 */ { OP_SEI,     "SEI",            MODE_IMPLIED,            1, 2 },
    /* 0x79 */ { OP_ADC,     "ADC",            MODE_ABSOLUTE_Y_INDEXED, 3, 4 },
    /* 0x7A */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x7B */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x7C */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x7D */ { OP_ADC,     "ADC",            MODE_ABSOLUTE_X_INDEXED, 3, 4 },
    /* 0x7E */ { OP_ROR,     "ROR",            MODE_ABSOLUTE_X_INDEXED, 3, 7 },
    /* 0x7F */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },

    /* 0x80 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x81 */ { OP_STA,     "STA",            MODE_X_INDEXED_INDIRECT, 2, 6 },
    /* 0x82 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x83 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x84 */ { OP_STY,     "STY",            MODE_ZEROPAGE,           2, 3 },
    /* 0x85 */ { OP_STA,     "STA",            MODE_ZEROPAGE,           2, 3 },
    /* 0x86 */ { OP_STX,     "STX",            MODE_ZEROPAGE,           2, 3 },
    /* 0x87 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x88 */ { OP_DEY,     "DEY",            MODE_IMPLIED,            1, 2 },
    /* 0x89 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x8A */ { OP_TXA,     "TXA",            MODE_IMPLIED,            1, 2 },
    /* 0x8B */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x8C */ { OP_STY,     "STY",            MODE_ABSOLUTE,           3, 4 },
    /* 0x8D */ { OP_STA,     "STA",            MODE_ABSOLUTE,           3, 4 },
    /* 0x8E */ { OP_STX,     "STX",            MODE_ABSOLUTE,           3, 4 },
    /* 0x8F */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },

    /* 0x90 */ { OP_BCC,     "BCC",            MODE_RELATIVE,           2, 2 },
    /* 0x91 */ { OP_STA,     "STA",            MODE_INDIRECT_Y_INDEXED, 2, 6 },
    /* 0x92 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x93 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x94 */ { OP_STY,     "STY",            MODE_ZEROPAGE_X_INDEXED, 2, 4 },
    /* 0x95 */ { OP_STA,     "STA",            MODE_ZEROPAGE_X_INDEXED, 2, 4 },
    /* 0x96 */ { OP_STX,     "STX",            MODE_ZEROPAGE_Y_INDEXED, 2, 4 },
    /* 0x97 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x98 */ { OP_TYA,     "TYA",            MODE_IMPLIED,            1, 2 },
    /* 0x99 */ { OP_STA,     "STA",            MODE_ABSOLUTE_Y_INDEXED, 3, 5 },
    /* 0x9A */ { OP_TXS,     "TXS",            MODE_IMPLIED,            1, 2 },
    /* 0x9B */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x9C */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x9D */ { OP_STA,     "STA",            MODE_ABSOLUTE_X_INDEXED, 3, 5 },
    /* 0x9E */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0x9F */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },

    /* 0xA0 */ { OP_LDY,     "LDY",            MODE_IMMEDIATE,          2, 2 },
    /* 0xA1 */ { OP_LDA,     "LDA",            MODE_X_INDEXED_INDIRECT, 2, 6 },
    /* 0xA2 */ { OP_LDX,     "LDX",            MODE_IMMEDIATE,          2, 2 },
    /* 0xA3 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xA4 */ { OP_LDY,     "LDY",            MODE_ZEROPAGE,           2, 3 },
    /* 0xA5 */ { OP_LDA,     "LDA",            MODE_ZEROPAGE,           2, 3 },
    /* 0xA6 */ { OP_LDX,     "LDX",            MODE_ZEROPAGE,           2, 3 },
    /* 0xA7 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xA8 */ { OP_TAY,     "TAY",            MODE_IMPLIED,            1, 2 },
    /* 0xA9 */ { OP_LDA,     "LDA",            MODE_IMMEDIATE,          2, 2 },
    /* 0xAA */ { OP_TAX,     "TAX",            MODE_IMPLIED,            1, 2 },
    /* 0xAB */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xAC */ { OP_LDY,     "LDY",            MODE_ABSOLUTE,           3, 4 },
    /* 0xAD */ { OP_LDA,     "LDA",            MODE_ABSOLUTE,           3, 4 },
    /* 0xAE */ { OP_LDX,     "LDX",            MODE_ABSOLUTE,           3, 4 },
    /* 0xAF */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },

    /* 0xB0 */ { OP_BCS,     "BCS",            MODE_RELATIVE,           2, 2 },
    /* 0xB1 */ { OP_LDA,     "LDA",            MODE_INDIRECT_Y_INDEXED, 2, 5 },
    /* 0xB2 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xB3 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xB4 */ { OP_LDY,     "LDY",            MODE_ZEROPAGE_Y_INDEXED, 2, 4 },
    /* 0xB5 */ { OP_LDA,     "LDA",            MODE_ZEROPAGE_X_INDEXED, 2, 4 },
    /* 0xB6 */ { OP_LDX,     "LDX",            MODE_ZEROPAGE_Y_INDEXED, 2, 4 },
    /* 0xB7 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xB8 */ { OP_CLV,     "CLV",            MODE_IMPLIED,            1, 2 },
    /* 0xB9 */ { OP_LDA,     "LDA",            MODE_ABSOLUTE_Y_INDEXED, 3, 4 },
    /* 0xBA */ { OP_TSX,     "TSX",            MODE_IMPLIED,            1, 2 },
    /* 0xBB */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xBC */ { OP_LDY,     "LDY",            MODE_ABSOLUTE_X_INDEXED, 3, 4 },
    /* 0xBD */ { OP_LDA,     "LDA",            MODE_ABSOLUTE_X_INDEXED, 3, 4 },
    /* 0xBE */ { OP_LDX,     "LDX",            MODE_ABSOLUTE_Y_INDEXED, 3, 4 },
    /* 0xBF */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },

    /* 0xC0 */ { OP_CPY,     "CPY",            MODE_IMMEDIATE,          2, 2 },
    /* 0xC1 */ { OP_CMP,     "CMP",            MODE_X_INDEXED_INDIRECT, 2, 6 },
    /* 0xC2 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xC3 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xC4 */ { OP_CPY,     "CPY",            MODE_ZEROPAGE,           2, 3 },
    /* 0xC5 */ { OP_CMP,     "CMP",            MODE_ZEROPAGE,           2, 3 },
    /* 0xC6 */ { OP_DEC,     "DEC",            MODE_ZEROPAGE,           2, 5 },
    /* 0xC7 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xC8 */ { OP_INY,     "INY",            MODE_IMPLIED,            1, 2 },
    /* 0xC9 */ { OP_CMP,     "CMP",            MODE_IMMEDIATE,          2, 2 },
    /* 0xCA */ { OP_DEX,     "DEX",            MODE_IMPLIED,            1, 2 },
    /* 0xCB */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xCC */ { OP_CPY,     "CPY",            MODE_ABSOLUTE,           3, 4 },
    /* 0xCD */ { OP_CMP,     "CMP",            MODE_ABSOLUTE,           3, 4 },
    /* 0xCE */ { OP_DEC,     "DEC",            MODE_ABSOLUTE,           3, 6 },
    /* 0xCF */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },

    /* 0xD0 */ { OP_BNE,     "BNE",            MODE_RELATIVE,           2, 2 },
    /* 0xD1 */ { OP_CMP,     "CMP",            MODE_INDIRECT_Y_INDEXED, 2, 5 },
    /* 0xD2 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xD3 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xD4 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xD5 */ { OP_CMP,     "CMP",            MODE_ZEROPAGE_X_INDEXED, 2, 4 },
    /* 0xD6 */ { OP_DEC,     "DEC",            MODE_ZEROPAGE_X_INDEXED, 2, 6 },
    /* 0xD7 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xD8 */ { OP_CLD,     "CLD",            MODE_IMPLIED,            1, 2 },
    /* 0xD9 */ { OP_CMP,     "CMP",            MODE_ABSOLUTE_Y_INDEXED, 3, 4 },
    /* 0xDA */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xDB */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xDC */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xDD */ { OP_CMP,     "CMP",            MODE_ABSOLUTE_X_INDEXED, 3, 4 },
    /* 0xDE */ { OP_DEC,     "DEC",            MODE_ABSOLUTE_X_INDEXED, 3, 7 },
    /* 0xDF */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },

    /* 0xE0 */ { OP_CPX,     "CPX",            MODE_IMMEDIATE,          2, 2 },
    /* 0xE1 */ { OP_SBC,     "SBC",            MODE_X_INDEXED_INDIRECT, 2, 6 },
    /* 0xE2 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xE3 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xE4 */ { OP_CPX,     "CPX",            MODE_ZEROPAGE,           2, 3 },
    /* 0xE5 */ { OP_SBC,     "SBC",            MODE_ZEROPAGE,           2, 3 },
    /* 0xE6 */ { OP_INC,     "INC",            MODE_ZEROPAGE,           2, 5 },
    /* 0xE7 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xE8 */ { OP_INX,     "INX",            MODE_IMPLIED,            1, 2 },
    /* 0xE9 */ { OP_SBC,     "SBC",            MODE_IMMEDIATE,          2, 2 },
    /* 0xEA */ { OP_NOP,     "NOP",            MODE_IMPLIED,            1, 2 },
    /* 0xEB */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xEC */ { OP_CPX,     "CPX",            MODE_ABSOLUTE,           3, 4 },
    /* 0xED */ { OP_SBC,     "SBC",            MODE_ABSOLUTE,           3, 4 },
    /* 0xEE */ { OP_INC,     "INC",            MODE_ABSOLUTE,           3, 6 },
    /* 0xEF */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },

    /* 0xF0 */ { OP_BEQ,     "BEQ",            MODE_RELATIVE,           2, 2 },
    /* 0xF1 */ { OP_SBC,     "SBC",            MODE_INDIRECT_Y_INDEXED, 2, 5 },
    /* 0xF2 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xF3 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xF4 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xF5 */ { OP_SBC,     "SBC",            MODE_ZEROPAGE_X_INDEXED, 2, 4 },
    /* 0xF6 */ { OP_INC,     "INC",            MODE_ZEROPAGE_X_INDEXED, 2, 6 },
    /* 0xF7 */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xF8 */ { OP_SED,     "SED",            MODE_IMPLIED,            1, 2 },
    /* 0xF9 */ { OP_SBC,     "SBC",            MODE_ABSOLUTE_Y_INDEXED, 3, 4 },
    /* 0xFA */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xFB */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xFC */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 },
    /* 0xFD */ { OP_SBC,     "SBC",            MODE_ABSOLUTE_X_INDEXED, 3, 4 },
    /* 0xFE */ { OP_INC,     "INC",            MODE_ABSOLUTE_X_INDEXED, 3, 7 },
    /* 0xFF */ { OP_INVALID, "INVALID OPCODE", MODE_INVALID,            0, 0 }
};
