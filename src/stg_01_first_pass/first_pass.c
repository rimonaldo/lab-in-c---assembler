#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "first_pass.h"
#include "../common/AST/ast.h"
#include "../common/table/table.h"
#include "../common/tokenizer/tokenizer.h"

#ifdef DEBUG
#define PRINT_DEBUG(fmt, ...)                                 \
    do                                                        \
    {                                                         \
        fprintf(stderr, "[DEBUG] %s:%d:%s(): " fmt "\n",      \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    } while (0)
#else
#define PRINT_DEBUG(fmt, ...) do {} while (0)
#endif

#define PRINT_LINE(n)           printf("\n\033[1;36m[Line %d]\033[0m\n", n)
#define PRINT_RAW_LINE(s)      printf("Raw:     %s", s)
#define PRINT_TOKEN(t)         printf("Token:   %s\n", t)
#define PRINT_LABEL_FOUND(l)   printf("\033[1;33mLabel Found:\033[0m %s\n", l)
#define PRINT_LABEL_INSERT(l,a) printf("Inserted Label: '%s' @ %d\n", l, a)
#define PRINT_LABEL_EXISTS(l)  printf("\033[1;31mLabel Error:\033[0m '%s' already exists\n", l)
#define PRINT_INSTRUCTION(o)   printf("Instruction Detected: opcode = %d\n", o)
#define PRINT_DIRECTIVE(s)     printf("Directive Detected: %s\n", s)
#define PRINT_OPERAND(n,tok)   printf("Operand %d: %s\n", n, tok)
#define PRINT_ADDR_MODE(s)     printf("Addressing Mode: %s\n", s)

char *trim_whitespace(char *str)
{
    static char trimmed[1024];
    int i = 0, j = 0;
    if (!str) { trimmed[0] = '\0'; return trimmed; }
    while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n') i++;
    for (; str[i] != '\0'; i++) {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
            trimmed[j++] = str[i];
    }
    trimmed[j] = '\0';
    return trimmed;
}

void run_first_pass(char *filename)
{
    int IC = 0, DC = 0;
    Table *symbol_table = table_create();
    FILE *file = fopen(filename, "r");
    char line[1024];
    int line_number = 1;
    Tokens tokenized_line;
    char *leader;

    if (!file) { perror("Error opening file"); return; }
    printf("\n\033[1;35mFILENAME:\033[0m %s\n", filename);

    while (fgets(line, sizeof(line), file))
    {
        PRINT_LINE(line_number);
        PRINT_RAW_LINE(line);

        tokenized_line = tokenize_line(line);
        leader = tokenized_line.tokens[0];
        PRINT_TOKEN(leader);

        if (is_label_declare(leader))
        {
            PRINT_LABEL_FOUND(leader);
            if (!table_lookup(symbol_table, leader))
            {
                char clean_label[strlen(leader)];
                strncpy(clean_label, leader, strlen(leader) - 1);
                clean_label[strlen(leader) - 1] = '\0';
                if (table_insert(symbol_table, clean_label, IC))
                    PRINT_LABEL_INSERT(clean_label, IC);
                else
                    printf("[Insert Error] Failed to insert label\n");
            }
            else PRINT_LABEL_EXISTS(leader);
            leader = tokenized_line.tokens[1];
            PRINT_TOKEN(leader);
        }

        if (is_instruction_line(leader))
        {
            Opcode opcode = get_code(leader);
            PRINT_INSTRUCTION(opcode);
            parse_instruction_line(line_number, DC, tokenized_line);
        }
        else if (is_directive_line(leader))
        {
            PRINT_DIRECTIVE(leader);
            parse_directive_line(DC, tokenized_line);
        }

        line_number++;
    }
    fclose(file);
}

ASTNode *parse_instruction_line(int line_num, int DC, Tokens tokenized_line)
{
    InstructionInfo *info = malloc(sizeof(InstructionInfo));
    Opcode opcode = get_code(tokenized_line.tokens[0]);
    int expected_num_op = expect_operands(opcode);
    info->opcode = opcode;

    printf("--> Expected operands: %d\n", expected_num_op);

    switch (expected_num_op)
    {
    case 1:
        PRINT_OPERAND(1, tokenized_line.tokens[1]);
        parse_operand(&(info->dest_op), tokenized_line, 1);
        break;
    case 2:
        PRINT_OPERAND(1, tokenized_line.tokens[1]);
        PRINT_OPERAND(2, tokenized_line.tokens[2]);
        parse_operand(&(info->src_op), tokenized_line, 1);
        parse_operand(&(info->dest_op), tokenized_line, 2);
        break;
    default:
        printf("--> No operands expected.\n");
        break;
    }

    return create_instruction_node(line_num, NULL, *info);
}

ASTNode *parse_directive_line(int line_num, Tokens tokenized_line)
{
    DirectiveInfo *info;
    info = malloc(sizeof(DirectiveInfo));
    if (!info)
    {
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
    char *mode_str = addressing_mode_name(operand_to_parse->mode);
    printf("Detected addressing mode: %s\n", mode_str);

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

    if (is_valid_mat_access(value))
        return MAT_ACCESS;
    else if (is_valid_number_operand(value))
        return IMMEDIATE;
    else if (is_valid_register(value))
        return REGISTER;
    else if (is_valid_label_name(value))
        return DIRECT;
    else
    {
        fprintf(stderr, "Warning: Unknown operand format: '%s'. Assuming DIRECT.\n", value);
        return DIRECT;
    }
}

int is_valid_number_operand(char *value)
{
    return value[0] == '#';
}

int is_valid_mat_access(char *value)
{
    /* trimmed copy of value */
    char *trimmed_value = trim_whitespace(value);
    char temp[MAX_TOKEN_LEN];
    char temp_reg1[3];
    char temp_reg2[3];
    int i = 0;

    /* copy name, stop at opening '[' */
    while (trimmed_value[i] != '[')
    {
        if (value[i] == '\0')
        {
            return 0;
            /* handle error */
        }
        temp[i] = trimmed_value[i];
        i++;
    }

    /* check if valid label name */
    temp[i] = '\0';
    if (!is_valid_label_name(temp))
    {
        return 0;
        /* handle error */
    }

    /* check valid register */
    temp_reg1[0] = trimmed_value[i + 1];
    temp_reg1[1] = trimmed_value[i + 2];
    temp_reg1[2] = '\0';
    if (!is_valid_register(temp_reg1))
    {
        return 0;
        /* handle error */
    }
    /* check for closing ']' */
    if (trimmed_value[i + 3] != ']')
    {
        return 0;
        /* handle error */
    }
    /* check opening '[' */
    if (trimmed_value[i + 4] != '[')
    {
        return 0;
        /* handle error */
    }

    /* check valid register */
    temp_reg2[0] = trimmed_value[i + 5];
    temp_reg2[1] = trimmed_value[i + 6];
    temp_reg2[2] = '\0';
    if (!is_valid_register(temp_reg2))
    {
        return 0;
        /* handle error */
    }

    /* check for closing ']' */
    if (trimmed_value[i + 7] != ']')
    {
        return 0;
        /* handle error */
    }

    return 1;
}

int is_valid_register(char *value)
{
    return value[0] == 'r' &&
           value[1] >= '0' && value[1] <= '7' &&
           value[2] == '\0';
}

const char *addressing_mode_name(AddressingMode mode)
{
    switch (mode)
    {
    case IMMEDIATE:
        return "IMMEDIATE (number)";
    case DIRECT:
        return "DIRECT (label)";
    case REGISTER:
        return "REGISTER";
    case MAT_ACCESS:
        return "MAT_ACCESS";
    default:
        return "UNKNOWN";
    }
}

/*====================================================*/
/*                Label & Directive Utils             */
/*====================================================*/

int is_label_declare(char *token)
{
    int len = strlen(token);
    return (is_valid_label_name(token) && token[len - 1] == ':');
}

int is_valid_label_name(char *token)
{
    int i;
    int len;

    if (!token)
    {
        /* handle error*/
        return false;
    }

    len = strlen(token);

    if (len == 0 || len > 31)
    {
        return false;
        /* handle error*/
    }

    if (!((token[0] >= 'A' && token[0] <= 'Z') ||
          (token[0] >= 'a' && token[0] <= 'z')))
    {
        return false;
        /* handle error */
    }

    for (i = 1; i < len - 1; i++)
    {
        if (!((token[i] >= 'A' && token[i] <= 'Z') ||
              (token[i] >= 'a' && token[i] <= 'z') ||
              (token[i] >= '0' && token[i] <= '9')))
        {
            return false;
            /*handle error */
        }
    }

    return true;
}

int is_instruction_line(char *leader)
{
    Opcode opcode = get_code(leader);     /* get_code אמור להחזיר -1 אם לא חוקי */
    return (opcode >= 0 && opcode <= 15); /* כל אופקוד חוקי בתחום הזה */
}

Opcode get_code(char *str)
{

    PRINT_DEBUG("getting opcode\n");
    int res;
    if (strcmp(str, "mov") == 0)
        return 0;
    if (strcmp(str, "cmp") == 0)
        return 1;
    if (strcmp(str, "add") == 0)
        return 2;
    if (strcmp(str, "sub") == 0)
        return 3;
    if (strcmp(str, "lea") == 0)
        return 6;
    if (strcmp(str, "clr") == 0)
        return 4;
    if (strcmp(str, "not") == 0)
        return 5;
    if (strcmp(str, "inc") == 0)
        return 7;
    if (strcmp(str, "dec") == 0)
        return 8;
    if (strcmp(str, "jmp") == 0)
        return 9;
    if (strcmp(str, "bne") == 0)
        return 10;
    if (strcmp(str, "red") == 0)
        return 11;
    if (strcmp(str, "prn") == 0)
        return 12;
    if (strcmp(str, "jsr") == 0)
        return 13;
    if (strcmp(str, "rts") == 0)
        return 14;
    if (strcmp(str, "stop") == 0)
        return 15;
    return -1;
}

int is_directive_line(char *leader)
{
    char dir_prefix;
    char *dir_name;

    if (leader == NULL || strlen(leader) < 2)
    {
        return false;
    }

    dir_prefix = leader[0];
    dir_name = &leader[1];

    if (dir_prefix != '.')
        return false;

    return is_valid_directive_name(leader);
}

int is_valid_directive_name(char *directive)
{
    char *dir_name = directive + 1;
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
