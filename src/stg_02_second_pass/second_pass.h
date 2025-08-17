#ifndef SECOND_PASS_H
#define SECOND_PASS_H
#include "../common/AST/ast.h"
#include "../common/errors/errors.h" 
#include "../common/printer/printer.h"
#include "../common/table/table.h"
#include "../common/encoding/encoding.h"
#include "../common/symbols/symbols.h"


void run_second_pass(Table *symbol_table, ASTNode **ast_head,EncodedList *encoded_list, StatusInfo *status_info);


#endif