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

    /*check + handle err*/
    if (!input)
    {
        fprintf(stderr, "Error: Cannot open %s for reading.\n", filename);
        return 1;
    }

    /* for input line listen to macro state*/
    while (fgets(line, sizeof(line), input) != NULL)
    {
        /*
            listen:
                macro
                    _start
                    _code
                    _end
                    _other
                    _call
        */
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
    }

    // Close the input file
    fclose(input);
    return 0;
}

/* hold macro name | code*/
typedef struct {
    
} data_table_t;



/*__STATUS:*/
typedef enum
{
    M_START = 0,
    M_LINE,
    M_END,
    M_CALL,
    M_NONE,
} m_status_t;


