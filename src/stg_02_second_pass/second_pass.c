#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "second_pass.h"
#define MAX_LABEL_SIZE 32
void run_second_pass(Table *symbol_table, ASTNode **ast_head, EncodedList *encoded_list, StatusInfo status_info)
{
    printf("second pass\n\n");
    EncodedLine *curr_encoded_line = encoded_list->head;

    int ins_word_count = 0;
    int data_word_count = 0;
    while (curr_encoded_line)
    {
        ASTNode *curr_ast_node = curr_encoded_line->ast_node;
        printf("line: %d\n", curr_ast_node->line_number);
        /* INSTRUCTION STATEMENT HANDLING */
        if (curr_ast_node->type == INSTRUCTION_STATEMENT)
        {
            AddressingMode src_addressing_mode = curr_ast_node->content.instruction.src_op.mode;
            AddressingMode dest_addressing_mode = curr_ast_node->content.instruction.dest_op.mode;
            char *label_name[MAX_LABEL_SIZE];
            int i = 0;

            /*--- Source operand ---*/
            if (src_addressing_mode == DIRECT || src_addressing_mode == MAT_ACCESS)
            {
                printf("\n--> src operand\n");
                strcpy(label_name, curr_ast_node->content.instruction.src_op.value.label);
                SymbolInfo *symbol_info = table_lookup(symbol_table, label_name);
                int address = symbol_info->address;

                if (src_addressing_mode == MAT_ACCESS)
                    printf("src operand is mat access.\nlabel name: %s\naddress: %d\n", label_name, address);
                else
                    printf("src operand is directly addressed.\nlabel name: %s\naddress: %d\n", label_name, address);

                while (i < curr_encoded_line->words_count && curr_encoded_line->is_waiting_words[i] != 1)
                    i++;

                if (i < curr_encoded_line->words_count)
                {
                    int AER = (symbol_info->type == SYMBOL_EXTERN) ? 1 : 2;
                    write_bits(curr_encoded_line->words[i], AER, 0, 1);
                    write_bits(curr_encoded_line->words[i], symbol_info->address, 2, 9);
                    printf("encoded label for symbol %s : ", label_name);
                    print_bincode(curr_encoded_line->words[i]);
                    printf("\n");
                    ins_word_count++;
                    i++;
                }
            }
            /*--- Destination operand ---*/
            if (dest_addressing_mode == DIRECT || dest_addressing_mode == MAT_ACCESS)
            {
                printf("\n--> dest operand\n");
                strcpy(label_name, curr_ast_node->content.instruction.dest_op.value.label);
                SymbolInfo *symbol_info = table_lookup(symbol_table, label_name);
                int address = symbol_info->address;

                if (dest_addressing_mode == MAT_ACCESS)
                    printf("dest operand is mat access.\nlabel name: %s\naddress: %d\n", label_name, address);
                else
                    printf("dest operand is directly addressed.\nlabel name: %s\naddress: %d\n", label_name, address);

                while (i < curr_encoded_line->words_count && curr_encoded_line->is_waiting_words[i] != 1)
                    i++;

                if (i < curr_encoded_line->words_count)
                {
                    int AER = (symbol_info->type == SYMBOL_EXTERN) ? 1 : 2;
                    write_bits(curr_encoded_line->words[i], AER, 0, 1);
                    write_bits(curr_encoded_line->words[i], symbol_info->address, 2, 9);
                    printf("encoded label for symbol %s : ", label_name);
                    print_bincode(curr_encoded_line->words[i]);
                    printf("\n");
                    i++;
                }
            }

            ins_word_count += curr_encoded_line->words_count;

            curr_encoded_line = curr_encoded_line->next;
        }
        else if (curr_ast_node->type == DIRECTIVE_STATEMENT)
        {
            int i;
            int data_size = curr_ast_node->content.directive.params.data.size;
            for (i = 0; i < data_size; i++)
            {
                data_word_count++;
            }
            curr_encoded_line = curr_encoded_line->next;
        }
        printf("______________________\n");
    }
    curr_encoded_line = encoded_list->head;
    /* instruction count : data count*/
    char buffer[10000];
    FILE *fp = fopen("output/prog1.ob", "w");
    fprintf(fp, "%d\t%d\n", ins_word_count, data_word_count);
    fputs(buffer, fp);
    int i;

    int address = 100;
    curr_encoded_line = encoded_list->head;
    while (curr_encoded_line)
    {
        int bit;
        if (curr_encoded_line->ast_node->type == INSTRUCTION_STATEMENT)
        {
            for (i = 0; i < curr_encoded_line->words_count; i++)
            {
                fprintf(fp, "%03d\t", address);
                fprintf(fp, "%s", curr_encoded_line->words[i]);
                fprintf(fp, "\n");
                address++;
            }
        }
        else if (curr_encoded_line->ast_node->type == DIRECTIVE_STATEMENT)
        {
            int data_size = curr_encoded_line->ast_node->content.directive.params.data.size;
            for (i = 0; i < data_size; i++)
            {
                fprintf(fp, "%03d\t", address);
                fprintf(fp, "%s", curr_encoded_line->data_words[i]);
                fprintf(fp, "\n");
                address++;
            }
        }

        curr_encoded_line = curr_encoded_line->next;
    }

    fclose(fp);
}
