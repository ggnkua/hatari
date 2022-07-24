#ifndef HARDWARE_ST_H
#define HARDWARE_ST_H

// Common queries to generate meaningful data from raw registers/data

#include <cstdint>

class Memory;

enum MACHINETYPE
{
    MACHINE_ST = 0,
    MACHINE_MEGA_ST = 1,
    MACHINE_STE = 2,
    MACHINE_MEGA_STE = 3,
    MACHINE_TT = 4,
    MACHINE_FALCON = 5
};

namespace HardwareST
{
    bool GetVideoBase(const Memory& mem, MACHINETYPE machineType, uint32_t& address);
    bool GetVideoCurrent(const Memory& mem, uint32_t& address);

    bool GetBlitterSrc(const Memory& mem, MACHINETYPE machineType, uint32_t& address);
    bool GetBlitterDst(const Memory& mem, MACHINETYPE machineType, uint32_t& address);

    bool GetDmaStart(const Memory& mem, MACHINETYPE machineType, uint32_t& address);
    bool GetDmaCurr(const Memory& mem, MACHINETYPE machineType, uint32_t& address);
    bool GetDmaEnd(const Memory& mem, MACHINETYPE machineType, uint32_t& address);
}

#endif // HARDWARE_ST_H
