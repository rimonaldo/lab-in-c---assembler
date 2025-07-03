#include <stdlib.h>
#include <stdio.h>

#include "encoding.h"
#include "../AST/ast.h"

void init_words(BinCode words[5], int count)
{
    int i, j;
    for (i = 0; i < count; i++)
    {
        for (j = 0; j < 10; j++)
        {
            words[i][j] = '0';
        }
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

int inverse_idx(int bit)
{
    if (bit > 9 || bit < 0)
    {
        /* handle error */
        printf("INVALID BIT NUMBER\n");
    }

    return 9 - bit;
}

/* Define a function pointer type for encoding operands:
   - returns void
   - receives: AddressingMode, pointer to word index (for incrementing when needed), and encoded line */
typedef void (*EncodeFunc)(AddressingMode mode, int *word_idx, EncodedLine *line, int is_src);

void encode_mat_access(AddressingMode mode, int *word_idx, EncodedLine *line, int is_src)
{

    printf("Waiting for address for matrix label: %s\n", line->ast_node->content.instruction.src_op.value.label);
    printf("encoding registers word only.\n");
    encode_operand(mode, word_idx, line, is_src);
}

void encode_single(AddressingMode mode, int *added_word_idx, EncodedLine *line, int is_src)
{
    printf("Encoding single operand at word index %d, mode: %s\n", *added_word_idx, get_ad_mod_name(mode));
    encode_operand(mode, added_word_idx, line, is_src);
}

void encode_register_pair(int *word_idx, EncodedLine *line)
{
    int src_reg_num = line->ast_node->content.instruction.src_op.value.reg_num;
    int dest_reg_num = line->ast_node->content.instruction.dest_op.value.reg_num;
    write_bits(line->words[*word_idx], src_reg_num, 6, 9);
    write_bits(line->words[*word_idx], dest_reg_num, 2, 5);
}

EncodeFunc encoders[] = {
    [IMMEDIATE] = encode_single,
    [DIRECT] = encode_single,
    [REGISTER] = encode_single,
    [MAT_ACCESS] = encode_mat_access,
};

void write_bits(BinCode bincode, int val, int start_bit, int end_bit)
{
    int num_bits = end_bit - start_bit + 1;
    int max_val = (1 << num_bits) - 1;
    int i;
    int is_src = 0;

    if (start_bit > end_bit)
    {
        /* handle error */
        printf("INVALID: start bit: %d is larger than end bit: %d", start_bit, end_bit);
        return;
    }

    if (val > max_val)
    {
        /* handle error */
        printf("INVALID VALUE: %d exceeds max allowed value %d for %d bits\n", val, max_val, num_bits);
        return;
    }

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

/*------------- Encoding functions ------------- */
EncodedLine *encode_instruction_line(ASTNode *inst_node, int leader_idx)
{
    printf("----------- ENCODING LINE ----------- \n");

    EncodedLine *encoded_line = malloc(sizeof(EncodedLine));
    encoded_line->words_count = 0;
    Opcode opcode = inst_node->content.instruction.opcode;
    AddressingMode src_ad_mod = NONE;
    AddressingMode dest_ad_mod = NONE;
    encoded_line->ast_node = inst_node;
    int i, j;
    int added_word_idx = 1;
    int is_src = 1;
    if (inst_node->content.instruction.src_op.mode != NONE)
    {
        src_ad_mod = inst_node->content.instruction.src_op.mode;
    }

    if (inst_node->content.instruction.dest_op.mode != NONE)
    {
        dest_ad_mod = inst_node->content.instruction.dest_op.mode;
    }

    /* TODO: validate expected number of ops is not exceeded or insufficient */
    int num_exp_ops;
    num_exp_ops = expect_operands(opcode);

    init_words(encoded_line->words, 5);

    encode_opcode(opcode, src_ad_mod, dest_ad_mod, encoded_line);

    if (!((src_ad_mod == REGISTER) && (dest_ad_mod == REGISTER)))
    {
        if (src_ad_mod != NONE)
            encoders[src_ad_mod](src_ad_mod, &added_word_idx, encoded_line, is_src);
        encoders[dest_ad_mod](dest_ad_mod, &added_word_idx, encoded_line, !is_src);
    }
    else
    {
        encoders[src_ad_mod](src_ad_mod, &added_word_idx, encoded_line, is_src);
        encoders[dest_ad_mod](dest_ad_mod, &added_word_idx, encoded_line, !is_src);
    }

    /* Print all saved words for debugging */
    printf("Encoded words:\n");
    for (i = 0; i < encoded_line->words_count + 1; i++)
    {
        printf("Word %d: ", i);
        for (j = 0; j < 10; j++)
        {
            printf("%c", encoded_line->words[i][j]);
        }
        printf("\n");
    }
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

void encode_operand(AddressingMode op_mode, int *added_word_idx, EncodedLine *line, int is_src)
{
    /*
    immediate operand: 8 bits for number + 2 AER bits (LSB) => AER must be A = 00
    direct operand: 8 bits for the adress of the label + 2 AER bits (LSB) => could be any of the AER
    mat access: 2 words, -> 1. is label (address + AER)
                     -> 2. 6-9 bits are row reg, 2-5 are col reg
    register:
    if register is src_op => 6-9 bits is reg_num bits
    if register is dest_op => 2-5 bits are reg_num bits
    if register is --both-- => than there is only 1 added word  2-5 bits are dest_reg_num bits
                                                                6-9 bits is src_reg_num bits
    */
    switch (op_mode)
    {
    case IMMEDIATE:
    {
        int val;
        if (added_word_idx == 1)
            val = line->ast_node->content.instruction.src_op.value.immediate_value;
        else
            val = line->ast_node->content.instruction.dest_op.value.immediate_value;

        write_bits(line->words[*added_word_idx], val, 2, 9);
        line->words_count++;
        (*added_word_idx)++;
    }
    break;
    case DIRECT:
    {
        char *label_name;
        if (added_word_idx == 1)
            label_name = line->ast_node->content.instruction.src_op.value.label;
        else
            label_name = line->ast_node->content.instruction.dest_op.value.label;
        printf("Waiting for address for label: %s\n", label_name);
        (*added_word_idx)++;
        line->words_count++;
    }
    break;
    case MAT_ACCESS:
    {
        int row_reg_num;
        int col_reg_num;
        if (is_src)
        {
            row_reg_num = line->ast_node->content.instruction.src_op.value.index.row_reg_num;
            col_reg_num = line->ast_node->content.instruction.src_op.value.index.col_reg_num;
        }
        else
        {
            row_reg_num = line->ast_node->content.instruction.dest_op.value.index.row_reg_num;
            col_reg_num = line->ast_node->content.instruction.dest_op.value.index.col_reg_num;
        }
        write_bits(line->words[*added_word_idx], row_reg_num, 6, 9);
        (*added_word_idx)++;
        line->words_count++;
        write_bits(line->words[*added_word_idx], col_reg_num, 2, 5);
        (*added_word_idx)++;
        line->words_count++;
    }
    break;
    case REGISTER:
    {
        int reg_num;
        int start_bit;
        int end_bit;
        int is_dest_mode_reg = line->ast_node->content.instruction.dest_op.mode == REGISTER;
        if (is_src)
        {
            reg_num = line->ast_node->content.instruction.src_op.value.reg_num;
            start_bit = 6;
            end_bit = 9;
            write_bits(line->words[*added_word_idx], reg_num, start_bit, end_bit);
            if (!is_dest_mode_reg)
            {
                (*added_word_idx)++;
                line->words_count++;
            }
        }
        else
        {
            reg_num = line->ast_node->content.instruction.dest_op.value.reg_num;
            start_bit = 2;
            end_bit = 5;
            write_bits(line->words[*added_word_idx], reg_num, start_bit, end_bit);
            line->words_count++;
        }
    }
    break;

    default:
        break;
    }
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