#ifndef PROFILEDATA_H
#define PROFILEDATA_H

#include <stdint.h>
#include <map>

struct ProfileDelta
{
    uint32_t addr;
    uint32_t count;
    uint32_t cycles;
};

class ProfileData
{
public:
    struct Entry
    {
        uint32_t count;
        uint32_t cycles;
    };

    void Add(const ProfileDelta& delta);
    void Get(uint32_t addr, uint32_t& count, uint32_t& cycles);
    void Reset();

    typedef std::map<uint32_t, Entry> Map;
    typedef std::pair<uint32_t, Entry> Pair;

    Map         m_entries;
};

#endif // PROFILEDATA_H
