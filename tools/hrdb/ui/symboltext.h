#ifndef SYMBOLTEXT_H
#define SYMBOLTEXT_H

#include <QString>
#include "../models/symboltable.h"

// Shared function to format symbol strings e.g. with relative offsets
QString DescribeSymbol(const SymbolTable& table, uint32_t addr);

#endif // SYMBOLTEXT_H
