#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "first_pass.h"
#include "../common/AST/ast.h"
#include "../common/table/table.h"
#include "../common/tokenizer/tokenizer.h"

/*====================================================*/
/*                  First Pass Driver                 */
/*====================================================*/

void run_first_pass(char *filename)
{
    int IC = 0;
    int DC = 0;
    int is_label = false;
    int is_directive = false;
    int is_ext_ent = false;
    Table *symbol_table = table_create();
    FILE *file = fopen(filename, "r");
    char line[1024];
    int line_number = 1;
    Tokens tokenized_line;
    char *leader;

    printf("FILENAME IS:\n%s\n\n", filename);

    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        printf("\n=== [Line %d] ===============================\n", line_number);
        printf("Raw Line: %s", line);

        tokenized_line = tokenize_line(line);
        leader = tokenized_line.tokens[0];
        printf("First Token: %s\n", leader);

        if (is_label_declare(leader))
        {
            printf("Detected label: %s\n", leader);
            if (!table_lookup(symbol_table, leader))
            {
                printf("Label '%s' is new — inserting into symbol table.\n", leader);
            }
            else
            {
                printf("ERROR: Label '%s' already declared.\n", leader);
            }
        }
        else if (is_instruction_line(tokenized_line))
        {
            printf("Instruction detected.\n");
            parse_instruction_line(line_number, DC, tokenized_line);
        }
        else if (is_directive_line(tokenized_line))
        {
            printf("Directive detected.\n");
            parse_directive_line(DC, tokenized_line);
        }

        line_number++;
    }

    fclose(file);
}

/*====================================================*/
/*               Instruction & Directive              */
/*====================================================*/

ASTNode *parse_instruction_line(int line_num, int DC, Tokens tokenized_line)
{
    InstructionInfo *info = create_instruction_info();
    Opcode opcode = get_opcode(tokenized_line.tokens[0]);
    int expected_num_op = expect_operands(opcode);
    info->opcode = opcode;

    printf("Parsing instruction line #%d\n", line_num);
    printf("Opcode: %d, Expected operands: %d\n", opcode, expected_num_op);

    switch (expected_num_op)
    {
    case 1:
        printf("Parsing one operand (dest)...\n");
        parse_operand(&(info->dest_op), tokenized_line, 1);
        break;
    case 2:
        printf("Parsing two operands (src, dest)...\n");
        parse_operand(&(info->src_op), tokenized_line, 1);
        parse_operand(&(info->dest_op), tokenized_line, 2);
        break;
    default:
        printf("Zero operands or unknown.\n");
        break;
    }

    return create_instruction_node(line_num, NULL, *info);
}

ASTNode *parse_directive_line(int line_num, Tokens tokenized_line)
{
    DirectiveInfo *info;
    info = malloc(sizeof(DirectiveInfo));
    if (!info) {
        printf("Failed to allocate DirectiveInfo\n");
        return NULL;
    }
    info->type = DATA;
    info->params.data.values = 2;
    info->params.data.size = 1;
    info->type = 0; 
    return create_directive_node(line_num, ".data", *info);
}

/*====================================================*/
/*                 Operand Parsing Utils              */
/*====================================================*/

void parse_operand(Operand *operand_to_parse, Tokens tokenized_line, int token_idx)
{
    printf("Parsing operand at token index %d: %s\n", token_idx, tokenized_line.tokens[token_idx]);

    operand_to_parse->mode = get_mode(tokenized_line, token_idx);
    printf("Detected addressing mode: %d\n", operand_to_parse->mode);

    switch (operand_to_parse->mode)
    {
    case IMMEDIATE:
        operand_to_parse->value.immediate_value = atoi(tokenized_line.tokens[token_idx] + 1);
        printf("Immediate value: %d\n", operand_to_parse->value.immediate_value);
        break;
    case DIRECT:
        operand_to_parse->value.label = strdup(tokenized_line.tokens[token_idx]);
        printf("Direct label: %s\n", operand_to_parse->value.label);
        break;
    case REGISTER:
        operand_to_parse->value.reg_num = atoi(tokenized_line.tokens[token_idx] + 1);
        printf("Register number: %d\n", operand_to_parse->value.reg_num);
        break;
    case MAT:
        operand_to_parse->value.index.label = strdup(tokenized_line.tokens[token_idx]);
        operand_to_parse->value.index.reg_num = atoi(tokenized_line.tokens[token_idx + 1] + 1);
        printf("Matrix label: %s, Register index: %d\n",
               operand_to_parse->value.index.label,
               operand_to_parse->value.index.reg_num);
        break;
    default:
        printf("Unknown operand mode.\n");
        break;
    }
}

AddressingMode get_mode(Tokens tokenized_line, int token_idx)
{
    char *value = tokenized_line.tokens[token_idx];
    if (is_label(value))
        return DIRECT;
    return DIRECT;
}

/*====================================================*/
/*                Label & Directive Utils             */
/*====================================================*/

int is_label_declare(char *token)
{
    int len = strlen(token);
    return (is_label(token) && token[len - 1] == ':');
}

int is_label(char *token)
{
    int i;
    int len;

    if (!token)
    {
        return false;
    }

    len = strlen(token);

    if (len == 0 || len > 31)
    {
        return false;
    }

    if (!((token[0] >= 'A' && token[0] <= 'Z') || (token[0] >= 'a' && token[0] <= 'z')))
    {
        return false;
    }

    for (i = 1; i < len - 1; i++)
    {
        if (!((token[i] >= 'A' && token[i] <= 'Z') ||
              (token[i] >= 'a' && token[i] <= 'z') ||
              (token[i] >= '0' && token[i] <= '9')))
        {
            return false;
        }
    }

    return true;
}

int is_directive_line(Tokens tokenized_line)
{
    char dir_prefix;
    char *dir_name;

    if (tokenized_line.tokens[0] == NULL || strlen(tokenized_line.tokens[0]) < 2)
    {
        return false;
    }

    dir_prefix = tokenized_line.tokens[0][0];
    dir_name = &tokenized_line.tokens[0][1];

    if (dir_prefix != '.')
        return false;

    return (strcmp(dir_name, "data") == 0 ||
            strcmp(dir_name, "string") == 0 ||
            strcmp(dir_name, "mat") == 0 ||
            strcmp(dir_name, "entry") == 0 ||
            strcmp(dir_name, "extern") == 0);
}

/*====================================================*/
/*                  Miscellaneous Utils               */
/*====================================================*/

int expect_operands(Opcode opcode)
{
    switch (opcode)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 6:
        return 2;
    case 4:
    case 5:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
        return 1;
    case 14:
    case 15:
        return 0;
    default:
        return -1;
    }
}
