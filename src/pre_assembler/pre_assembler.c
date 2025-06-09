#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pre_assembler.h" // assuming your macro table is in this module

int run_pre_assembler(const char *filename)
{
    /* declares */
    char line[256];

    /* input file with fopen (open a file and create a new stream for it) */
    FILE *input = fopen(filename, "r");

    /* output file with fopen (open a file and create a new stream for it) */
    FILE *output = fopen("output.am", "w");

    if (output == NULL)
    {
        perror("Error opening output file");
        return 1; // or handle error appropriately
    }
    /*check + handle err*/
    if (!input)
    {
        fprintf(stderr, "Error: Cannot open %s for reading.\n", filename);
        return 1;
    }

    /* needs to be checked:
        - macro name is not empty
        - macro name is valid (cant use lexical keywords, follows naming conventions)
        - macro name is not already used
        - no extensive code after <name>
        - no extensive code after mcroend

        assumptions:
        - no nested macros
        - macroend is always used
        */
    /* for input line listen to macro state*/
    while (fgets(line, sizeof(line), input) != NULL)
    {
        /*
            listen:
                macro
                    _start
                        - collect macro name
                        - validate macro name
                            - not empty
                            - not a keyword
                            - not used
                            - no extensive code after (same line)
                        - push macro name to macro table
                        - *set status to _code ->
                    _code
                        - collect code
                            - on first word set status to notempty? (for warning on empty macro)
                        - if mcroend is found
                            - set status to _end ->
                    _end
                        - push macro code to macro_name location in macro table
                        - clear macro object
                        - set status to other ->
                    _other
                        - if macro end is found
                            - ERR: macro end without start
                        - else if macro start is found
                            - set status to _start ->
                        - else if macro call is found
                            - set status to _call ->
                        - else
                            - copy line to output file
                    _call
                        - if this word matching an existing macro name than it is a macro call
                            - expand macro code into output file
                        -set status to _other ->
        */
    }
    /* commented code
    int m_status = M_NONE;

    int is_macro_start = is_macro_start(line);
    int is_macro_end = is_macro_end(line);
    printf("%s", line);

    if (is_macro_start)
    {
        // handle macro start
        char *macro_name = get_macro_name(line);
        if (macro_name)
        {
            printf("Macro start detected: %s\n", macro_name);
            // Here you would typically add the macro to a macro table
            // For example: add_macro_to_table(macro_name);
        }
        else
        {
            fprintf(stderr, "Error: Invalid macro name in line: %s\n", line);
        }
    }
    else if (is_macro_end)
    {
        // handle macro end
        printf("Macro end detected.\n");
    }
    */

    // Close the input file
    fclose(input);
    return 0;
}

/* hold macro name | code*/
typedef struct
{

} data_table_t;

/*__STATUS:*/
typedef enum
{
    M_START = 0,
    M_LINE,
    M_END,
    M_OTHER,
    M_CALL,
} m_status_t;
