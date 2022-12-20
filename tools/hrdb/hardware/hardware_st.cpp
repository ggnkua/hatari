#include "hardware_st.h"
#include "regs_st.h"

#include "../models/memory.h"
#include "../models/targetmodel.h"

namespace HardwareST
{
    bool GetVideoBase(const Memory& mem, MACHINETYPE machineType, uint32_t& address)
    {
        if (!mem.HasAddress(Regs::VID_BASE_HIGH))
            return false;
        if (!mem.HasAddress(Regs::VID_BASE_MID))
            return false;
        uint32_t hi = mem.ReadAddressByte(Regs::VID_BASE_HIGH);
        uint32_t mi = mem.ReadAddressByte(Regs::VID_BASE_MID);
        uint32_t lo = 0;
        if (!IsMachineST(machineType))
        {
            if (!mem.HasAddress(Regs::VID_BASE_LOW_STE))
                return false;
            lo = mem.ReadAddressByte(Regs::VID_BASE_LOW_STE);
        }
        address = (hi << 16) | (mi << 8) | lo;
        return true;
    }

    bool GetVideoCurrent(const Memory& mem, uint32_t& address)
    {
        if (!mem.HasAddress(Regs::VID_CURR_HIGH))
            return false;
        if (!mem.HasAddress(Regs::VID_CURR_MID))
            return false;
        if (!mem.HasAddress(Regs::VID_CURR_LOW))
            return false;
        uint32_t hi = mem.ReadAddressByte(Regs::VID_CURR_HIGH);
        uint32_t mi = mem.ReadAddressByte(Regs::VID_CURR_MID);
        uint32_t lo = mem.ReadAddressByte(Regs::VID_CURR_LOW);
        address = (hi << 16) | (mi << 8) | lo;
        return true;
    }

    bool GetBlitterSrc(const Memory &mem, MACHINETYPE machineType, uint32_t &address)
    {
        if (IsMachineST(machineType))
            return false;

        if (!mem.HasAddressMulti(Regs::BLT_SRC_ADDR, 4))
            return false;
        uint32_t val = mem.ReadAddressMulti(Regs::BLT_SRC_ADDR, 4);
        address = val & 0xffffff;
        return true;
    }

    bool GetBlitterDst(const Memory &mem, MACHINETYPE machineType, uint32_t &address)
    {
        if (IsMachineST(machineType))
            return false;

        if (!mem.HasAddressMulti(Regs::BLT_DST_ADDR, 4))
            return false;
        uint32_t val = mem.ReadAddressMulti(Regs::BLT_DST_ADDR, 4);
        address = val & 0xffffff;
        return true;
    }

    bool GetDmaStart(const Memory &mem, MACHINETYPE machineType, uint32_t &address)
    {
        if (!IsMachineSTE(machineType))
            return false;
        if (!mem.HasAddressMulti(Regs::DMA_START_HIGH, 5))
            return false;
        uint32_t hi = mem.ReadAddressByte(Regs::DMA_START_HIGH);
        uint32_t mi = mem.ReadAddressByte(Regs::DMA_START_MID);
        uint32_t lo = mem.ReadAddressByte(Regs::DMA_START_LOW);
        address = (hi << 16) | (mi << 8) | lo;
        return true;
    }
    bool GetDmaCurr(const Memory &mem, MACHINETYPE machineType, uint32_t &address)
    {
        if (!IsMachineSTE(machineType))
            return false;
        if (!mem.HasAddressMulti(Regs::DMA_CURR_HIGH, 5))
            return false;
        uint32_t hi = mem.ReadAddressByte(Regs::DMA_CURR_HIGH);
        uint32_t mi = mem.ReadAddressByte(Regs::DMA_CURR_MID);
        uint32_t lo = mem.ReadAddressByte(Regs::DMA_CURR_LOW);
        address = (hi << 16) | (mi << 8) | lo;
        return true;
    }
    bool GetDmaEnd(const Memory &mem, MACHINETYPE machineType, uint32_t &address)
    {
        if (!IsMachineSTE(machineType))
            return false;
        if (!mem.HasAddressMulti(Regs::DMA_END_HIGH, 5))
            return false;
        uint32_t hi = mem.ReadAddressByte(Regs::DMA_END_HIGH);
        uint32_t mi = mem.ReadAddressByte(Regs::DMA_END_MID);
        uint32_t lo = mem.ReadAddressByte(Regs::DMA_END_LOW);
        address = (hi << 16) | (mi << 8) | lo;
        return true;
    }

    void GetColour(uint16_t regValue, MACHINETYPE machineType, uint32_t &result)
    {
        static const uint32_t stToRgb[16] =
        {
            0x00, 0x22, 0x44, 0x66, 0x88, 0xaa, 0xcc, 0xee,
            0x00, 0x22, 0x44, 0x66, 0x88, 0xaa, 0xcc, 0xee
        };
        static const uint32_t steToRgb[16] =
        {
            0x00, 0x22, 0x44, 0x66, 0x88, 0xaa, 0xcc, 0xee,
            0x11, 0x33, 0x55, 0x77, 0x99, 0xbb, 0xdd, 0xff
        };

        bool isST = IsMachineST(machineType);
        const uint32_t* pPalette = isST ? stToRgb : steToRgb;
        uint32_t  r = (regValue >> 8) & 0xf;
        uint32_t  g = (regValue >> 4) & 0xf;
        uint32_t  b = (regValue >> 0) & 0xf;

        uint32_t colour = 0xff000000U;
        colour |= pPalette[r] << 16;
        colour |= pPalette[g] << 8;
        colour |= pPalette[b] << 0;
        result = colour;
    }
}
