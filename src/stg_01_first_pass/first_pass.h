#ifndef FIRST_PASS_H
#define FIRST_PASS_H
#include "../common/AST/ast.h"
#include "../common/tokenizer/tokenizer.h"

void run_first_pass(char *filename);
ASTNode *parse_instruction_line(int line_num, Tokens tokenized_line, int leader_idx);
ASTNode *parse_directive_line(int line_num, Tokens tokenized_line, int leader_idx, int *DC_ptr);
int is_symbol_declare(char *token);
int is_instruction_line(char *leader);
int is_directive_line(char *leader);

Opcode get_code(char *str);

/* HELPER FUNCTIONS */
int is_valid_number_operand(char *value);
int is_valid_mat_access(char *value);
int is_valid_register(char *value);
int is_valid_label_name(char *token);
int is_comment_line(char *token);
int is_empty_line(Tokens tokens);

const char *addressing_mode_name(AddressingMode mode);

Status parse_operand(Operand *operand_to_parse, Tokens tokenized_line, int token_idx);

typedef enum
{
    SYMBOL_DATA,
    SYMBOL_CODE,
    SYMBOL_EXTERN
} SymbolType;

typedef struct SymbolInfo
{
    char name[31];
    int address;
    SymbolType type;
    int is_entry;
} SymbolInfo;

#endif
