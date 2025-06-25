#ifndef FIRST_PASS_H
#define FIRST_PASS_H
#include "../common/AST/ast.h"
#include "../common/tokenizer/tokenizer.h"

void run_first_pass(char *filename);
ASTNode *parse_instruction_line(int line_num, int DC, Tokens tokenized_line);
ASTNode *parse_directive_line(int line_num, Tokens tokenized_line);
int is_label_declare(char *token);
int is_directive_line(Tokens tokenized_line);
AddressingMode get_mode(Tokens tokenized_line, int token_idx);

#endif
