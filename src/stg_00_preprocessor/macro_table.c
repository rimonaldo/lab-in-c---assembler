#include <string.h>
#include <stdio.h>
#include "macro_table.h"
#include "../common/errors/errors.h"

#define MAX_MACRO_NAME_LEN 31
#define MAX_MACRO_LINES 100

void init_macro_table(MacroTable *table)
{
    table->count = 0;
}

int add_macro(MacroTable *table, const char *name, char lines[MAX_LINES_PER_MACRO][MAX_LINE_LEN], int line_count)
{
    printf("Adding macro: %s with %d lines\n", name, line_count);

    int i;
    if (table->count >= MAX_MACROS)
    {
        fprintf(stderr, "Error: Macro table full\n");
        return 0;
    }

    Macro *macro = &table->macros[table->count];
    strncpy(macro->name, name, MAX_MACRO_NAME_LEN - 1);
    macro->name[MAX_MACRO_NAME_LEN - 1] = '\0';
    macro->line_count = line_count;

    for (i = 0; i < line_count && i < MAX_MACRO_LINES; ++i)
    {
        strncpy(macro->lines[i], lines[i], MAX_LINE_LEN - 1);
        macro->lines[i][MAX_LINE_LEN - 1] = '\0';
    }

    table->count++;
    return 1;
}

int add_macro_line(MacroTable *table, const char *line)
{
    if (table->count == 0)
        return 0; /*No macro to add to*/

    Macro *current = &table->macros[table->count - 1];
    if (current->line_count >= MAX_LINES_PER_MACRO)
        return 0; /*Too many lines*/

    strncpy(current->lines[current->line_count], line, MAX_LINE_LEN - 1);
    current->lines[current->line_count][MAX_LINE_LEN - 1] = '\0';
    current->line_count++;
    return 1; /*Success*/
}

const int get_macro_idx(const MacroTable *table, const char *name)
{
    int i;
    for (i = 0; i < table->count; i++)
    {
        printf("\t🔍 🔍comparing %s with: %s\n", name, table->macros[i].name);

        if (strcmp(table->macros[i].name, name) == 0)
        {
            if (table->macros[i].line_count == 0)
            {
                fprintf(stderr, "⚠️  Warning: Macro '%s' has no lines\n", table->macros[i].name);
            }
            return i;
        }
    }
    return -1;
}

const Macro *get_macro(const MacroTable *table, const char *name)
{
    int i;
    for (i = 0; i < table->count; i++)
    {
        printf("\t🔍 🔍comparing %s with: %s\n", name, table->macros[i].name);

        if (strcmp(table->macros[i].name, name) == 0)
        {
            if (table->macros[i].line_count == 0)
            {
                fprintf(stderr, "⚠️  Warning: Macro '%s' has no lines\n", table->macros[i].name);
            }
            return &table->macros[i];
        }
    }
    return NULL;
}

void expand_macro(MacroTable *table, const char *name, FILE *output)
{
    int i, j;
    for (i = 0; i < table->count; ++i)
    {
        if (strcmp(table->macros[i].name, name) == 0)
        {
            if (table->macros[i].line_count == 0)
            {
                printf("🔵 Macro '%s' is empty, expanding to nothing.\n", name);
                return;
            }

            for (j = 0; j < table->macros[i].line_count; ++j)
            {
                printf("📝 Writing macro line to output: %s", table->macros[i].lines[j]);
                fputs(table->macros[i].lines[j], output);
            }
            return;
        }
    }
    fprintf(stderr, "Warning: Macro '%s' not found\n", name);
}

void print_macro_table(const MacroTable *table)
{
    int i, j;
    printf("Macro Table:\n");
    for (i = 0; i < table->count; ++i)
    {
        printf("Macro: %s\n", table->macros[i].name);
        for (j = 0; j < table->macros[i].line_count; ++j)
        {
            printf("  Line %d: %s", j + 1, table->macros[i].lines[j]);
        }
    }
}