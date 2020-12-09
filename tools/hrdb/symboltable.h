#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "stdint.h"
#include <string>
#include <vector>
#include <map>

struct Symbol
{
    Symbol() : address(0) {}
    // Create internal symbols
    Symbol(std::string name, uint32_t address);

    std::string name;
    uint32_t address;
    std::string type;
};

class SymbolTable
{
public:
    SymbolTable();
    void Add(const Symbol& sym);
    void AddComplete();

    size_t Count() const { return m_symbols.size(); }
    bool Find(uint32_t address, Symbol& result) const;
    bool FindLowerOrEqual(uint32_t address, Symbol& result) const;

    int m_userSymbolCount;
private:
    std::vector<Symbol> m_symbols;

    typedef std::pair<uint32_t, size_t> Pair;
    typedef std::map<uint32_t, size_t> Map;
    typedef std::vector<Pair>   Keys;

    Map m_addrLookup;
    Keys m_addrKeys;    // sorted address keys
};

#endif // SYMBOLTABLE_H
