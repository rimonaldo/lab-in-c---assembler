#include <stdlib.h>
#include <stdio.h>

#include "second_pass.h"

void run_second_pass(Table *symbol_table, ASTNode **ast_head, EncodedList *encoded_list, StatusInfo status_info)
{
    printf("second pass\n\n");
    EncodedLine *curr_encoded_line = encoded_list->head;
    while (curr_encoded_line->next)
    {
        ASTNode *curr_ast_node = curr_encoded_line->ast_node;
        if (curr_ast_node->type == INSTRUCTION_STATEMENT)
        {
            if (curr_ast_node->content.instruction.src_op.mode == DIRECT)
            {
                printf("line: %d\n", curr_ast_node->line_number);
                printf("src operand is directly addresed.\nlabel name: %s\naddress: %d\n", curr_ast_node->content.instruction.src_op.value.label, 100);
                printf("_____________________________\n");
            }

            if (curr_ast_node->content.instruction.dest_op.mode == DIRECT)
            {
                char *label_name = curr_ast_node->content.instruction.dest_op.value.label;
                void *data = table_lookup(symbol_table, label_name);
                SymbolInfo *symbol_info = (SymbolInfo *)data;
                printf("line: %d\n", curr_ast_node->line_number);
                int address = symbol_info->address;
                printf("src operand is directly addresed.\nlabel name: %s\naddress: %d\n", label_name, address);
                int i = 0;
                for (i = 0; i < curr_encoded_line->words_count; i++)
                {
                    if (curr_encoded_line->is_waiting_words[i] == 1)
                    {
                        int AER;
                        AER = symbol_info->type == SYMBOL_EXTERN ? 1 : 2;
                        write_bits(curr_encoded_line->words[i], AER, 0, 1);
                        write_bits(curr_encoded_line->words[i], symbol_info->address, 2, 9);
                        printf("encoded label: ");
                        print_bincode(curr_encoded_line->words[i]);
                        printf("\n");
                    }
                }
                printf("_____________________________\n");
            }
        }
        curr_encoded_line = curr_encoded_line->next;
    }

    /*
    while (curr_ast_node->next)
    {

    if (curr_ast_node->type == INSTRUCTION_STATEMENT)
        {
            if (curr_ast_node->content.instruction.src_op.mode == DIRECT)
            {
                printf("line: %d\n", curr_ast_node->line_number);
                printf("src operand is directly addresed.\nlabel name: %s\naddress: %d\n", curr_ast_node->content.instruction.src_op.value.label, 100);
                printf("_____________________________\n");
            }

            if (curr_ast_node->content.instruction.dest_op.mode == DIRECT)
            {
                char *label_name = curr_ast_node->content.instruction.dest_op.value.label;
                void *data = table_lookup(symbol_table, label_name);
                SymbolInfo *symbol_info = (SymbolInfo *)data;
                printf("line: %d\n", curr_ast_node->line_number);
                int address = symbol_info->address;
                printf("src operand is directly addresed.\nlabel name: %s\naddress: %d\n", label_name, address);

                printf("_____________________________\n");
            }
        }
        curr_ast_node = curr_ast_node->next;
    }
    */
}