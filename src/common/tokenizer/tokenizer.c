#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "tokenizer.h"

Tokens tokenize_line(const char *line)
{
    Tokens result;
    memset(&result, 0, sizeof(Tokens));
    int i = 0, len = strlen(line);
    int expecting_value = 0;

    while (i < len && result.count < MAX_TOKENS)
    {
        int j = 0;

        /* Skip whitespace */
        while (i < len && isspace(line[i]))
            i++;

        /* Handle comment */
        if (i < len && line[i] == ';')
        {
            j = 0;
            while (i < len && j < MAX_TOKEN_LEN - 1)
                result.tokens[result.count][j++] = line[i++];
            result.tokens[result.count][j] = '\0';
            result.count++;
            break;
        }

        /* If current char is ',' */
        if (i < len && line[i] == ',')
        {
            if (expecting_value)
            {
                strcpy(result.tokens[result.count++], ",");
            }
            expecting_value = 1; 
            i++;
            continue;
        }

        /* Handle special one-char tokens */
        if (i < len && strchr("[]()", line[i]))
        {
            result.tokens[result.count][0] = line[i];
            result.tokens[result.count][1] = '\0';
            result.count++;
            i++;
            expecting_value = 0;
            continue;
        }

        /* If we reach here, it's a real token */
        j = 0;
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
        expecting_value = 0;
    }

    /* סוף שורה נגמרה בפסיק? סימן שחסרה מחרוזת אחרי */
    if (expecting_value && result.count < MAX_TOKENS)
    {
        strcpy(result.tokens[result.count++], "");
    }

    return result;
}
