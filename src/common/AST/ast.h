#ifndef AST_H
#define AST_H

/*================================
/* Type Definitions(Enums) /*
/*===============================*/

typedef enum
{
    IMMEDIATE,
    DIRECT,
    MATRIX_ACCESS,
    DIRECT_REGISTER
} AddressingMode;

typedef enum
{
    MOV,
    CMP,
    ADD,
    SUB,
    LEA,
    NOT,
    CLR,
    INC,
    DEC,
    JMP,
    BNE,
    RED,
    PRN,
    JSR,
    RTS,
    STOP
} Opcode;

typedef enum
{
    DATA,
    STRING,
    ENTRY,
    EXTERN
} DirectiveType;

typedef enum
{
    INSTRUCTION,
    DIRECTIVE
} StatementType;

/*================================
/* Struct Definitions /*
/*===============================*/

/*@brief single operand*/
typedef struct Operand
{
    AddressingMode mode;
    union
    {
        int immediate_val; /*for IMMEDIATE*/
        char *label;       /*for DIRECT*/
        int reg_num;       /*for DIRECT_REGISTER*/
        struct
        { /*for INDEX (matrix access)*/
            char *label;
            int reg_num;
        } index;
    } value;
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
        } data;      /*for DATA*/
        char *str;   /*for STRING*/
        char *label; /*for ENTRY or EXTERN*/
    } params;
} DirectiveInfo;

typedef struct ASTNode
{
    int line_number;
    char *label;        /*either label or NULL*/
    StatementType type; /* either INSTRUCTION or DIRECTIVE*/
    union
    {
        InstructionInfo instruction;
        DirectiveInfo directive;
    } content;

    struct ASTNode *next; /* pointer to the next node in the list */
} ASTNode;

/*================================
/* Function Prototypes /*
/*===============================*/

ASTNode *create_instruction_node(int line_num, const char *label, InstructionInfo instruction);
ASTNode *create_directive_node(int line_num, const char *label, DirectiveInfo directive);
void free_ast(ASTNode *head);

#endif