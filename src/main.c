#include <stdlib.h>
#include <stdio.h>
#include "stg_00_preprocessor/preprocessor.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    return run_pre_assembler(argv[1]);
}