#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "second_pass.h"

#define MAX_LABEL_SIZE 32

/* Convert unsigned 8-bit address (0–255) into 4-char base-4 string using 'a'–'d' */
void addr_to_base4(unsigned char value, char out[5])
{
    static const char map[4] = {'a', 'b', 'c', 'd'};
    int i;
    for (i = 0; i < 4; ++i)
    {
        int shift = 6 - 2 * i;
        out[i] = map[(value >> shift) & 0x3];
    }
    out[4] = '\0';
}

void bincode_to_base4(signed char value, char out[4])
{
    static const char map[4] = {'a', 'b', 'c', 'd'};
    unsigned short bits = (unsigned short)value;

    int i;
    for (i = 0; i < 5; ++i)
    {
        int shift = 8 - 2 * i;
        out[i] = map[(bits >> shift) & 0x3];
    }
    out[5] = '\0';
}

void run_second_pass(Table *symbol_table, ASTNode **ast_head, EncodedList *encoded_list, StatusInfo status_info)
{
    printf("second pass\n\n");

    EncodedLine *curr_encoded_line = encoded_list->head;
    int ins_word_count = 0;
    int data_word_count = 0;

    /* encode label operands and count instruction/data words */
    while (curr_encoded_line)
    {
        ASTNode *curr_ast_node = curr_encoded_line->ast_node;
        /*printf("line: %d\n", curr_ast_node->line_number);*/

        if (curr_ast_node->type == INSTRUCTION_STATEMENT)
        {
            AddressingMode src_addressing_mode = curr_ast_node->content.instruction.src_op.mode;
            AddressingMode dest_addressing_mode = curr_ast_node->content.instruction.dest_op.mode;

            char label_name[MAX_LABEL_SIZE];
            int i = 0;

            /* --- Source operand --- */
            if (src_addressing_mode == DIRECT || src_addressing_mode == MAT_ACCESS)
            {
                strcpy(label_name, curr_ast_node->content.instruction.src_op.value.label);
                SymbolInfo *symbol_info = table_lookup(symbol_table, label_name);

                while (i < curr_encoded_line->words_count && curr_encoded_line->is_waiting_words[i] != 1)
                    i++;

                if (i < curr_encoded_line->words_count)
                {
                    int AER = (symbol_info->type == SYMBOL_EXTERN) ? 1 : 2;
                    write_bits(curr_encoded_line->words[i], AER, 0, 1);
                    write_bits(curr_encoded_line->words[i], symbol_info->address, 2, 9);

                    /* External symbol reference logged later when we know its address */
                    if (symbol_info->type == SYMBOL_EXTERN)
                        curr_encoded_line->is_waiting_words[i] = 2; /* mark for ext logging */

                    ins_word_count++;
                    i++;
                }
            }

            /* --- Destination operand --- */
            if (dest_addressing_mode == DIRECT || dest_addressing_mode == MAT_ACCESS)
            {
                strcpy(label_name, curr_ast_node->content.instruction.dest_op.value.label);
                SymbolInfo *symbol_info = table_lookup(symbol_table, label_name);

                while (i < curr_encoded_line->words_count && curr_encoded_line->is_waiting_words[i] != 1)
                    i++;

                if (i < curr_encoded_line->words_count)
                {
                    int AER = (symbol_info->type == SYMBOL_EXTERN) ? 1 : 2;
                    write_bits(curr_encoded_line->words[i], AER, 0, 1);
                    write_bits(curr_encoded_line->words[i], symbol_info->address, 2, 9);

                    if (symbol_info->type == SYMBOL_EXTERN)
                        curr_encoded_line->is_waiting_words[i] = 2; /* mark for ext logging */

                    i++;
                }
            }

            ins_word_count += curr_encoded_line->words_count;
        }
        else if (curr_ast_node->type == DIRECTIVE_STATEMENT)
        {
            int data_size = curr_ast_node->content.directive.params.data.size;
            data_word_count += data_size;
        }

        /*printf("______________________\n");*/
        curr_encoded_line = curr_encoded_line->next;
    }

    /* Open output files */
    FILE *fp = fopen("output/prog1.ob", "w");
    FILE *ent_file = fopen("output/prog1.ent", "w");
    FILE *ext_file = fopen("output/prog1.ext", "w");
    if (!fp || !ent_file || !ext_file)
    {
        fprintf(stderr, "Error opening output files\n");
        return;
    }

    /* Write header to .ob */
    fprintf(fp, "%d\t%d\n", ins_word_count, data_word_count);

    /* output .ob and log externs using address counter */
    int address = 100;
    curr_encoded_line = encoded_list->head;
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
                bincode_to_base4((signed char)curr_encoded_line->words[i], base4_code);
                fprintf(fp, "%03s\t%s\n", base4_add, base4_code);

                /* If this word is an extern reference, log it */
                if (curr_encoded_line->is_waiting_words[i] == 2)
                {
                    /* Recover symbol name for extern */
                    char *symbol_name = NULL;
                    if (i == 0)
                        symbol_name = node->content.instruction.src_op.value.label;
                    else
                        symbol_name = node->content.instruction.dest_op.value.label;

                    fprintf(ext_file, "%s\t%s\n", symbol_name, base4_add);
                }

                address++;
            }
        }
        else if (node->type == DIRECTIVE_STATEMENT)
        {
            char base4_add[5];
            char base4_code[5];
            int data_size = node->content.directive.params.data.size;
            int i;
            for (i = 0; i < data_size; i++)
            {
                addr_to_base4(address, base4_add);
                bincode_to_base4((signed char)curr_encoded_line->words[i], base4_code);
                fprintf(fp, "%s\t%s\n", base4_add, base4_code);
                address++;
            }
        }

        curr_encoded_line = curr_encoded_line->next;
    }

    /* Third pass: write .ent from symbol table */
    TableNode *current = symbol_table->head;
    while (current)
    {
        char base4_add[5];
        SymbolInfo *info = (SymbolInfo *)current->data;
        if (info->is_entry == 1)
        {

            addr_to_base4(info->address, base4_add);
            fprintf(ent_file, "%s\t%s\n", info->name, base4_add);
        }
        current = current->next;
    }

    fclose(fp);
    fclose(ent_file);
    fclose(ext_file);

    printf("Second pass complete. Files generated: prog1.ob, prog1.ent, prog1.ext\n");
}