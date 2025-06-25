#include <stdio.h>
#include <string.h>
#include "first_pass.h"
#include "../common/AST/ast.h"
#include "../common/table/table.h"
#include "../common/tokenizer/tokenizer.h"

void run_first_pass(char *filename)
{
    int IC = 0;
    int DC = 0;
    bool is_label_declare = false;
    bool is_directive = false;
    bool is_ext_ent = false;
    Table *symbol_table = table_create();
    FILE *file = fopen(filename, "r");
    printf("FILENAME IS: \n %s\n\n", filename);
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }
    char line[1024];

    while (fgets(line, sizeof(line), file))
    {
        printf("%s", line);
        Tokens tokenized_line = tokenize_line(line);
        char *leader = tokenized_line.tokens[0];

        /* if first token is a label declaration*/
        if (is_label(leader))
        {
            /* if it's not declared before */
            if (!table_lookup(symbol_table, leader))
            {
                /* enter label to symbol table */
            }
            else
            {
                /* add to error list -> label already declared*/
            }
            /* create an ast node starting from tokens[1] to termination of line */
        }

        if (is_instruction_line(tokenized_line))
        {
            parse_instruction_line(DC, tokenized_line);
        }
        else if (is_directive_line(tokenized_line))
        {
            parse_directive_line(DC, tokenized_line);
        }
    }

    /* parse line */
    /* if the first token is a symbol definition than add to symbol table */
    /* parse rest of line and create ast node */
    /* read - .entry MAIN directive -> that declares MAIN as an entry point (for linker) */
    /* כאן אכניס לרשימת הנחיות כניסה את הסמל */
    /* read - SIZE:   .data 2 -> |add to symble table, IC = 1 | SIZE now is a constant with value 2*/
    /*      הכנסת סמל לטבלת סמלים */
    /*      יצירת צומת הנחיה לשמירת מספר */
    /* read - SUM:    .data 0 -> |add to symble table, IC = 2 | MAT is now a matrix with these values */
    /*      יצירת צומת הנחיה לשמירת מספר */
    /*      הכנסת סמל לטבלה */
    /* read - MAT:    .mat [2][2] 10, -3, 8, 5 -> |add to symble table, IC = 3 | MAT is now a matrix with these values */
    /*      הכנסת סמל לטבלה */
    /*      יצירת צומת הנחיה לשמירת מספר */
    /* read - MAIN -> |add to symble table, IC = 0, IC = 4, has entry att | */
    /* read - clr     r0 -> sets r0 to zero */
    /* read - mov     SIZE, r1 ->  r1 is 2 (the )*/
    /* read - dec     r1 -> */
    fclose(file);
}

ASTNode *parse_instruction_line(int line_num, Tokens tokenized_line)
{
    char *init_label = NULL;
    InstructionInfo *info = create_instruction_info();
    ASTNode *instrucion_node = create_instruction_node(line_num, init_label, *info);
    Opcode opcode = get_opcode(tokenized_line.tokens[0]);
    int expected_num_op = operands_expected(opcode);
    info->opcode = opcode;

    switch (expected_num_op)
    {
    case 0:
    {
        /* 15 and 14 : are zero operand instructions */
        /*      no need to parse any operand*/
    }
    break;
    case 1:
    {
        /* 13 to 5  : are one operand instructions */
        /*      parse dest_operand*/
        parse_operand((&(info->dest_op)), tokenized_line);
    }
    break;
    case 2:
    {
        /* 4 to 0  : are two operand instructions */
        /*      parse src_operand*/
        /*      parse dest_operand*/
        parse_operand((&(info->src_op)), tokenized_line);
        parse_operand((&(info->dest_op)), tokenized_line);
    }
    break;

    default:
        break;
    }
    /* commented code
    switch (src_mode)
    {
        case IMMEDIATE:
    {
    }
    break;
    case DIRECT:
    {
    }
    break;
    case INDEX:
    {
    }
    break;
    case REGISTER:
    {
    }
    break;

    default:
    break;
    }
*/
}

void parse_directive_line(int line_num, Tokens tokenized_line)
{
}

/* HELPER FUNCTIONS */
int operands_expected(Opcode opcode)
{

    /* Determine the number of operands expected for each opcode */
    switch (opcode)
    {
    case 0: /* mov */
    case 1: /* cmp */
    case 2: /* add */
    case 3: /* sub */
    case 6: /* lea */
        return 2;
    case 4:  /* not */
    case 5:  /* clr */
    case 7:  /* inc */
    case 8:  /* dec */
    case 9:  /* jmp */
    case 10: /* bne */
    case 11: /* red */
    case 12: /* prn */
    case 13: /* jsr */
        return 1;
    case 14: /* rts */
    case 15: /* stop */
        return 0;
    default:
        return -1; /* Invalid opcode */
    }
}

/*@brief validates all syntax rules for valid label name*/
/*
    *Starts with an alphabetic character (a-z, A-Z).
       Followed by zero or more alphanumeric characters.
       Terminated by a colon (:)
       Maximum length: 30 characters.
       Case-sensitive.
       Not a reserved word (instruction name, directive name, register name).
       */
bool is_label(char *token)
{
    if (token)

        return true;
}

bool is_directive_line(Tokens tokenized_line)
{
    char dir_prefix = tokenized_line.tokens[0][0];
    char *dir_name = &tokenized_line.tokens[0][1];

    if (dir_prefix != '.')
        return false;

    return (strcmp(dir_name, "data") == 0 ||
            strcmp(dir_name, "string") == 0 ||
            strcmp(dir_name, "mat") == 0 ||
            strcmp(dir_name, "entry") == 0 ||
            strcmp(dir_name, "extern") == 0);
}