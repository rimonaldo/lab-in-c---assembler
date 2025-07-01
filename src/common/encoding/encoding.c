#include <stdlib.h>
#include <stdio.h>

#include "encoding.h"
#include "../AST/ast.h"
int inverse_idx(int bit)
{
    if (bit > 9 || bit < 0)
    {
        /* handle error */
        printf("INVALID BIT NUMBER\n");
    }

    return (bit - 9) * -1;
}

void write_bits(BinCode bincode, int val, int start_bit, int end_bit)
{
    if (start_bit > end_bit)
    {
        /* handle error */
        printf("INVALID: start bit: %d is larger than end bit: %d", start_bit, end_bit);
        return;
    }
    int num_bits = end_bit - start_bit + 1;

    int max_val = (1 << num_bits) - 1;

    if (val > max_val)
    {
        /* handle error */
        printf("INVALID VALUE: %d exceeds max allowed value %d for %d bits\n", val, max_val, num_bits);
        return;
    }
    int i;
    for (i = 0; i < num_bits; i++)
    {
        /* shift 'val' right by i to bring the i-th bit to position 0, then mask it */
        int bit_val = (val >> i) & 1;
        /* compute the logical bit number (e.g., 6,7,8...) */
        int target_bit = start_bit + i;
        /* convert to array index using inverse_idx, then assign the bit value as '0' or '1' */
        bincode[inverse_idx(target_bit)] = bit_val ? '1' : '0';
    }
}

/*
   typedef struct EncodedLine
   {
   ASTNode *ast_node;
   int decimal_address;
   char base4_address[5];
   BinCode words[5];
   int words_count;
   struct EncodedLine *next;
   } EncodedLine;
   */

/*------------- Encoding functions ------------- */
EncodedLine *encode_instruction_line(ASTNode *inst_node, int leader_idx)
{
    printf("----------- ENCODING LINE ----------- \n");

    EncodedLine *encoded_line = malloc(sizeof(EncodedLine));
    Opcode opcode = inst_node->content.instruction.opcode;
    AddressingMode src_ad_mod = NONE;
    AddressingMode dest_ad_mod = NONE;
    int i;

    if (inst_node->content.instruction.src_op.mode != NONE)
    {
        src_ad_mod = inst_node->content.instruction.src_op.mode;
    }

    if (inst_node->content.instruction.dest_op.mode != NONE)
    {
        dest_ad_mod = inst_node->content.instruction.dest_op.mode;
    }

    int num_exp_ops;
    num_exp_ops = expect_operands(opcode);

    for (i = 0; i < 10; i++)
    {
        encoded_line->words[0][i] = '0';
    }

    switch (num_exp_ops)
    {
    case 0:
    {
        /* without src or dest ops bits */
        encode_opcode(opcode, NONE, NONE, encoded_line);
    }
    break;
    case 1:
    {
        /* only dest op bits */
        encode_opcode(opcode, NONE, dest_ad_mod, encoded_line);
    }
    break;
    case 2:
    {
        /* both src and dest ops bits */
        encode_opcode(opcode, src_ad_mod, dest_ad_mod, encoded_line);
    }
    break;
    }

    /* Print the bincode for debugging */
    printf("opcode bincode: ");
    for (i = 0; i < 10; i++)
    {
        printf("%c", encoded_line->words[0][i]);
    }
    printf("\n");
    return encoded_line;
}

void encode_opcode(Opcode opcode, AddressingMode src_op_mode, AddressingMode dest_op_mode, EncodedLine *line)
{
    BinCode *bincode = line->words[0];

    printf("src_ad_mod: %s, dest_ad_mod: %s\n", get_ad_mod_name(src_op_mode), get_ad_mod_name(dest_op_mode));
    assemble_AER(bincode, 0);
    assemble_src_op_mod(bincode, src_op_mode);
    assemble_dest_op_mod(bincode, dest_op_mode);
    assemble_opcode(bincode, opcode);
    
}

/*--------------- encode opcode helper functions ---------------*/
void assemble_AER(BinCode bincode, int AER)
{

    switch (AER)
    {
    case 0:
        write_bits(bincode, 0, 0, 1);
        break;
    case 1:
        write_bits(bincode, 1, 0, 1);
        break;
    case 2:
        write_bits(bincode, 2, 0, 1);
        break;
    }
}

void assemble_src_op_mod(BinCode bincode, AddressingMode mode)
{
    switch (mode)
    {
    case IMMEDIATE:
        write_bits(bincode, 0, 2, 3);
        break;
    case DIRECT:
        write_bits(bincode, 1, 2, 3);
        break;
    case REGISTER:
        write_bits(bincode, 2, 2, 3);
        break;
    case MAT_ACCESS:
        write_bits(bincode, 3, 2, 3);
        break;
    case NONE:
        write_bits(bincode, 0, 2, 3);
        break;
    }
}

void assemble_dest_op_mod(BinCode bincode, AddressingMode mode)
{

    switch (mode)
    {
    case IMMEDIATE:
        write_bits(bincode, 0, 4, 5);
        break;
    case DIRECT:
        write_bits(bincode, 1, 4, 5);
        break;
    case REGISTER:
        write_bits(bincode, 2, 4, 5);
        break;
    case MAT_ACCESS:
        write_bits(bincode, 3, 4, 5);
        break;
    case NONE:
        write_bits(bincode, 0, 4, 5);
        break;
    }
}

void assemble_opcode(BinCode bincode, Opcode opcode)
{
    write_bits(bincode, opcode, 6, 9);
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