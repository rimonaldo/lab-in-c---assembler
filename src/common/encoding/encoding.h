#ifndef ENCODING_H
#define ENCODING_H
#include "../AST/ast.h"
typedef char BinCode[11];

typedef struct EncodedLine
{
    ASTNode *ast_node;
    int decimal_address;
    char base4_address[5];
    BinCode words[5];
    int is_waiting_words[5];
    int words_count;
    struct EncodedLine *next;
} EncodedLine;

/*------------- Encoding functions ------------- */
EncodedLine *encode_instruction_line(ASTNode *inst_node, int leader_idx);
void encode_opcode(Opcode opcode, AddressingMode src_op_mode, AddressingMode dest_op_mode, EncodedLine *line);

/*------------- bit convertions functions ------------- */
void write_bits(BinCode bincode, int val, int start_bit, int end_bit);

/*------------- encoded list functions ------------- */
void append_encoded_line(ASTNode **head, ASTNode **tail, EncodedLine *new_line);
void free_encoded_line_list(EncodedLine *head);

#endif
