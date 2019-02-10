#ifndef _CPU_H
#define _CPU_H

#include <cstdint>

class Nes;
class Mmu;

typedef struct {
    uint8_t A;
    uint8_t X;
    uint8_t Y;
    uint16_t PC;
    uint8_t SP;
    uint8_t P;
} registers;

class Cpu {
public:
    enum status_flag {
        F_CARRY = (1 << 0),
        F_ZERO = (1 << 1),
        F_INT_DISABLE = (1 << 2),
        F_DECIMAL = (1 << 3),
        F_BL = (1 << 4),
        F_BH = (1 << 5),
        F_OVERFLOW = (1 << 6),
        F_NEGATIVE = (1 << 7)
    };

    enum opcode {
        OP_INVALID,
        OP_ADC, OP_AND, OP_ASL,
        OP_BCC, OP_BCS, OP_BEQ, OP_BIT, OP_BMI, OP_BNE, OP_BPL, OP_BRK, OP_BVC, OP_BVS,
        OP_CLC, OP_CLD, OP_CLI, OP_CLV, OP_CMP, OP_CPX, OP_CPY,
        OP_DCP, OP_DEC, OP_DEX, OP_DEY,
        OP_EOR, OP_INC, OP_INX, OP_INY, OP_ISB,
        OP_JMP, OP_JSR,
        OP_LAX, OP_LDA, OP_LDX, OP_LDY, OP_LSR,
        OP_NOP,
        OP_ORA,
        OP_PHA, OP_PHP, OP_PLA, OP_PLP,
        OP_ROL, OP_ROR, OP_RTI, OP_RTS,
        OP_SAX, OP_SBC, OP_SEC, OP_SED, OP_SEI, OP_SLO, OP_STA, OP_STX, OP_STY,
        OP_TAX, OP_TAY, OP_TSX, OP_TXA, OP_TXS, OP_TYA
    };

    enum addr_mode {
        MODE_INVALID,
        MODE_ACCUMULATOR,
        MODE_ABSOLUTE,
        MODE_ABSOLUTE_X_INDEXED,
        MODE_ABSOLUTE_Y_INDEXED,
        MODE_IMMEDIATE,
        MODE_IMPLIED,
        MODE_INDIRECT,
        MODE_X_INDEXED_INDIRECT,
        MODE_INDIRECT_Y_INDEXED,
        MODE_RELATIVE,
        MODE_ZEROPAGE,
        MODE_ZEROPAGE_X_INDEXED,
        MODE_ZEROPAGE_Y_INDEXED
    };

    typedef struct {
        opcode      op;
        const char* name;
        addr_mode   mode;
        int         size;
        int         cycles;
        int         pagecrossed_cycles;
    } opcode_t;

    Cpu(Nes* nes, Mmu* mmu);
    ~Cpu() = default;

    void PowerOn(void);
    void Reset(void);
    void Step(void);

    void SetPC(uint16_t addr);

    uint8_t GetFlag(int flag);
    void SetFlag(int flag);
    void ClearFlag(int flag);

private:
    Nes* nes_;
    Mmu* mmu_;
    registers reg_;
    addr_mode mode_;
    static const opcode_t opcode_table_[256];

    uint64_t cycles_;

    void Push8(uint8_t val);
    uint8_t Pop8(void);
    void Push16(uint16_t val);
    uint16_t Pop16(void);
    bool IsPageCrossed(uint16_t new_addr, uint16_t old_addr);
    void SetNZFlag(uint8_t val);
    void SetNFlag(uint8_t val);
    void SetZFlag(uint8_t val);
    uint64_t GetBranchCycles(int8_t offset);
    void Branch(int8_t offset, bool cond);
    void Compare(uint8_t a, uint8_t b);

    void ADC(uint16_t addr);
    void AND(uint16_t addr);
    void ASL(uint16_t addr);

    void BCC(int8_t offset);
    void BCS(int8_t offset);
    void BEQ(int8_t offset);
    void BIT(uint16_t addr);
    void BMI(int8_t offset);
    void BNE(int8_t offset);
    void BPL(int8_t offset);
    void BVC(int8_t offset);
    void BVS(int8_t offset);

    void CLC(void);
    void CLD(void);
    void CLV(void);
    void CMP(uint16_t addr);
    void CPX(uint16_t addr);
    void CPY(uint16_t addr);

    void DCP(uint16_t addr);
    void DEC(uint16_t addr);
    void DEX(void);
    void DEY(void);

    void EOR(uint16_t addr);

    void INC(uint16_t addr);
    void INX(void);
    void INY(void);
    void ISB(uint16_t addr);

    void JMP(uint16_t addr);
    void JSR(uint16_t addr);

    void LAX(uint16_t addr);
    void LDA(uint16_t addr);
    void LDX(uint16_t addr);
    void LDY(uint16_t addr);
    void LSR(uint16_t addr);

    void ORA(uint16_t addr);

    void PHA(void);
    void PHP(void);
    void PLA(void);
    void PLP(void);

    void ROL(uint16_t addr);
    void ROR(uint16_t addr);
    void RTI(void);
    void RTS(void);

    void SAX(uint16_t addr);
    void SBC(uint16_t addr);
    void SEC(void);
    void SED(void);
    void SEI(void);
    void SLO(uint16_t addr);
    void STA(uint16_t addr);
    void STX(uint16_t addr);
    void STY(uint16_t addr);

    void TAX(void);
    void TAY(void);
    void TSX(void);
    void TXA(void);
    void TXS(void);
    void TYA(void);
};

#endif
