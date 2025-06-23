#ifndef AST_H
#define AST_H

#include <stdlib.h>

/* --- Operand Structures --- */

typedef enum
{
    OPERAND_IMMEDIATE,
    OPERAND_REGISTER,
    OPERAND_LABEL,
    OPERAND_MATRIX
} OperandType;

typedef struct
{
    OperandType type;
    union
    {
        int immediate;
        int reg_num;
        char *label;
        struct
        {
            char *label;
            int row_reg;
            int col_reg;
        } matrix;
    } value;
} Operand;


/* --- Statement Structures --- */

typedef enum {
    STATEMENT_INSTRUCTION,
    STATEMENT_DIRECTIVE
} StatementType;

typedef struct Statement {
    char* label;                
    StatementType type;
    union {
        struct {              
            char* name;
            Operand* src_operand;
            Operand* dest_operand;
        } instruction;
        struct {               
            char* name;
            void* data;        
        } directive;
    } line;
    struct Statement *next;     
} Statement;


/* --- Program AST Structure --- */

typedef struct {
    Statement *head;
    Statement *tail;
} ProgramAST;


/* --- Function Prototypes for ast.c --- */

/* Operand Creation */
Operand* create_immediate_operand(int value);
Operand* create_register_operand(int reg_num);
Operand* create_label_operand(char *label);
Operand* create_matrix_operand(char *label, int row_reg, int col_reg);

/* Statement Creation */
Statement* create_instruction_statement(char* label, char* name, Operand* src, Operand* dest);
Statement* create_directive_statement(char* label, char* name, void* data);

/* AST Management */
ProgramAST* create_program_ast();
void add_statement_to_ast(ProgramAST *ast, Statement *statement);

/* Memory Management */
void free_operand(Operand *operand);
void free_statement(Statement *statement);
void free_program_ast(ProgramAST *ast);

#endif /* AST_H */
