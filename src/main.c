#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common/errors/errors.h"
#include "stg_00_preprocessor/preprocessor.h"
#include "stg_01_first_pass/first_pass.h"

int main(int argc, char *argv[])
{
    /* Check if input file was provided */
    if (argc < 2)
    {
        /* Print usage message and exit with error code */
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    /* Get pointer to input filename from command-line argument */
    const char *input_filename = argv[1];

    StatusInfo *status_info = malloc(sizeof(StatusInfo));
    status_info->error_log = malloc(sizeof(ErrorInfo) * 10);
    status_info->capacity = 10;
    /*
     * Extract the basename of the file (without path):
     * If input is "path/to/file.asm", we want just "file.asm"
     * strrchr finds the last '/' in the string
     */
    const char *basename = strrchr(input_filename, '/');
    if (basename != NULL)
        basename++; /* Skip the '/' */
    else
        basename = input_filename;

    /*
     * Prepare destination buffer for the expanded file name:
     * This will become: "output/file.am"
     */
    char expanded_filename[1024];
    generate_expanded_filename(expanded_filename, sizeof(expanded_filename), basename);

    /* Run the pre-assembler on the original source file */
    run_pre_assembler(input_filename, status_info);

    print_errors(status_info);
    if (status_info->error_count > 0)
    {
        printf("Did not pass preprocessor stage\n");
        return 0;
    }

    /* Run the first pass on the preprocessed (".am") file */
    run_first_pass(expanded_filename, status_info);

    if (status_info->error_count > 0)
    {
        printf("Did not pass first_pass stage\n");
        return 0;
    }

    printf("------------Starting 2nd pass------------\n");
    /* Return success */
    return 0;
}

/*
 * Build the output file name with extension ".am" in the "output/" folder.
 * - 'dest' is the destination buffer
 * - 'dest_size' is the size of that buffer
 * - 'basename' is a filename like "file.asm" (not including directory)
 *
 * The function removes the extension and adds ".am" instead.
 */
void generate_expanded_filename(char *dest, size_t dest_size, const char *basename)
{
    /*
     * strrchr(basename, '.') returns pointer to last dot in filename
     * If there is a dot, we truncate before it.
     * Else, we take full length of basename.
     *
     * snprintf ensures we do not overflow the destination buffer.
     */
    snprintf(dest, dest_size, "output/%.*s.am",
             (int)(strrchr(basename, '.') ? strrchr(basename, '.') - basename : strlen(basename)),
             basename);
}
