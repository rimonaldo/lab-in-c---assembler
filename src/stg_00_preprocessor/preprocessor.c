#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "preprocessor.h"
#include "macro_table.h"
#include "../common/tokenizer/tokenizer.h"
#include "../common/utils/file_utils.h"
#include "../common/errors/errors.h"

/*-----------------------------------------------------------
    Macro expansion pre-assembler module
    Processes input file, expands macros, and writes output
------------------------------------------------------------*/

/*-------------------------
    Constants and Globals
--------------------------*/

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

int run_pre_assembler(const char *input_path, StatusInfo *status_info)
{
    FILE *input = NULL, *output = NULL;
    char line[MAX_LINE_LEN];
    char macro_name[MAX_LINE_LEN];
    char macro_lines[MAX_LINES_PER_MACRO][MAX_LINE_LEN];
    int macro_line_count = 0;
    int line_number = 1;

    MacroTable table;
    MacroState state = M_OTHER;

    char base_name[PATH_MAX];
    char output_path[PATH_MAX];

    /* Initialize macro table */
    init_macro_table(&table);

    /* Prepare output path */
    if (extract_basename_no_ext(input_path, base_name, sizeof(base_name)) != 0)
    {
        fprintf(stderr, "❌ Failed to extract basename from: %s\n", input_path);
        return 1;
    }

    if (ensure_directory_exists("output") != 0)
    {
        fprintf(stderr, "❌ Failed to create or access 'output/' directory\n");
        return 1;
    }

    make_output_path(base_name, output_path, sizeof(output_path));

    /* Open files */
    input = fopen(input_path, "r");
    if (!input)
    {
        fprintf(stderr, "❌ Cannot open file for reading: %s\n", input_path);
        return 1;
    }

    output = fopen(output_path, "w");
    if (!output)
    {
        fprintf(stderr, "❌ Cannot open file for writing: %s\n", output_path);
        fclose(input);
        return 1;
    }

    printf("🔧 Preprocessing: %s → %s\n", input_path, output_path);

    /* Process line by line */
    while (fgets(line, sizeof(line), input) != NULL)
    {
        Tokens tokens = tokenize_line(line);

        if (tokens.count == 0)
        {
            /* Preserve blank lines */
            fputs(line, output);
            line_number++;
            continue;
        }

        const char *first = tokens.tokens[0];

        if (state == M_CODE)
        {
            if (is_macro_end(first))
            {
                /* End of macro */
                if (macro_line_count == 0)
                    write_error_log(status_info, W404_MACRO_EMPTY, -line_number);

                add_macro(&table, macro_name, macro_lines, macro_line_count);

                macro_line_count = 0;
                macro_name[0] = '\0';
                state = M_OTHER;
            }
            else
            {
                /* Accumulate macro body */
                if (macro_line_count < MAX_LINES_PER_MACRO)
                {
                    strncpy(macro_lines[macro_line_count], line, MAX_LINE_LEN - 1);
                    macro_lines[macro_line_count][MAX_LINE_LEN - 1] = '\0';
                    macro_line_count++;
                }
                else
                {
                    write_error_log(status_info, E400_MACRO_UNDEFINED, line_number);
                }
            }
        }
        else if (state == M_OTHER)
        {
            if (is_comment(first))
            {
                fputs(line, output);
            }
            else if (is_macro_start(first))
            {
                if (tokens.count < 2 || is_macro_end(tokens.tokens[1]) || tokens.tokens[1][0] == '\0')
                {
                    write_error_log(status_info, W402_MACRO_UNNAMED, line_number);
                    state = M_CODE;
                    macro_line_count = 0;
                    continue;
                }

                strncpy(macro_name, tokens.tokens[1], MAX_LINE_LEN - 1);
                macro_name[MAX_LINE_LEN - 1] = '\0';

                if (macro_exists(&table, macro_name))
                {
                    write_error_log(status_info, W403_MACRO_REDEFINED, line_number);
                    continue;
                }

                macro_line_count = 0;
                state = M_CODE;
            }
            else if (is_macro_end(first))
            {
                write_error_log(status_info, W402_MACRO_UNNAMED, line_number);
            }
            else if (macro_exists(&table, first))
            {
                Macro *macro = get_macro(&table, first);
                if (macro->line_count == 0)
                    write_error_log(status_info, W404_MACRO_EMPTY, line_number);

                expand_macro(&table, first, output);
            }
            else
            {
                fputs(line, output);
            }
        }

        line_number++;
    }

    fclose(input);
    fclose(output);

    /* Optional: print valid macros only */
    printf("\n📦 Macro Table:\n");
    int i,j;
    for (i = 0; i < table.count; i++)
    {
        Macro *macro = &table.macros[i];
        if (macro->name[0] == '\0')
            continue;

        printf("Macro: %s\n", macro->name);
        for ( j = 0; j < macro->line_count; j++)
        {
            printf("  Line %d: %s", j + 1, macro->lines[j]);
        }
        if (macro->line_count == 0)
            printf("  (empty)\n");
    }

    return 0;
}
