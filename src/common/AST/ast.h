#ifndef AST_H
#define AST_H

/*===============================
  Type Definitions (Enums)
===============================*/

typedef enum
{
    IMMEDIATE,
    DIRECT,
    INDEX,
    REGISTER
} AddressingMode;

typedef enum
{
    MOV,
    CMP,
    ADD,
    SUB,
    NOT,
    CLR,
    LEA,
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
    DIRECTIVE_STATEMENT
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

#endif /* AST_H */
