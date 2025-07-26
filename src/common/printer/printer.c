#include "../../stg_01_first_pass/first_pass.h"

void print_symbol(const char *key, void *data)
{
    SymbolInfo *info = (SymbolInfo *)data;

    printf("  [%s] Address: %d, Type: ",
           key,
           info->address);

    switch (info->type)
    {
    case SYMBOL_CODE:
        printf("CODE");
        break;
    case SYMBOL_DATA:
        printf("DATA");
        break;
    case SYMBOL_EXTERN:
        printf("EXTERN");
        break;
    case SYMBOL_ENTRY:
        printf("ENTRY");
        break;
    default:
        printf("UNKNOWN");
        break;
    }
    printf("\n");
}

void print_extern(const char *key, void *data)
{
    if (!key || !data)
    {
        printf("Invalid extern data\n");
        return;
    }
    int address = *(int *)data;
    printf("  [EXTERN] %s @ %d\n", key, address);
}

void print_entry(const char *key, void *data)
{
    if (!key || !data)
    {
        printf("Invalid extern data\n");
        return;
    }
    int address = *(int *)data;
    printf("  [ENTRY] %s @ %d\n", key, address);
}
