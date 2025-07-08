#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "first_pass.h"
#include "../common/AST/ast.h"
#include "../common/table/table.h"
#include "../common/tokenizer/tokenizer.h"
#include "../common/encoding/encoding.h"
#include "../common/utils/utils.h"

#ifdef DEBUG
#define PRINT_DEBUG(fmt, ...)                                                           \
    do                                                                                  \
    {                                                                                   \
        fprintf(stderr, "\033[1;90m[DEBUG]\033[0m %s:%d \033[3m%s()\033[0m: " fmt "\n", \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__);                           \
    } while (0)
#else
#define PRINT_DEBUG(fmt, ...) \
    do                        \
    {                         \
    } while (0)
#endif

/* --- Parsing Output Helpers --- */

#define PRINT_LINE(n) printf("\n\033[1;36m╔════════════════════════════╗\n" \
                             "║        [ Line %3d ]        ║\n"             \
                             "╚════════════════════════════╝\033[0m\n",     \
                             n)

#define PRINT_RAW_LINE(s) printf("  \033[0;37mRaw Line     :\033[0m %s", s)
#define PRINT_TOKEN(t) printf("  \033[0;32mToken        :\033[0m %s\n", t)
#define PRINT_LABEL_FOUND(l) printf("  \033[1;33mLabel Found  :\033[0m %s\n", l)
#define PRINT_LABEL_INSERT(l, a) printf("  \033[0;36mLabel Insert :\033[0m '%s' @ %d\n", l, a)
#define PRINT_LABEL_EXISTS(l) printf("  \033[1;31mLabel Error  :\033[0m '%s' already exists\n", l)
#define PRINT_INSTRUCTION(o) printf("  \033[1;35mInstruction  :\033[0m opcode = %d\n", o)
#define PRINT_DIRECTIVE(s) printf("  \033[1;34mDirective    :\033[0m %s\n", s)
#define PRINT_OPERAND(n, tok) printf("  \033[0;32mOperand %-5d:\033[0m %s\n", n, tok)
#define PRINT_ADDR_MODE(s) printf("  \033[0;36mAddr. Mode   :\033[0m %s\n", s)
#define PRINT_DC(dc) printf("  \033[1;36mData Counter :\033[0m %d\n", dc)

/**
 * A custom implementation of strdup for debugging purposes.
 * It allocates memory with malloc and copies the string content.
 */
char *my_strdup(const char *s)
{
    /* הקצאת זיכרון עבור המחרוזת החדשה + מקום לתו מסיים ('\0') */
    char *new_str = malloc(strlen(s) + 1);

    /* בדיקה שההקצאה הצליחה */
    if (new_str == NULL)
    {
        return NULL;
    }

    /* העתקת תוכן המחרוזת המקורית למחרוזת החדשה */
    strcpy(new_str, s);

    return new_str;
}

char *trim_whitespace(char *str)
{
    static char trimmed[1024];
    int i = 0, j = 0;
    if (!str)
    {
        trimmed[0] = '\0';
        return trimmed;
    }
    while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
        i++;
    for (; str[i] != '\0'; i++)
    {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
            trimmed[j++] = str[i];
    }
    trimmed[j] = '\0';
    return trimmed;
}

StatementType get_statement_type(char *leader)
{
    if (is_instruction_line(leader))
        return INSTRUCTION_STATEMENT;

    else if (is_directive_line(leader))
        return DIRECTIVE_STATEMENT;

    else
        return INVALID_STATEMENT;
}

void run_first_pass(char *filename)
{
    int is_label_declaration = 0;
    int IC = 0, DC = 0;
    Table *symbol_table = table_create();
    FILE *file = fopen(filename, "r");
    char line[1024];
    int line_number = 1;
    Tokens tokenized_line;
    char *leader;
    ASTNode *head = NULL, *tail = NULL;
    if (!file)
    {
        perror("Error opening file");
        return;
    }
    printf("\n\033[1;35mFILENAME:\033[0m %s\n", filename);

    while (fgets(line, sizeof(line), file))
    {
        PRINT_LINE(line_number);
        PRINT_RAW_LINE(line);

        tokenized_line = tokenize_line(line);
        int leader_idx = 0;
        leader = tokenized_line.tokens[0];
        PRINT_TOKEN(leader);
        StatementType statement_type;
        SymbolInfo *symbol_info = malloc(sizeof(SymbolInfo));
        memset(symbol_info, 0, sizeof(SymbolInfo));

        /* ignore non code lines */
        if (is_comment_line(leader) || is_empty_line(tokenized_line))
        {
            line_number++;
            continue;
        }

        /* could be instruction on directive line */
        if (is_symbol_declare(leader))
        {
            is_label_declaration = 1;
            PRINT_LABEL_FOUND(leader);
            int is_declared = table_lookup(symbol_table, leader);
            /* if not declared before add to table */
            if (!is_declared)
            {
                char clean_label[strlen(leader)];
                strncpy(clean_label, leader, strlen(leader) - 1);
                clean_label[strlen(leader) - 1] = '\0';

                *symbol_info->name = clean_label;
                symbol_info->address = 1;
                if (table_insert(symbol_table, clean_label, IC))
                    PRINT_LABEL_INSERT(clean_label, IC);
                else
                    printf("[Insert Error] Failed to insert label\n");
            }
            else
            {
                /* else, handle error */
                PRINT_LABEL_EXISTS(leader);
            }

            /* move leader to next token */
            leader = tokenized_line.tokens[++leader_idx];
            PRINT_TOKEN(leader);
        }

        statement_type = get_statement_type(leader);
        switch (statement_type)
        {
        case INSTRUCTION_STATEMENT:
        {

            if (is_label_declaration)
                symbol_info->type = SYMBOL_CODE;
            Opcode opcode = get_code(leader);
            ASTNode *new_node;
            PRINT_INSTRUCTION(opcode);
            new_node = parse_instruction_line(line_number, tokenized_line, leader_idx);
            append_ast_node(&head, &tail, new_node);
            encode_instruction_line(new_node, leader_idx);

            /* increment instruction counter */
            IC++;
        }
        break;
        case DIRECTIVE_STATEMENT:
        {
            if (is_label_declaration)
                symbol_info->type = SYMBOL_DATA;
            PRINT_DIRECTIVE(leader);
            ASTNode *new_node;
            /*TODO: refactor data count not as side effect*/
            new_node = parse_directive_line(line_number, tokenized_line, leader_idx, &DC);
            if (head == NULL)
            {
                head = tail = new_node;
            }
            else
            {
                tail->next = new_node;
                tail = new_node;
            }

            /* increment data counter */
            /* temporary, TODO token validation for each data token */
        }
        break;
        case INVALID_STATEMENT:
        {
            /* handle error */
        }
        break;
        }

        line_number++;
    }
    free_ast(head);
    fclose(file);
}

DirectiveType get_directive_type(char *dir)
{
    if (strcmp(dir, ".data") == 0)
        return DATA;
    else if (strcmp(dir, ".string") == 0)
        return STRING;
    else if (strcmp(dir, ".mat") == 0)
        return MAT;
    else if (strcmp(dir, ".entry") == 0)
        return ENTRY;
    else if (strcmp(dir, ".extern") == 0)
        return EXTERN;
    else
        return ERROR_DIRECTIVE;
}

ASTNode *parse_instruction_line(int line_num, Tokens tokenized_line, int leader_idx)
{
    /*TODO: memeset*/
    InstructionInfo info;
    Opcode opcode = get_code(tokenized_line.tokens[leader_idx]);
    int expected_num_op = expect_operands(opcode);
    info.opcode = opcode;
    info.src_op.mode = NONE;
    info.dest_op.mode = NONE;
    printf("--> Expected operands: %d\n", expected_num_op);

    switch (expected_num_op)
    {
    case 1:
        PRINT_OPERAND(1, tokenized_line.tokens[leader_idx + 1]);
        parse_operand(&(info.dest_op), tokenized_line, leader_idx + 1);
        break;
    case 2:
        PRINT_OPERAND(1, tokenized_line.tokens[leader_idx + 1]);
        PRINT_OPERAND(2, tokenized_line.tokens[leader_idx + 2]);
        parse_operand(&(info.src_op), tokenized_line, leader_idx + 1);
        parse_operand(&(info.dest_op), tokenized_line, leader_idx + 2);
        break;
    default:
        printf("--> No operands expected.\n");
        break;
    }

    return create_instruction_node(line_num, NULL, info);
}

ASTNode *parse_directive_line(int line_num, Tokens tokenized_line, int leader_idx, int *DC_ptr)
{
    const char *delimeter = ",";
    int data_size = tokenized_line.count - (leader_idx + 1);
    int data_val_idx;

    DirectiveInfo *info = malloc(sizeof(DirectiveInfo));
    if (!info)
    {
        printf("Failed to allocate DirectiveInfo\n");
        return NULL;
    }

    info->type = get_directive_type(tokenized_line.tokens[leader_idx]);

    switch (info->type)
    {
    case DATA:
    {
        /* allocate values array */
        int *values = malloc((sizeof(int)) * data_size);
        int i = 0;
        if (!values)
        {
            printf("Failed to allocate values array\n");
            free(info);
            return NULL;
        }

        /* parese data values (integers), handling errors*/
        for (i = 0; i < data_size; i++)
        {
            data_val_idx = leader_idx + 1 + i;
            char *data_value_token = tokenized_line.tokens[data_val_idx];
            int is_missing_val = strcmp(data_value_token, delimeter) == 0;

            /*TODO: handle error , , empty value ERR CODE*/
            int err = -200;

            /* if token was 2 (or more) adjacent ',' in a row */
            if (is_missing_val)
            {
                printf("ERROR: MISSING VALUE\n");
                /* handle error */
                values[i] = err;
            }
            else if (is_valid_number(data_value_token))
            {
                values[i] = atoi(data_value_token);
                /* increment data counter */
                (*DC_ptr)++;
                PRINT_DC(*DC_ptr);
            }
        }

        /* populate info fields */
        info->params.data.values = values;
        info->params.data.size = data_size;
    }
    break;
    case MAT:
    {
        int i;
        /* validate .mat directive format */
        char *size_row = tokenized_line.tokens[leader_idx + 2];
        char *size_col = tokenized_line.tokens[leader_idx + 5];
        if (!is_valid_number(size_row) || !is_valid_number(size_col))
        {
            /* handle error */
            printf("ERROR: missing row or col size in mat directive ");
            break;
        }

        /* adjust data count */
        data_size = atoi(size_row) * atoi(size_col);

        /* allocate values array */
        int *values = malloc((sizeof(int)) * data_size);
        if (!values)
        {
            printf("Failed to allocate values array\n");
            free(info);
            return NULL;
        }

        /* parese data values (integers), handling errors*/
        for (i = 0; i < data_size; i++)
        {
            int mat_increment = 6;
            int data_val_idx = leader_idx + 1 + i + mat_increment;
            char *data_value_token = tokenized_line.tokens[data_val_idx];
            int is_missing_val = strcmp(data_value_token, delimeter) == 0;

            /*TODO: handle error , , empty value ERR CODE*/
            int err = -200;

            /* if token was 2 (or more) adjacent ',' in a row */
            if (is_missing_val)
            {
                printf("ERROR: MISSING VALUE\n");
                /* handle error */
                values[i] = err;
            }
            else if (is_valid_number(data_value_token))
            {
                values[i] = atoi(data_value_token);
                /* increment data counter */
                (*DC_ptr)++;
                PRINT_DC(*DC_ptr);
            }
        }

        /* populate info fields */
        info->params.data.values = values;
        info->params.data.size = data_size;
    }
    break;
    case STRING:
    {
        int i;
        int data_val_idx = leader_idx + 1;
        char *data_val_token = tokenized_line.tokens[data_val_idx];
        data_size = strlen(data_val_token);
        char delimeter = '"';
        int str_len = data_size - 2;
        char *str_buffer = malloc(str_len + 1);
        if (!str_buffer)
        {
            printf("ERROR: failed to allocate string buffer\n");
            break;
        }

        if (data_size < 2 || data_val_token[0] != '"' || data_val_token[data_size - 1] != '"')
        {
            printf("ERROR: invalid string token: %s\n", data_val_token);
            break;
        }

        for (i = 0; i < str_len; i++)
            str_buffer[i] = data_val_token[i + 1];

        str_buffer[str_len] = '\0';
        (*DC_ptr) += str_len + 1;
        PRINT_DC(*DC_ptr);
        printf("string is: %s", str_buffer);
    }
    break;
    case ENTRY:
    {
    }
    break;
    case EXTERN:
    {
    }
    break;
    case ERROR_DIRECTIVE:
    {
    }
    break;
    }

    info->params.data.size = data_size;
    return create_directive_node(line_num, tokenized_line.tokens[leader_idx], info);
}

/*====================================================*/
/*                 Operand Parsing Utils              */
/*====================================================*/

void parse_operand(Operand *operand_to_parse, Tokens tokenized_line, int token_idx)
{

    printf("Parsing operand at token index %d: %s\n", token_idx, tokenized_line.tokens[token_idx]);

    operand_to_parse->mode = get_mode(tokenized_line, token_idx);
    printf("Detected addressing mode: %s\n", addressing_mode_name(operand_to_parse->mode));

    switch (operand_to_parse->mode)
    {
    case IMMEDIATE:
        operand_to_parse->value.immediate_value = atoi(tokenized_line.tokens[token_idx] + 1);
        printf("Immediate value: %d\n", operand_to_parse->value.immediate_value);
        break;
    case DIRECT:
        operand_to_parse->value.label = my_strdup(tokenized_line.tokens[token_idx]);
        printf("Direct label: %s\n", operand_to_parse->value.label);
        break;
    case REGISTER:
        operand_to_parse->value.reg_num = atoi(tokenized_line.tokens[token_idx] + 1);
        printf("Register number: %d\n", operand_to_parse->value.reg_num);
        break;
    case MAT_ACCESS:
        /* Parse matrix access: label[reg1][reg2] */
        operand_to_parse->value.index.label = my_strdup(tokenized_line.tokens[token_idx]);
        operand_to_parse->value.index.row_reg_num = atoi(tokenized_line.tokens[token_idx + 1] + 1);
        operand_to_parse->value.index.col_reg_num = atoi(tokenized_line.tokens[token_idx + 2] + 1);
        printf("Matrix label: %s, Row register: %d, Col register: %d\n",
               operand_to_parse->value.index.label,
               operand_to_parse->value.index.row_reg_num,
               operand_to_parse->value.index.col_reg_num);
        break;
    case NONE:
    {
        /*TODO: not entering*/
        printf("WE HAVE CASE NONE");
    }
    break;
    default:
        printf("Unknown operand mode.\n");
        break;
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
    while (i < MAX_TOKEN_LEN - 1 && trimmed_value[i] != '[')
    {
        if (trimmed_value[i] == '\0')
        {
            return 0;
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
    /* TODO: hande unvalid register */
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

int is_symbol_declare(char *token)
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
        return 4;
    if (strcmp(str, "clr") == 0)
        return 5;
    if (strcmp(str, "not") == 0)
        return 6;
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

int is_comment_line(char *line)
{
    const char *trimmed;
    if (!line)
        return 0;
    trimmed = trim_whitespace(line);
    return trimmed[0] == ';';
}

int is_empty_line(Tokens tokens)
{
    const char *trimmed;
    if (tokens.count == 0)
        return 1;

    trimmed = trim_whitespace(tokens.tokens[0]);
    return trimmed[0] == '\0';
}
