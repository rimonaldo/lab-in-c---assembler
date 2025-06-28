#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "tokenizer.h"

Tokens tokenize_line(const char *line)
{
    Tokens result;
    memset(&result, 0, sizeof(Tokens));
    int i = 0, len = strlen(line);

    while (i < len && result.count < MAX_TOKENS)
    {
        int j = 0; /* Reset j for each token */

        /* Skip whitespace and commas */
        while (i < len && (isspace(line[i]) || line[i] == ','))
            i++;

        /* If we hit a comment, store the rest of the line as a comment token */
        if (i < len && line[i] == ';')
        {
            j = 0;
            while (i < len && j < MAX_TOKEN_LEN - 1)
                result.tokens[result.count][j++] = line[i++];
            result.tokens[result.count][j] = '\0';
            result.count++;
            break;
        }

        /* End of line */
        if (i >= len)
            break;

        /* Handle special single-char tokens as standalone */
        if (strchr("[]()", line[i]))
        {
            result.tokens[result.count][0] = line[i];
            result.tokens[result.count][1] = '\0';
            result.count++;
            i++;
            continue;
        }

        /* Collect characters for a normal token */
        while (
            i < len &&
            !isspace(line[i]) &&
            line[i] != ',' &&
            line[i] != ';' &&
            !strchr("[]()", line[i]) &&
            j < MAX_TOKEN_LEN - 1)
        {
            result.tokens[result.count][j++] = line[i++];
        }
        result.tokens[result.count][j] = '\0';
        result.count++;
    }

    return result;
}