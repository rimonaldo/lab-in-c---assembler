#include <ctype.h>
#include <stdlib.h>

int is_valid_number(char *s)
{
    if (s == 0 || *s == '\0')
        return 0;

    if (*s == '-' || *s == '+')
    {
        s++;
        if (*s == '\0')
            return 0;
    }

    while (*s)
    {
        if (*s < '0' || *s > '9')
            return 0;
        s++;
    }

    return 1;
}

int is_valid_num_char(char c)
{
    return (c >= '0' && c <= '9');
}

/**
 * A custom implementation of strdup for debugging purposes.
 * It allocates memory with malloc and copies the string content.
 */
char *my_strdup(const char *s)
{
    /* הקצאת זיכרון עבור המחרוזת החדשה + מקום לתו מסיים ('\0') */
    char *new_str = malloc(strlen(s) + 1);

    /* בדיקה שההקצאה הצליחה */
    if (new_str == NULL)
    {
        return NULL;
    }

    /* העתקת תוכן המחרוזת המקורית למחרוזת החדשה */
    strcpy(new_str, s);

    return new_str;
}

char *trim_whitespace(char *str)
{
    static char trimmed[1024];
    int i = 0, j = 0;
    if (!str)
    {
        trimmed[0] = '\0';
        return trimmed;
    }
    while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
        i++;
    for (; str[i] != '\0'; i++)
    {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
            trimmed[j++] = str[i];
    }
    trimmed[j] = '\0';
    return trimmed;
}
