#include <ctype.h>

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