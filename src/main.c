#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common/errors/errors.h"
#include "stg_00_preprocessor/preprocessor.h"
#include "stg_01_first_pass/first_pass.h"

void generate_expanded_filename(char *dest, size_t dest_size, const char *basename);
void generate_output_files(EncodedList *encoded_list, Table *symbol_table);

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

    /* update data memory locations, count words */
    TableNode *current = symbol_table->head;
    SymbolInfo *curr_info = (SymbolInfo *)current;
    int ICF = IC;
    int j = 1;
    int instruction_word_count = 0;
    int data_word_count = 0;
    while (current && current->next)
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
    {
        EncodedLine *el = encoded_list->head;
        while (el)
        {
            ASTNode *node = el->ast_node;
            if (node->type == INSTRUCTION_STATEMENT)
            {
                instruction_word_count += el->words_count;
            }
            else if (node->type == DIRECTIVE_STATEMENT)
            {
                int data_size = node->content.directive.params.data.size;
                data_word_count += data_size;
            }
            el = el->next;
        }
    }

    /* CHECK ERROR LOG */
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

    /* CHECK ERROR LOG */
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

    generate_output_files(encoded_list, symbol_table);

    return 0;
}

void generate_expanded_filename(char *dest, size_t dest_size, const char *basename)
{
    snprintf(dest, dest_size, "output/%.*s.am",
             (int)(strrchr(basename, '.') ? strrchr(basename, '.') - basename : strlen(basename)),
             basename);
}

void generate_output_files(EncodedList *encoded_list, Table *symbol_table)
{
    /* Open output .ob (always needed) */
    FILE *fp = fopen("output/prog1.ob", "w");
    if (!fp) {
        fprintf(stderr, "Error opening output/prog1.ob\n");
        return;
    }

    /* ent/ext files will be opened conditionally */
    FILE *ent_file = NULL;
    FILE *ext_file = NULL;

    /* ---- Reserve header: fixed width so overwrite won't shift lines ---- */
    /* Keep the SAME format for the final overwrite */
    fprintf(fp, "%10d\t%10d\n", 0, 0);

    int instruction_word_count = 0;
    int data_word_count = 0;

    /* output .ob and log externs using address counter */
    int address = 100;
    EncodedLine *curr_encoded_line = encoded_list->head;
    while (curr_encoded_line)
    {
        ASTNode *node = curr_encoded_line->ast_node;
        char base4_add[5];
        char base4_code[5];

        if (node->type == INSTRUCTION_STATEMENT)
        {
            int i;
            for (i = 0; i < curr_encoded_line->words_count; i++)
            {
                addr_to_base4(address, base4_add);
                int code_to_write = bincode_to_int(curr_encoded_line->words[i]);
                bincode_to_base4(code_to_write, base4_code);
                fprintf(fp, "%03s\t%s\n", base4_add, base4_code);

                /* If this word is an extern reference, log it (open .ext lazily) */
                if (curr_encoded_line->is_waiting_words[i] == 2)
                {
                    if (!ext_file)
                    {
                        ext_file = fopen("output/prog1.ext", "w");
                        if (!ext_file) {
                            fprintf(stderr, "Error opening output/prog1.ext\n");
                            /* If we cannot open .ext, just skip writing externs */
                        }
                    }

                    if (ext_file)
                    {
                        char *symbol_name = NULL;
                        if (i == 0)
                            symbol_name = node->content.instruction.src_op.value.label;
                        else
                            symbol_name = node->content.instruction.dest_op.value.label;

                        fprintf(ext_file, "%s\t%s\n", symbol_name, base4_add);
                    }
                }

                instruction_word_count++;
                address++;
            }
        }
        else if (node->type == DIRECTIVE_STATEMENT)
        {
            int data_size = node->content.directive.params.data.size;
            int i;
            for (i = 0; i < data_size; i++)
            {
                addr_to_base4(address, base4_add);
                int code_to_write = bincode_to_signed(curr_encoded_line->data_words[i]);
                bincode_to_signed_base4(code_to_write, base4_code);
                fprintf(fp, "%s\t%s\n", base4_add, base4_code);
                data_word_count++;
                address++;
            }
        }

        curr_encoded_line = curr_encoded_line->next;
    }

    /* Write .ent only if there are entry labels (open lazily on first one) */
    {
        TableNode *current_node = symbol_table->head;
        while (current_node)
        {
            SymbolInfo *info = (SymbolInfo *)current_node->data;
            if (info->is_entry == 1)
            {
                if (!ent_file)
                {
                    ent_file = fopen("output/prog1.ent", "w");
                    if (!ent_file) {
                        fprintf(stderr, "Error opening output/prog1.ent\n");
                        /* If cannot open, don't attempt further writes */
                        break;
                    }
                }

                if (ent_file)
                {
                    char base4_add[5];
                    addr_to_base4(info->address, base4_add);
                    fprintf(ent_file, "%s\t%s\n", info->name, base4_add);
                }
            }
            current_node = current_node->next;
        }
    }

    /* ---- Overwrite header in place (no shifting) ---- */
    fflush(fp);                /* ensure body is on disk before seeking */
    fseek(fp, 0, SEEK_SET);   /* go back to file start */
    fprintf(fp, "%10d\t%10d\n", instruction_word_count, data_word_count);

    fclose(fp);
    if (ent_file) fclose(ent_file);
    if (ext_file) fclose(ext_file);
}
