#ifndef _IMEMORY_UNIT_H
#define _IMEMORY_UNIT_H

#include <cstdint>

class IMemoryUnit {
public:
    virtual ~IMemoryUnit() = default;

    virtual uint8_t Read8(uint16_t addr) = 0;
    virtual void Write8(uint16_t addr, uint8_t data) = 0;
};

#endif
