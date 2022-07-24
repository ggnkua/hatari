#include "profiledata.h"

void ProfileData::Add(const ProfileDelta &delta)
{
    Map::iterator it = m_entries.find(delta.addr);
    if (it != m_entries.end())
    {
        it->second.count += delta.count;
        it->second.cycles += delta.cycles;
    }
    else
    {
        Pair p;
        p.first = delta.addr;
        p.second.count = delta.count;
        p.second.cycles = delta.cycles;
        m_entries.insert(p);
    }
}

void ProfileData::Get(uint32_t addr, uint32_t& count, uint32_t& cycles)
{
    Map::iterator it = m_entries.find(addr);
    if (it != m_entries.end())
    {
        count = it->second.count;
        cycles = it->second.cycles;
    }
    else
    {
        count = cycles = 0;
    }
}

void ProfileData::Reset()
{
    m_entries.clear();
}
