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
    status_info->warning_count = 0;
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
    ASTNode *ast_head = NULL;
    Table *symbol_table = table_create();
    int IC = 100;
    run_first_pass(expanded_filename, symbol_table, &ast_head, &IC, status_info);

    /* update data memory locations */
    TableNode *current = symbol_table->head;
    SymbolInfo *curr_info = (SymbolInfo *)current;
    int ICF = IC;
    int j = 1;
    while (current->next)
    {
        void *data_ptr = current->data;
        curr_info = (SymbolInfo *)data_ptr;
        strcpy(((SymbolInfo *)current->data)->name, current->key);
        printf("%d\n", j++);
        if (curr_info->type == SYMBOL_DATA)
        {
            curr_info->address += ICF;
            printf("new addy: %d\n", curr_info->address);
        }

        current = current->next;
    }
    /* close and release memory */
    if (status_info->error_count > 0)
    {
        printf("Did not pass first_pass stage\n");
        printf("Error count: %d\n", status_info->error_count);
        printf("Warning count: %d\n", status_info->warning_count);
        int i = 0;
        for (i = 0; i < status_info->error_count; i++)
        {
            printf("Line: %d\n", status_info->error_log[i].line_number);
        }
        return 0;
    }
    printf("------------Starting 2nd pass------------\n");

    run_second_pass(symbol_table, &ast_head, status_info);
    /* fill addresses */
    /* traverse ast nodes.
        when operand is directly addressed
        look up in symbol table
        found ? encode [address and ARE] : write error */

    /* create .ent .ext and .obj */

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
