#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "first_pass.h"

char *my_strdup(const char *s);
char *trim_whitespace(char *str);

/* -------------- MAIN DRIVER -------------- */
void run_first_pass(char *filename, StatusInfo *status_info)
{
    int is_label_declaration = 0;
    int IC = 100, DC = 0;
    Table *symbol_table = table_create(), *ext_table = table_create(), *ent_table = table_create();
    FILE *file = fopen(filename, "r");
    char line[1024]; /* move to machine definitions */
    int line_number = 1;
    Tokens tokenized_line;
    char *leader;
    ASTNode *head = NULL, *tail = NULL;
    char *clean_label;
    ErrorInfo err;
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
        if (strlen(line) > MAX_LINE_LEN)
            write_error_log(status_info, E701_MEMORY_LINE_CHAR_LIMIT, line_number);

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

        /* inserts new label to symbol table as: clean_label | symbol_info */
        /* moves leader token, keeps is_label_declaration flag */
        if (is_symbol_declare(leader))
        {
            is_label_declaration = 1;
            PRINT_LABEL_FOUND(leader);
            int is_declared = table_lookup(symbol_table, leader);

            if (is_declared)
                write_error_log(status_info, E502_LABEL_REDEFINED, line_number);
            else
            {
                clean_label = malloc(strlen(leader));
                strncpy(clean_label, leader, strlen(leader) - 1);
                clean_label[strlen(leader) - 1] = '\0';

                *symbol_info->name = clean_label;
            }

            /* move leader to next token */
            leader = tokenized_line.tokens[++leader_idx];
            PRINT_TOKEN(leader);
        }

        /* switch statetment type */
        statement_type = get_statement_type(leader);
        switch (statement_type)
        {
        case INSTRUCTION_STATEMENT:
        {
            EncodedLine *encoded_line = malloc(sizeof(EncodedLine));
            Opcode opcode = get_opcode(leader);
            ASTNode *new_node;
            PRINT_INSTRUCTION(opcode);
            new_node = parse_instruction_line(line_number, tokenized_line, leader_idx);
            if (!new_node)
                break;
            append_ast_node(&head, &tail, new_node);

            if (new_node->status != ERR1)
            {
                encoded_line = encode_instruction_line(new_node, leader_idx);
                if (is_label_declaration >= 0)
                {
                    symbol_info->type = SYMBOL_CODE;
                    symbol_info->address = IC;
                    /* insert to table with IC before Instruction increment as address */
                    if (table_insert(symbol_table, clean_label, symbol_info))
                        PRINT_LABEL_INSERT(clean_label, IC);
                    else
                        printf("[Insert Error] Failed to insert label\n");
                    is_label_declaration = -1;
                }
                /* increment instruction counter */
                IC += encoded_line->words_count;
            }
        }
        break;
        case DIRECTIVE_STATEMENT:
        {
            PRINT_DIRECTIVE(leader);
            int pre_inc_DC = DC; /* Save DC before increment */
            char *label_token;
            /* Parse directive and update DC */
            ASTNode *node = parse_directive_line(line_number, tokenized_line, leader_idx, &DC);
            if (node->status != SUCCESS)
                break;

            append_ast_node(&head, &tail, node); /* Add node to AST */

            symbol_info->type = SYMBOL_DATA;
            symbol_info->is_entry = -1;
            symbol_info->is_extern = -1;

            /* Handle .entry directive */
            if (node->content.directive.type == ENTRY)
            {
                int address = -100;
                char *str_name = tokenized_line.tokens[leader_idx + 1];
                strncpy(clean_label, tokenized_line.tokens[leader_idx + 1], strlen(str_name));

                *symbol_info->name = clean_label;

                void *data = table_lookup(symbol_table, clean_label);
                SymbolInfo *info = (SymbolInfo *)data;

                if (info->name)
                {
                    info->is_entry = 1; /* Mark symbol as entry */
                    address = DC;
                }
                else
                {
                    printf("Symbol %s not found yet\n", clean_label);
                }
                /* Get the label token after directive */
                char *token = tokenized_line.tokens[leader_idx + 1];
                label_token = copy_label_token(token);
                /* Add to entry table with temp -100 address */
                symbol_info->is_entry = 1;

                insert_entry_label(ent_table, label_token, address);
            }
            /* Handle .extern directive */
            else if (node->content.directive.type == EXTERN)
            {
                /* Get the label token after directive */
                char *token = tokenized_line.tokens[leader_idx + 1];
                label_token = copy_label_token(token);
                /* Add to extern table with pre_inc_dc address */
                symbol_info->type = SYMBOL_EXTERN;
                symbol_info->is_extern = 1;
                insert_extern_label(ext_table, label_token, 0);
            }

            /* Insert label if declared or extern */
            if (is_label_declaration >= 0 || symbol_info->is_extern >= 0)
            {
                if (symbol_info->type == SYMBOL_EXTERN)
                    clean_label = label_token;
                /* Warn if .entry or .extern used with label declaration */
                if (is_label_declaration && (symbol_info->type == ENTRY || symbol_info->type == EXTERN))
                    warn("entry or extern used in label declaration");

                symbol_info->address = symbol_info->is_extern > 0 ? 0 : pre_inc_DC;

                if (table_insert(symbol_table, clean_label, symbol_info))
                    PRINT_LABEL_INSERT(clean_label, pre_inc_DC); /* Confirm insertion */
                else
                    printf("[Insert Error] Failed to insert label\n");
            }
        }
        break;
        case INVALID_STATEMENT:
        {
            /* handle error */
            write_error_log(status_info, E600_INSTRUCTION_NAME_INVALID, line_number);
        }
        break;
        }

        line_number++;
    }

    /* tables print */
    printf("\n\033[1;36mSYMBOL TABLE:\033[0m\n");
    table_print(symbol_table, print_symbol);
    printf("\n\033[1;36mEXTERN TABLE:\033[0m\n");
    table_print(ext_table, print_extern);
    printf("\n\033[1;36mENTRY TABLE:\033[0m\n");
    table_print(ent_table, print_entry);
    printf("_____________________________________\n");
    int ICF = IC + 1;
    /* update data memory locations */
    TableNode *current = symbol_table->head;
    SymbolInfo *curr_info = (SymbolInfo *)current;

    int j = 1;
    while (current->next)
    {
        curr_info = current->data;
        printf("%d\n", j++);
        current = current->next;
    }
    /* close and release memory */
    free_ast(head);
    fclose(file);
}

/* -------------- parsers -------------- */
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
    info->status = SUCCESS;
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
                info->status = ERR1;
                values[i] = err;
            }
            else if (is_valid_number(data_value_token))
            {
                values[i] = atoi(data_value_token);
                /* increment data counter */
                (*DC_ptr)++;
            }
        }
        PRINT_DC(*DC_ptr);

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
            info->status = ERR1;
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
                info->status = ERR1;
                break;
            }
            else if (is_valid_number(data_value_token))
            {
                values[i] = atoi(data_value_token);
                /* increment data counter */
            }
            (*DC_ptr)++;
        }

        PRINT_DC(*DC_ptr);
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
    case ERROR_DIRECTIVE:
    {
    }
    break;
    }

    info->params.data.size = data_size;
    return create_directive_node(line_num, tokenized_line.tokens[leader_idx], info);
}

ASTNode *parse_instruction_line(int line_num, Tokens tokenized_line, int leader_idx)
{
    /*TODO: memeset*/
    InstructionInfo info;
    Opcode opcode = get_opcode(tokenized_line.tokens[leader_idx]);
    int expected_num_op = expect_operands(opcode);
    info.opcode = opcode;
    info.src_op.mode = NONE;
    info.dest_op.mode = NONE;
    info.status = SUCCESS;
    printf("--> Expected operands: %d\n", expected_num_op);
    int operands_count = tokenized_line.count - (leader_idx + 2);
    if (expected_num_op != operands_count)
    {
        /* handle error - wrong amount of operands (too few or too many)*/
        return NULL;
    }
    switch (expected_num_op)
    {
    case 1:
        PRINT_OPERAND(1, tokenized_line.tokens[leader_idx + 1]);
        info.status = parse_instruction_operand(&(info.dest_op), tokenized_line, leader_idx + 1);
        break;
    case 2:
        PRINT_OPERAND(1, tokenized_line.tokens[leader_idx + 1]);
        PRINT_OPERAND(2, tokenized_line.tokens[leader_idx + 2]);
        info.status = parse_instruction_operand(&(info.src_op), tokenized_line, leader_idx + 1);
        info.status = parse_instruction_operand(&(info.dest_op), tokenized_line, leader_idx + 2);
        break;
    default:
        printf("--> No operands expected.\n");
        break;
    }

    return create_instruction_node(line_num, NULL, info);
}

Status parse_instruction_operand(Operand *operand_to_parse, Tokens tokenized_line, int token_idx)
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
    {

        /* Parse matrix access: label[reg1][reg2] */
        int str_label_size = 0;
        int i = 0;
        while (tokenized_line.tokens[token_idx][str_label_size] != '[')
            str_label_size++;
        char str_label[str_label_size + 1];
        while (i < str_label_size)
        {
            str_label[i] = tokenized_line.tokens[token_idx][i];
            i++;
        }
        str_label[str_label_size] = '\0';

        operand_to_parse->value.index.label = my_strdup(str_label);
        operand_to_parse->value.index.row_reg_num = tokenized_line.tokens[token_idx][str_label_size + 2] - '0';
        operand_to_parse->value.index.row_reg_num = tokenized_line.tokens[token_idx][str_label_size + 6] - '0';
        printf("Matrix label: %s, Row register: %d, Col register: %d\n",
               operand_to_parse->value.index.label,
               operand_to_parse->value.index.row_reg_num,
               operand_to_parse->value.index.col_reg_num);
    }
    break;
    case NONE:
    {
        /*TODO: not entering*/
        /* handle error */
        return ERR1;
        printf("WE HAVE CASE NONE");
    }
    break;
    }
    return SUCCESS;
}

/* -------------- type vendors -------------- */
StatementType get_statement_type(char *leader_token)
{
    if (is_instruction_line(leader_token))
        return INSTRUCTION_STATEMENT;

    else if (is_directive_line(leader_token))
        return DIRECTIVE_STATEMENT;

    else
        return INVALID_STATEMENT;
}

DirectiveType get_directive_type(char *dir_token)
{
    if (strcmp(dir_token, ".data") == 0)
        return DATA;
    else if (strcmp(dir_token, ".string") == 0)
        return STRING;
    else if (strcmp(dir_token, ".mat") == 0)
        return MAT;
    else if (strcmp(dir_token, ".entry") == 0)
        return ENTRY;
    else if (strcmp(dir_token, ".extern") == 0)
        return EXTERN;
    else
        return ERROR_DIRECTIVE;
}

Opcode get_opcode(char *opcode_token)
{

    PRINT_DEBUG("getting opcode\n");
    int res;
    if (strcmp(opcode_token, "mov") == 0)
        return 0;
    if (strcmp(opcode_token, "cmp") == 0)
        return 1;
    if (strcmp(opcode_token, "add") == 0)
        return 2;
    if (strcmp(opcode_token, "sub") == 0)
        return 3;
    if (strcmp(opcode_token, "lea") == 0)
        return 4;
    if (strcmp(opcode_token, "clr") == 0)
        return 5;
    if (strcmp(opcode_token, "not") == 0)
        return 6;
    if (strcmp(opcode_token, "inc") == 0)
        return 7;
    if (strcmp(opcode_token, "dec") == 0)
        return 8;
    if (strcmp(opcode_token, "jmp") == 0)
        return 9;
    if (strcmp(opcode_token, "bne") == 0)
        return 10;
    if (strcmp(opcode_token, "red") == 0)
        return 11;
    if (strcmp(opcode_token, "prn") == 0)
        return 12;
    if (strcmp(opcode_token, "jsr") == 0)
        return 13;
    if (strcmp(opcode_token, "rts") == 0)
        return 14;
    if (strcmp(opcode_token, "stop") == 0)
        return 15;
    return -1;
}

/* -------------- statement recognition -------------- */
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

int is_instruction_line(char *leader)
{
    Opcode opcode = get_opcode(leader);   /* get_code אמור להחזיר -1 אם לא חוקי */
    return (opcode >= 0 && opcode <= 15); /* כל אופקוד חוקי בתחום הזה */
}

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

int is_symbol_declare(char *token)
{
    int len = strlen(token);
    return (is_valid_label_name(token) && token[len - 1] == ':');
}

/* -------------- validators -------------- */
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
              (token[i] >= '0' && token[i] <= '9') ||
              (token[i] == '_')))
        {
            return false;
            /*handle error */
        }
    }

    return true;
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

/* -------------- ....... -------------- */

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
    case NONE:
        return "NONE";
    default:
        return "UNKNOWN";
    }
}

char *copy_label_token(char *token)
{
    char *copy = malloc(strlen(token) + 1);
    strcpy(copy, token);
    return copy;
}

void insert_entry_label(Table *ent_table, char *label, int address)
{
    int *addr = malloc(sizeof(int));
    if (!addr)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    *addr = address; /* Store the value in allocated memory */
    table_insert(ent_table, label, addr);
}

void insert_extern_label(Table *ext_table, char *label, int address)
{

    int *addr = malloc(sizeof(int));
    *addr = address;
    table_insert(ext_table, label, addr);
}

void set_directive_flags(ASTNode *node, SymbolInfo *info)
{
    info->type = SYMBOL_DATA;
    info->is_entry = -1;
    info->is_extern = -1;

    if (node->content.directive.type == ENTRY)
        info->is_entry = 1;
    if (node->content.directive.type == EXTERN)
    {
        info->type = SYMBOL_EXTERN;
        info->is_extern = 1;
    }
}