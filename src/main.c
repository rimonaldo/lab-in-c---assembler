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

    const char *basename = strrchr(input_filename, '/');
    if (basename != NULL)
        basename++; /* Skip the '/' */
    else
        basename = input_filename;

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
    EncodedList *encoded_list = malloc(sizeof(EncodedList));
    if (!encoded_list)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    /* Initialize */
    encoded_list->size = 0;
    encoded_list->head = NULL;
    encoded_list->tail = NULL;

    int IC = 100;
    run_first_pass(expanded_filename, symbol_table, &ast_head, &IC, encoded_list, status_info);

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
        j++;
        if (curr_info->type == SYMBOL_DATA)
        {
            printf("%s\n", current->key);
            curr_info->address += ICF;
            printf("--> moving data symbol to data image\nnew address: %d\n\n", curr_info->address);
        }

        current = current->next;
    }
    /* close and release memory */

    if (status_info->error_count > 0)
    {
        int i; 

        printf("Error count: %d\n", status_info->error_count);
        printf("Warning count: %d\n", status_info->warning_count);
        printf("Did not pass first_pass stage\n");

        for (i = 0; i < status_info->error_count; i++)
        {
            if (status_info->error_log[i].sevirity == SEV_ERROR)
            {
                printf("\033[1;31mLine: %d: %s\033[0m\n",
                       status_info->error_log[i].line_number,
                       status_info->error_log[i].message); /* Red */
            }
            else if (status_info->error_log[i].sevirity == SEV_WARNING)
            {
                printf("\033[1;33mLine: %d: %s\033[0m\n",
                       status_info->error_log[i].line_number,
                       status_info->error_log[i].message); /* Yellow */
            }
            else
            {
                printf("Line: %d: %s\n",
                       status_info->error_log[i].line_number,
                       status_info->error_log[i].message);
            }
        }
        return 0;
    }
    printf("\033[1;32m------------ Starting 2nd pass ------------\033[0m\n\n");

    run_second_pass(symbol_table, &ast_head, encoded_list, status_info);

    return 0;
}


void generate_expanded_filename(char *dest, size_t dest_size, const char *basename)
{
    snprintf(dest, dest_size, "output/%.*s.am",
             (int)(strrchr(basename, '.') ? strrchr(basename, '.') - basename : strlen(basename)),
             basename);
}
