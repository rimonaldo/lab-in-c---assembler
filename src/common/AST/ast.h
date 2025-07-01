#ifndef AST_H
#define AST_H
#include "../tokenizer/tokenizer.h"
/*===============================
  Type Definitions (Enums)
===============================*/

typedef enum
{
    IMMEDIATE,
    DIRECT,
    MAT_ACCESS,
    REGISTER,
    NONE
} AddressingMode;

typedef enum
{
    MOV = 0,
    CMP,
    ADD,
    SUB,
    LEA,
    CLR,
    NOT,
    INC,
    DEC,
    JMP,
    BNE,
    RED,
    PRN,
    JSR,
    RTS,
    STOP = 15

} Opcode;

typedef enum
{
    DATA,
    STRING,
    MAT,
    ENTRY,
    EXTERN
} DirectiveType;

typedef enum
{
    INSTRUCTION_STATEMENT,
    DIRECTIVE_STATEMENT,
    INVALID_STATEMENT
} StatementType;

/*===============================
  Struct Definitions
===============================*/

typedef struct Operand
{
    union
    {
        int immediate_value; /* for IMMEDIATE */
        char *label;         /* for DIRECT */
        int reg_num;         /* for DIRECT_REGISTER */
        struct               /* for MATRIX_ACCESS */
        {
            char *label; /* label of the matrix */
            int reg_num; /* register holding the index */
        } index;
    } value;
    AddressingMode mode;
} Operand;

/**
 * @brief Structure representing information about an assembly instruction.
 *
 * This structure holds the opcode, number of operands, and the source and
 * destination operands for a single instruction in the abstract syntax tree (AST).
 *
 * @typedef InstructionInfo
 * @param opcode      The operation code specifying the instruction type.
 * @param num_operands The number of operands used by the instruction.
 * @param src_op      The source operand for the instruction.
 * @param dest_op     The destination operand for the instruction.
 */
typedef struct InstructionInfo
{
    Opcode opcode;
    int num_operands;
    Operand src_op;
    Operand dest_op;
} InstructionInfo;

typedef struct DirectiveInfo
{
    DirectiveType type;
    union
    {
        struct
        {
            int *values;
            int size;
        } data;      /* for DATA directive */
        char *str;   /* for STRING directive */
        char *label; /* for ENTRY or EXTERN directives */
    } params;
} DirectiveInfo;

typedef struct ASTNode
{
    int line_number;    /* source code line number */
    char *label;        /* label or NULL */
    StatementType type; /* INSTRUCTION_STATEMENT or DIRECTIVE_STATEMENT */
    union
    {
        InstructionInfo instruction;
        DirectiveInfo directive;
    } content;

    struct ASTNode *next; /* pointer to next AST node */
} ASTNode;

/*===============================
  Function Prototypes
===============================*/

/* AST node builders*/
ASTNode *create_instruction_node(int line_num, const char *label, InstructionInfo instruction);
ASTNode *create_directive_node(int line_num, const char *label, DirectiveInfo directive);
/* Append an ASTNode to the end of the list, updating head and tail pointers */
void append_ast_node(ASTNode **head, ASTNode **tail, ASTNode *new_node);

/* Operand builders */
Operand *create_immediate_operand(int value);
Operand *create_direct_operand(const char *label);
Operand *create_register_operand(int reg_num);
Operand *create_index_operand(const char *label, int reg_num);

/* statement builders */
InstructionInfo *create_instruction_info();
DirectiveInfo *create_directive_info();

/* destroy structs */
void free_ast(ASTNode *head);
void free_operand(Operand *op);
void free_instruction_contents(InstructionInfo *inst);
void free_directive_contents(DirectiveInfo *dir);

AddressingMode get_mode(Tokens tokenized_line, int token_idx);
int expect_operands(Opcode opcode);
const char *get_ad_mod_name(AddressingMode mode);


#endif /* AST_H */
