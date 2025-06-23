#include "file_utils.h"
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <direct.h> /* mkdir Windows */
#define mkdir(dir, mode) _mkdir(dir)
#endif

int extract_basename_no_ext(const char *input_path, char *output_basename, size_t max_len)
{
    if (!input_path || !output_basename || max_len == 0)
        return -1;

    /* חיפוש התו '/' או '\' בתלות במערכת */
    const char *base = strrchr(input_path, '/');
#ifdef _WIN32
    const char *base_win = strrchr(input_path, '\\');
    if (base_win && (!base || base_win > base))
        base = base_win;
#endif

    base = base ? base + 1 : input_path;

    /* העתק את השם (כולל סיומת) */
    strncpy(output_basename, base, max_len - 1);
    output_basename[max_len - 1] = '\0';

    /* הסר סיומת אם קיימת (הסרת נקודה אחרונה + כל מה שאחריה) */
    {
        char *dot = strrchr(output_basename, '.');
        if (dot)
            *dot = '\0';
    }

    return 0;
}

void make_output_path(const char *output_basename, char *output_path, size_t max_len)
{
    /* יוצרים נתיב כמו "output/<basename>.am" */
    snprintf(output_path, max_len, "output/%s.am", output_basename);
}

int ensure_directory_exists(const char *directory_path)
{
    struct stat st = {0};
    if (stat(directory_path, &st) == -1)
    {
        /* לא קיים - מנסים ליצור */
        if (mkdir(directory_path, 0700) != 0)
            return -1; /* failed */
    }
    else
    {
        /* קיים, לוודא שזה תיקייה */
        if (!S_ISDIR(st.st_mode))
            return -2; /* not a directory */
    }
    return 0;
}
