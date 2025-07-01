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
    int words_count;
    struct EncodedLine *next;
} EncodedLine;

/*------------- Encoding functions ------------- */
EncodedLine *encode_instruction_line(ASTNode *inst_node, int leader_idx);
void encode_opcode(Opcode opcode, AddressingMode src_op_mode, AddressingMode dest_op_mode, EncodedLine *line);
void encode_immediate_val(int val, EncodedLine *line);
void encode_direct_label(char *label, int is_ext, EncodedLine *line);
void encode_mat_access(char *label, int row_reg_num, int col_reg_num, int is_ext, EncodedLine *line);
void encode_register(int reg_num, EncodedLine *line);

/*------------- bit convertions functions ------------- */

/*------------- encoded list functions ------------- */
void append_encoded_line(ASTNode **head, ASTNode **tail, EncodedLine *new_line);
void free_encoded_line_list(EncodedLine *head);

#endif
