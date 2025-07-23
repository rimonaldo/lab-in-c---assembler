#include <stdlib.h>
#include <stdio.h>

#include "second_pass.h"

void run_second_pass(Table *symbol_table, ASTNode **ast_head, StatusInfo status_info)
{
    printf("second pass\n\n");
    ASTNode *curr_ast_node = *ast_head;
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
}