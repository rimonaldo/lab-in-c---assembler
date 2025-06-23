#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H

#define MAX_MACROS 100
#define MAX_MACRO_NAME 31
#define MAX_LINES_PER_MACRO 100
#define MAX_LINE_LEN 80


typedef struct
{
    char name[MAX_MACRO_NAME];
    char lines[MAX_LINES_PER_MACRO][MAX_LINE_LEN];
    int line_count;
} Macro;

typedef struct
{
    Macro macros[MAX_MACROS];
    int count;
} MacroTable;

/* Initialize macro table */
void init_macro_table(MacroTable *table);

/* Add a new macro by name. Returns 1 on success, 0 if full, -1 if duplicate */
int add_macro(MacroTable *table, const char *name, char lines[][MAX_LINE_LEN], int line_count);

/* Add a line to the most recently added macro. Returns 1 on success, 0 on overflow */
int add_macro_line(MacroTable *table, const char *line);

/* Get a pointer to a macro by name, or NULL if not found */
const Macro *get_macro(const MacroTable *table, const char *name);

void expand_macro(MacroTable *table, const char *name, FILE *output);

void print_macro_table(const MacroTable *table);

#endif
