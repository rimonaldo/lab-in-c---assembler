#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "preprocessor.h"
#include "macro_table.h"
#include "../common/tokenizer/tokenizer.h"
#include "../common/utils/file_utils.h"

/*-----------------------------------------------------------
    Macro expansion pre-assembler module
    Processes input file, expands macros, and writes output
------------------------------------------------------------*/

/*-------------------------
    Constants and Globals
--------------------------*/

#define MAX_LINE_LEN 256
#define MAX_MACRO_LINES 100

/* Used to store the macro currently found (optional global) */
const Macro *CURR_MACRO = NULL;

/*-------------------------
    Internal States
--------------------------*/

/* Enum representing current macro processing state */
typedef enum
{
    M_START = 0,
    M_CODE,
    M_END,
    M_OTHER,
    M_CALL
} MacroState;

/*-------------------------
    Helper Functions
--------------------------*/

/* Returns 1 if token is 'mcro' */
int is_macro_start(const char *token)
{
    return strncmp(token, "mcro", 4) == 0;
}

/* Returns 1 if token is 'mcroend' */
int is_macro_end(const char *token)
{
    return strncmp(token, "mcroend", 7) == 0;
}

/* Returns 1 if token is a comment line */
int is_comment(const char *token)
{
    return token[0] == ';' || token[0] == '\n' ||
           (token[0] == '\r' && token[1] == '\n');
}

/* Checks if macro exists in table and sets CURR_MACRO */
int macro_exists(const MacroTable *table, const char *name)
{
    CURR_MACRO = get_macro(table, name);
    return (CURR_MACRO != NULL);
}

/*-------------------------
    Main Pre-Assembler API
--------------------------*/

int run_pre_assembler(const char *input_path)
{
    char line[MAX_LINE_LEN];
    char macro_name_buffer[MAX_LINE_LEN];
    char macro_code_buffer[MAX_MACRO_LINES][MAX_LINE_LEN];
    int macro_code_line_count = 0;
    MacroState m_state = M_OTHER;
    Tokens tokenized_line;
    MacroTable table;

    FILE *input = NULL;
    FILE *output = NULL;
    char base_name[PATH_MAX];
    char output_path[PATH_MAX];

    /* Initialize macro table */
    init_macro_table(&table);

    /* Prepare output path based on input filename */
    if (extract_basename_no_ext(input_path, base_name, sizeof(base_name)) != 0)
    {
        fprintf(stderr, "Error extracting basename from: %s\n", input_path);
        return 1;
    }

    if (ensure_directory_exists("output") != 0)
    {
        fprintf(stderr, "Error: cannot create or access 'output/' directory\n");
        return 1;
    }

    make_output_path(base_name, output_path, sizeof(output_path));

    /* Open input and output files */
    input = fopen(input_path, "r");
    if (!input)
    {
        fprintf(stderr, "Error: Cannot open %s for reading.\n", input_path);
        return 1;
    }

    output = fopen(output_path, "w");
    if (!output)
    {
        fprintf(stderr, "Error: Cannot open %s for writing.\n", output_path);
        fclose(input);
        return 1;
    }

    printf("🔧 Preprocessing: %s → %s\n", input_path, output_path);

    /* Process file line-by-line */
    while (fgets(line, sizeof(line), input) != NULL)
    {
        int i;
        tokenized_line = tokenize_line(line);

        if (tokenized_line.count == 0)
        {
            /* Preserve blank lines */
            if (line[0] == '\n' || (line[0] == '\r' && line[1] == '\n'))
            {
                fputs(line, output);
            }
            continue;
        }

        switch (m_state)
        {
        case M_CODE:
            /* Collecting lines inside macro body */
            if (is_macro_end(tokenized_line.tokens[0]))
            {
                add_macro(&table, macro_name_buffer, macro_code_buffer, macro_code_line_count);
                macro_code_line_count = 0;
                macro_name_buffer[0] = '\0';
                m_state = M_OTHER;
            }
            else
            {
                strncpy(macro_code_buffer[macro_code_line_count], line, MAX_LINE_LEN - 1);
                macro_code_buffer[macro_code_line_count][MAX_LINE_LEN - 1] = '\0';
                macro_code_line_count++;
            }
            break;

        case M_OTHER:
            /* Handle comments and macro declarations */
            if (is_comment(tokenized_line.tokens[0]))
            {
                fputs(line, output);
            }
            else if (is_macro_start(tokenized_line.tokens[0]))
            {
                if (tokenized_line.count < 2)
                {
                    fprintf(stderr, "Error: Macro name missing after 'mcro'\n");
                    break;
                }

                strncpy(macro_name_buffer, tokenized_line.tokens[1], MAX_LINE_LEN - 1);
                macro_name_buffer[MAX_LINE_LEN - 1] = '\0';

                if (macro_exists(&table, macro_name_buffer))
                {
                    fprintf(stderr, "Error: Macro '%s' redefined.\n", macro_name_buffer);
                    break;
                }

                m_state = M_CODE;
                macro_code_line_count = 0;
            }
            else if (is_macro_end(tokenized_line.tokens[0]))
            {
                fprintf(stderr, "Error: Unexpected 'mcroend'\n");
            }
            else if (macro_exists(&table, tokenized_line.tokens[0]))
            {
                expand_macro(&table, tokenized_line.tokens[0], output);
            }
            else
            {
                fputs(line, output);
            }
            break;

        default:
            break;
        }
    }

    /* Cleanup */
    fclose(input);
    fclose(output);
    print_macro_table(&table);
    return 0;
}
