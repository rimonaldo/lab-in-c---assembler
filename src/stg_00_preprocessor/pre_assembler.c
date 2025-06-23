#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pre_assembler.h"
#include "macro_table.h"
#include "../tokenizer/tokenizer.h"

const Macro *CURR_MACRO = NULL;

/*__STATUS:*/
typedef enum
{
    M_START = 0,
    M_CODE,
    M_END,
    M_OTHER,
    M_CALL,
} MacroState;

int run_pre_assembler(const char *filename)
{
    /* Declarations */
    char line[256];
    MacroState m_state = M_OTHER;
    Tokens tokenized_line;
    char macro_name_buffer[MAX_LINE_LEN];
    char macro_code_buffer[MAX_MACRO_LINES][MAX_LINE_LEN];
    int macro_code_line_count = 0;
    MacroTable table;
    init_macro_table(&table);

    FILE *input = fopen(filename, "r");
    FILE *output = fopen("output.am", "w");

    if (!input)
    {
        fprintf(stderr, "Error: Cannot open %s for reading.\n", filename);
        return 1;
    }

    if (!output)
    {
        perror("Error opening output file");
        fclose(input);
        return 1;
    }

    while (fgets(line, sizeof(line), input) != NULL)
    {
        int i;
        tokenized_line = tokenize_line(line);
        printf("__________________________________________________\n");
        printf("🔍 Processing line: %s \n", line);
        printf("\tToken count: %d\n", tokenized_line.count);

        if (tokenized_line.count == 0)
        {
            if (line[0] == '\n' || (line[0] == '\r' && line[1] == '\n'))
            {
                fputs(line, output);
            }
            continue;
        }

        for (i = 0; i < tokenized_line.count; i++)
        {
            printf("\tToken[%d]: '%s'\n", i, tokenized_line.tokens[i]);
        }
        if (tokenized_line.count == 0)
            continue; /* Skip empty lines */

        switch (m_state)
        {
        case M_CODE:
        {

            if (is_macro_end(tokenized_line.tokens[0]))
            {
                add_macro(&table, macro_name_buffer, macro_code_buffer, macro_code_line_count);
                printf("✅ Macro stored: %s (%d lines)\n", macro_name_buffer, macro_code_line_count);
                macro_name_buffer[0] = '\0';
                macro_code_line_count = 0;
                m_state = M_OTHER;
            }
            else
            {
                strncpy(macro_code_buffer[macro_code_line_count], line, MAX_LINE_LEN - 1);
                macro_code_buffer[macro_code_line_count][MAX_LINE_LEN - 1] = '\0';
                macro_code_line_count++;
            }
        }
        break;
        case M_END:
        {

            add_macro(&table, macro_name_buffer, macro_code_buffer, macro_code_line_count);
            printf("✅ Macro stored: %s (%d lines)\n", macro_name_buffer, macro_code_line_count);
            macro_name_buffer[0] = '\0';
            macro_code_line_count = 0;
            m_state = M_OTHER;
        }
        break;

        case M_CALL:
            printf("\n\n");
            m_state = M_OTHER;
            break;

        case M_OTHER:

            if(is_comment(tokenized_line.tokens[0]))
            {
                printf("Comment detected: %s", line);
                fputs(line, output);
                continue;
            }
            if (is_macro_start(tokenized_line.tokens[0]))
            {
                if (tokenized_line.count < 2)
                {
                    fprintf(stderr, "Error: Macro name missing after 'mcro'.\n");
                    break;
                }

                strncpy(macro_name_buffer, tokenized_line.tokens[1], MAX_LINE_LEN - 1);
                macro_name_buffer[MAX_LINE_LEN - 1] = '\0';
                printf("🔍 Macro name detected: %s\n\n\n", tokenized_line.tokens[1]);

                if (macro_exists(&table, macro_name_buffer))
                {
                    fprintf(stderr, "Error: Macro '%s' redefined.\n", macro_name_buffer);
                    break;
                }

                printf("🟢 Starting macro: %s\n", macro_name_buffer);
                m_state = M_CODE;
                macro_code_line_count = 0;
            }
            else if (is_macro_end(tokenized_line.tokens[0]))
            {
                fprintf(stderr, "Error: Unexpected 'mcroend'\n");
            }
            else if (macro_exists(&table, tokenized_line.tokens[0]))
            {
                printf("🟣 Macro call: %s 🟣\n", tokenized_line.tokens[0]);
                expand_macro(&table, tokenized_line.tokens[0], output);
            }
            else
            {
                printf("🔴 Writing regular line to output: %s", line);
                fputs(line, output);
            }
            break;
        }
    }

    fclose(input);
    fclose(output);
    print_macro_table(&table);
    return 0;
}

int is_macro_start(const char *line)
{
    printf("Checking if line is macro start: %s\n", line);
    return strncmp(line, "mcro", 4) == 0;
}
int is_macro_end(const char *line)
{
    printf("Checking if line is macro end: %s\n", line);
    return strncmp(line, "mcroend", 7) == 0;
}
int is_macro_call(const char *line)
{

    printf("Checking if line is macro call: %s\n", line);
    return get_macro(NULL, line) != NULL ||
           strncmp(line, "mcro", 4) != 0 &&
               strncmp(line, "mcroend", 7) != 0;
}

int macro_exists(const MacroTable *table, const char *name)
{
    printf("Checking if macro exists: %s\n", name);
    CURR_MACRO = get_macro(table, name);
    if (CURR_MACRO != NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int is_comment(const char *line)
{
    printf("Checking if line is a comment: %s\n", line);
    return line[0] == ';' || line[0] == '\n' || (line[0] == '\r' && line[1] == '\n');
}