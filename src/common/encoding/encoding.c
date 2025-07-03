#include <stdlib.h>
#include <stdio.h>

#include "encoding.h"
#include "../AST/ast.h"

/*
 * Note: The following data structures are assumed to be defined in included headers:
 * - BinCode (e.g., typedef char BinCode[10];)
 * - ASTNode and related structs (AddressingMode, Opcode, etc.)
 * - EncodedLine
 */

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

/* Define a function pointer type for encoding specific operand addressing modes */
typedef void (*EncodeFunc)(AddressingMode mode, int *word_idx, EncodedLine *line, int is_src);

/* ----------------LOW-LEVEL BIT & MEMORY UTILITIES---------------- */
/**
 * Converts a logical bit number (0-9) to its inverse array index for a 10-char array.
 * Example: bit 9 maps to index 0, bit 0 maps to index 9.
 */
int inverse_idx(int bit)
{
    if (bit > 9 || bit < 0)
    {
        /* handle error */
        printf("INVALID BIT NUMBER\n");
        return -1; /* Return error */
    }
    return 9 - bit;
}

/**
 * Writes an integer value into a specified range of bits in a binary code string.
 */
void write_bits(BinCode bincode, int val, int start_bit, int end_bit)
{
    int num_bits = end_bit - start_bit + 1;
    int max_val = (1 << num_bits) - 1;
    int i;

    if (start_bit > end_bit)
    {
        printf("INVALID: start bit: %d is larger than end bit: %d\n", start_bit, end_bit);
        return;
    }

    if (val > max_val)
    {
        printf("INVALID VALUE: %d exceeds max allowed value %d for %d bits\n", val, max_val, num_bits);
        return;
    }

    for (i = 0; i < num_bits; i++)
    {
        /* Isolate the i-th bit of the value */
        int bit_val = (val >> i) & 1;
        /* Calculate the target bit position in the 0-9 range */
        int target_bit = start_bit + i;
        /* Write '0' or '1' to the correct inverse array index */
        bincode[inverse_idx(target_bit)] = bit_val ? '1' : '0';
    }
}

/**
 * Initializes the binary code words in an EncodedLine to all '0's.
 */
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

/**
 * Frees the memory allocated for a linked list of EncodedLine structs.
 */
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

/* --------------FIRST WORD ASSEMBLY HELPER FUNCTIONS-------------- */
/**
 * Assembles the A,R,E (Absolute, Relocatable, External) bits (0-1).
 */
void assemble_AER(BinCode bincode, int AER)
{
    /* AER values: 0 for Absolute, 1 for Relocatable, 2 for External */
    write_bits(bincode, AER, 0, 1);
}

/**
 * Assembles the source operand addressing mode bits (2-3).
 */
void assemble_src_op_mod(BinCode bincode, AddressingMode mode)
{
    /* Addressing modes are mapped to 0-3 */
    write_bits(bincode, mode, 4, 5);
}

/**
 * Assembles the destination operand addressing mode bits (4-5).
 */
void assemble_dest_op_mod(BinCode bincode, AddressingMode mode)
{
    /* Addressing modes are mapped to 0-3 */
    write_bits(bincode, mode, 2, 3);
}

/**
 * Assembles the opcode bits (6-9).
 */
void assemble_opcode(BinCode bincode, Opcode opcode)
{
    write_bits(bincode, opcode, 6, 9);
}

/* ----------------OPERAND ENCODING IMPLEMENTATIONS---------------- */
/**
 * Encodes an operand based on its addressing mode. This function generates
 * the additional machine words required by the operand.
 */
void encode_operand(AddressingMode op_mode, int *added_word_idx, EncodedLine *line, int is_src)
{
    switch (op_mode)
    {
    case IMMEDIATE:
    {
        int val = is_src ? line->ast_node->content.instruction.src_op.value.immediate_value
                         : line->ast_node->content.instruction.dest_op.value.immediate_value;

        write_bits(line->words[*added_word_idx], val, 2, 9);
        assemble_AER(line->words[*added_word_idx], 0); /* Immediate is always Absolute */
        line->words_count++;
        (*added_word_idx)++;
        break;
    }
    case DIRECT:
    {
        char *label_name = is_src ? line->ast_node->content.instruction.src_op.value.label
                                  : line->ast_node->content.instruction.dest_op.value.label;

        printf("Waiting for address for label: %s\n", label_name);
        /* The address and AER bits will be filled in a later pass */
        line->is_waiting_words[line->words_count] = 1;
        line->words_count++;
        (*added_word_idx)++;
        break;
    }
    case MAT_ACCESS:
    {
        int row_reg_num, col_reg_num;

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

        /* First extra word is for the matrix label address (handled like DIRECT) */
        printf("Waiting for address for matrix label: %s\n", line->ast_node->content.instruction.src_op.value.label);
        line->words_count++;
        (*added_word_idx)++;

        /* Second extra word encodes the two registers */
        write_bits(line->words[*added_word_idx], row_reg_num, 6, 9); /* Source reg */
        write_bits(line->words[*added_word_idx], col_reg_num, 2, 5); /* Dest reg */
        assemble_AER(line->words[*added_word_idx], 0);               /* Register word is Absolute */
        line->words_count++;
        (*added_word_idx)++;
        break;
    }
    case REGISTER:
    {
        int reg_num;

        int is_dest_mode_reg = (line->ast_node->content.instruction.dest_op.mode == REGISTER);

        /* Logic to handle one or two registers in a single extra word */
        if (is_src)
        {
            reg_num = line->ast_node->content.instruction.src_op.value.reg_num;
            write_bits(line->words[*added_word_idx], reg_num, 6, 9); /* Source bits */

            if (!is_dest_mode_reg) /* If only source is a register */
            {
                assemble_AER(line->words[*added_word_idx], 0);
                line->words_count++;
                (*added_word_idx)++;
            }
        }
        else /* is_dest */
        {
            reg_num = line->ast_node->content.instruction.dest_op.value.reg_num;
            write_bits(line->words[*added_word_idx], reg_num, 2, 5); /* Destination bits */
            assemble_AER(line->words[*added_word_idx], 0);
            line->words_count++;
            /* No need to increment added_word_idx if src was also a register,
               as it has already been written to the same word. */
        }
        break;
    }
    default:
        break;
    }
}

/* Wrapper for operands that need a single extra word (Immediate, Direct) */
void encode_single(AddressingMode mode, int *added_word_idx, EncodedLine *line, int is_src)
{
    printf("Encoding single operand at word index %d, mode: %s\n", *added_word_idx, get_ad_mod_name(mode));
    encode_operand(mode, added_word_idx, line, is_src);
}

/* Wrapper for matrix access encoding */
void encode_mat_access(AddressingMode mode, int *word_idx, EncodedLine *line, int is_src)
{
    encode_operand(mode, word_idx, line, is_src);
}

/* An array of function pointers to dispatch encoding based on addressing mode */
EncodeFunc encoders[] = {
    [IMMEDIATE] = encode_single,
    [DIRECT] = encode_single,
    [REGISTER] = encode_operand, /* Use the main function for registers' special logic */
    [MAT_ACCESS] = encode_mat_access,
};

/* ----------------TOP-LEVEL ENCODING ORCHESTRATION----------------*/
/**
 * Encodes the first machine word containing the opcode and addressing modes.
 */
void encode_opcode(Opcode opcode, AddressingMode src_op_mode, AddressingMode dest_op_mode, EncodedLine *line)
{
    BinCode *bincode = &(line->words[0]);

    printf("src_ad_mod: %s, dest_ad_mod: %s\n", get_ad_mod_name(src_op_mode), get_ad_mod_name(dest_op_mode));

    assemble_AER(*bincode, 0); /* Instruction word is always Absolute */
    assemble_src_op_mod(*bincode, src_op_mode);
    assemble_dest_op_mod(*bincode, dest_op_mode);
    assemble_opcode(*bincode, opcode);
}

/**
 * Main function to encode a full instruction line from the AST.
 * It orchestrates the encoding of the first word and any subsequent operand words.
 */
EncodedLine *encode_instruction_line(ASTNode *inst_node, int leader_idx)
{
    printf("----------- ENCODING LINE ----------- \n");

    EncodedLine *encoded_line = malloc(sizeof(EncodedLine));
    if (!encoded_line)
        return NULL;

    encoded_line->words_count = 1; /* Start with 1 for the opcode word */
    encoded_line->ast_node = inst_node;
    encoded_line->next = NULL;

    Opcode opcode = inst_node->content.instruction.opcode;
    AddressingMode src_ad_mod = inst_node->content.instruction.src_op.mode;
    AddressingMode dest_ad_mod = inst_node->content.instruction.dest_op.mode;

    int added_word_idx = 1; /* Index for extra words, starts after opcode word */

    init_words(encoded_line->words, 5);

    /* 1. Encode the first word (opcode and modes) */
    encode_opcode(opcode, src_ad_mod, dest_ad_mod, encoded_line);

    /* 2. Encode operand words */
    if (src_ad_mod == REGISTER && dest_ad_mod == REGISTER)
    {
        /* Special case: both operands are registers, share one word */
        write_bits(encoded_line->words[added_word_idx], inst_node->content.instruction.src_op.value.reg_num, 6, 9);
        write_bits(encoded_line->words[added_word_idx], inst_node->content.instruction.dest_op.value.reg_num, 2, 5);
        assemble_AER(encoded_line->words[added_word_idx], 0);
        encoded_line->words_count++;
    }
    else
    {
        /* Handle source and destination operands sequentially */
        if (src_ad_mod != NONE)
        {
            encoders[src_ad_mod](src_ad_mod, &added_word_idx, encoded_line, 1); /* is_src = true */
        }
        if (dest_ad_mod != NONE)
        {
            encoders[dest_ad_mod](dest_ad_mod, &added_word_idx, encoded_line, 0); /* is_src = false */
        }
    }

    /* For debugging: Print all generated words for the line */
    printf("Encoded words:\n");
    int i, j;
    for (i = 0; i < encoded_line->words_count; i++)
    {
        printf("Word %d: ", i);
        if (!encoded_line->is_waiting_words[i])
        {
            for (j = 0; j < 10; j++)
            {
                printf("%c", encoded_line->words[i][j]);
            }
        }
        else
        {
            printf("?");
        }
        printf("\n");
    }
    return encoded_line;
}