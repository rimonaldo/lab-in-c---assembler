#ifndef FIRST_PASS_H
#define FIRST_PASS_H
#include "../common/AST/ast.h"
#include "../common/tokenizer/tokenizer.h"

#include "../common/AST/ast.h"
#include "../common/table/table.h"
#include "../common/tokenizer/tokenizer.h"
#include "../common/encoding/encoding.h"
#include "../common/utils/utils.h"
#include "../common/printer/printer.h"
#include "../common/errors/errors.h"

typedef enum
{
    SYMBOL_DATA,
    SYMBOL_CODE,
    SYMBOL_EXTERN,
    SYMBOL_ENTRY
} SymbolType;

typedef struct SymbolInfo
{
    char name[31];
    int address;
    SymbolType type;
    int is_entry;
    int is_extern
} SymbolInfo;

void run_first_pass(char *filename,Table *symbol_table, ASTNode **head,int *IC ,StatusInfo *status_info);
ASTNode *parse_instruction_line(int line_num, Tokens tokenized_line, int leader_idx);
ASTNode *parse_directive_line(int line_num, Tokens tokenized_line, int leader_idx, int *DC_ptr);
int is_symbol_declare(char *token);
int is_instruction_line(char *leader);
int is_directive_line(char *leader);

Opcode get_opcode(char *str);

/* HELPER FUNCTIONS */
int is_valid_number_operand(char *value);
int is_valid_mat_access(char *value);
int is_valid_register(char *value);
int is_valid_label_name(char *token);
int is_comment_line(char *token);
int is_empty_line(Tokens tokens);

char *copy_label_token(char *token);
void insert_entry_label(Table *ent_table, char *label, int address);
void insert_extern_label(Table *ext_table, char *label, int address);
void set_directive_flags(ASTNode *node, SymbolInfo *info);

/* GETTER FUNCTIONS */
StatementType get_statement_type(char *leader);
DirectiveType get_directive_type(char *dir);
Opcode get_opcode(char *str);

const char *addressing_mode_name(AddressingMode mode);

Status parse_instruction_operand(Operand *operand_to_parse, Tokens tokenized_line, int token_idx);

#endif
