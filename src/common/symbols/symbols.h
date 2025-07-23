#ifndef SYMBOLS_H
#define SYMBOLS_H

typedef enum
{
    SYMBOL_DATA,
    SYMBOL_CODE,
    SYMBOL_EXTERN,
    SYMBOL_ENTRY
} SymbolType;

typedef struct SymbolInfo
{
    char name[31];
    int address;
    SymbolType type;
    int is_entry;
    int is_extern;
} SymbolInfo;

#endif