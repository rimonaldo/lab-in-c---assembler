#include <stdlib.h>
#include <stdio.h>

#include "encoding.h"

/*------------- Encoding functions ------------- */
EncodedLine *encode_instruction_node(ASTNode *inst_node)
{

    EncodedLine encoded_line;
    printf("ENCODING LINE \n");
    return &encoded_line;
}

void encode_opcode(Opcode opcode, AddressingMode src_op_mode, AddressingMode dest_op_mode, EncodedLine *line)
{
    /*
    ----------- instructions encoding -----------
    0,1 => A(absolute),E(external),R(relocatable) => A=00, E=01, R=10
    ** only for encoded instructions, and their added words **
    ** not for directives and data **

    2,3 => addressing mode of src op

    4,5 => addressing mode of dest op

    6,7,8,9 => opcode (0-15) => [0000 , 1111]
    */

    printf("Encoding opcode: %d, src_op_mode: %d, dest_op_mode: %d\n", opcode, src_op_mode, dest_op_mode);
}

void encode_immediate_val(int val, EncodedLine *line)
{
    printf("Encoding immediate value: %d\n", val);
}
void encode_direct_label(char *label, int is_ext, EncodedLine *line)
{
    printf("Encoding direct label: %s, is_ext: %d\n", label, is_ext);
}
void encode_mat_access(char *label, int row_reg_num, int col_reg_num, int is_ext, EncodedLine *line)
{
    printf("Encoding matrix access: label=%s, row_reg_num=%d, col_reg_num=%d, is_ext=%d\n", label, row_reg_num, col_reg_num, is_ext);
}
void encode_register(int reg_num, EncodedLine *line)
{
    printf("Encoding register: reg_num=%d\n", reg_num);
}

/*------------- bit convertions functions ------------- */

/*------------- encoded list functions ------------- */

void free_encoded_line(EncodedLine *head)
{
    EncodedLine *current = head;
    EncodedLine *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
}