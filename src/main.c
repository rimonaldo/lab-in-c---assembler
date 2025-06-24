#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stg_00_preprocessor/preprocessor.h"
#include "stg_01_first_pass/first_pass.h"

int main(int argc, char *argv[])
{
    /* Check if input file argument is provided */
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    /* Get the input filename from command line */
    const char *input_filename = argv[1];

    /* Extract the base name (file name without path) */
    const char *basename = strrchr(input_filename, '/');
    basename = (basename != NULL) ? basename + 1 : input_filename;

    /* Prepare the expanded filename for the output of the preprocessor */
    char expanded_filename[1024];
    generate_expanded_filename(expanded_filename, sizeof(expanded_filename), basename);

    /* Run the pre-assembler stage */
    run_pre_assembler(input_filename);

    /* Run the first pass stage on the expanded file */
    run_first_pass(expanded_filename);

    return 0;
}

/* @brief
 * Generate the expanded filename by:
 * - Placing it in the "output/" directory
 * - Using the base name of the input file, but removing its extension (if any)
 * - Appending the ".am" extension
 * For example, "source.asm" becomes "output/source.am"
 */
void generate_expanded_filename(char *dest, size_t dest_size, const char *basename)
{

    snprintf(dest, dest_size, "output/%.*s.am",
             (int)(strrchr(basename, '.') ? strrchr(basename, '.') - basename : strlen(basename)),
             basename);
}