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
    while (curr_encoded_line->next)
    {
        ASTNode *curr_ast_node = curr_encoded_line->ast_node;

        /* INSTRUCTION STATEMENT HANDLING */
        if (curr_ast_node->type == INSTRUCTION_STATEMENT)
        {
            AddressingMode src_addressing_mode = curr_ast_node->content.instruction.src_op.mode;
            AddressingMode dest_addressing_mode = curr_ast_node->content.instruction.dest_op.mode;
            char *label_name[MAX_LABEL_SIZE];
            int i;
            printf("line: %d\n", curr_ast_node->line_number);
            if (src_addressing_mode == DIRECT || src_addressing_mode == MAT_ACCESS)
            {
                printf("\n--> src operand\n");
                strcpy(label_name, curr_ast_node->content.instruction.src_op.value.label);
                void *data = table_lookup(symbol_table, label_name);
                SymbolInfo *symbol_info = (SymbolInfo *)data;
                int address = symbol_info->address;
                if (src_addressing_mode == MAT_ACCESS)
                    printf("src operand is mat access.\nlabel name: %s\naddress: %d\n", label_name, address);
                else
                    printf("src operand is directly addresed.\nlabel name: %s\naddress: %d\n", label_name, address);

                for (i = 0; i < curr_encoded_line->words_count; i++)
                {
                    if (curr_encoded_line->is_waiting_words[i] == 1)
                    {
                        int AER;
                        AER = symbol_info->type == SYMBOL_EXTERN ? 1 : 2;
                        write_bits(curr_encoded_line->words[i], AER, 0, 1);
                        write_bits(curr_encoded_line->words[i], symbol_info->address, 2, 9);
                        printf("encoded label for symbol %s : ", label_name);
                        print_bincode(curr_encoded_line->words[i]);
                        printf("\n");
                        break;
                    }
                }
            }

            if (dest_addressing_mode == DIRECT || dest_addressing_mode == MAT_ACCESS)
            {
                printf("\n--> dest operand\n");
                strcpy(label_name, curr_ast_node->content.instruction.dest_op.value.label);
                void *data = table_lookup(symbol_table, label_name);
                SymbolInfo *symbol_info = (SymbolInfo *)data;
                int address = symbol_info->address;
                if (dest_addressing_mode == MAT_ACCESS)
                    printf("dest operand is mat access.\nlabel name: %s\naddress: %d\n", label_name, address);
                else
                    printf("dest operand is directly addresed.\nlabel name: %s\naddress: %d\n", label_name, address);

                for (i = i; i < curr_encoded_line->words_count; i++)
                {
                    if (curr_encoded_line->is_waiting_words[i] == 1)
                    {
                        int AER;
                        AER = symbol_info->type == SYMBOL_EXTERN ? 1 : 2;
                        write_bits(curr_encoded_line->words[i], AER, 0, 1);
                        write_bits(curr_encoded_line->words[i], symbol_info->address, 2, 9);
                        printf("encoded label for symbol %s : ", label_name);
                        print_bincode(curr_encoded_line->words[i]);
                        printf("\n");
                        break;
                    }
                }
            }

            ins_word_count += curr_encoded_line->words_count;
            printf("______________________\n");

            curr_encoded_line = curr_encoded_line->next;
        }
        else if (curr_ast_node->type == DIRECTIVE_STATEMENT)
        {
        }

        /*

        ASTNode *curr_ast_node = curr_encoded_line->ast_node;
        char *label_name;

        if (curr_ast_node->content.instruction.src_op.mode == DIRECT)
        {
            label_name = curr_ast_node->content.instruction.dest_op.value.label;
        }
        void *data = table_lookup(symbol_table, label_name);
        SymbolInfo *symbol_info = (SymbolInfo *)data;
        int i;
        for (i = 0; i < curr_encoded_line->words_count; i++)
        {
            if (curr_encoded_line->is_waiting_words[i] == 1)
            {

                fprintf(fp, "%d %s\n", symbol_info->address, curr_encoded_line->words[i]);
            }
        }

        curr_encoded_line = curr_encoded_line->next;
    }
    */
    }

    /* instruction count : data count*/
    FILE *fp = fopen("output/prog1.ob", "w");
    fprintf(fp, "%d %d\n", 43, 50);
    fclose(fp);
}
