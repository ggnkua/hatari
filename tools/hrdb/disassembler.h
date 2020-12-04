#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <vector>
#include <QTextStream>

#include "hopper/instruction.h"
#include "hopper/buffer.h"

class Disassembler
{
public:
    struct line
    {
        uint32_t    address;
        instruction inst;
    };

    // ----------------------------------------------------------------------------
    //	HIGHER-LEVEL DISASSEMBLY CREATION
    // ----------------------------------------------------------------------------
    // Storage for an attempt at tokenising the memory
    class disassembly
    {
    public:
        std::vector<line>    lines;
    };

    static int decode_buf(buffer_reader& buf, disassembly& disasm, uint32_t address);
    static void print(const instruction& inst, /*const symbols& symbols, */ uint32_t inst_address, QTextStream& ref);
};

#endif // DISASSEMBLER_H