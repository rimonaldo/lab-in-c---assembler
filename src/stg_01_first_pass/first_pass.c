#include <stdio.h>
#include "first_pass.h"

void run_first_pass(char *filename)
{
    printf("FILENAME IS: \n %s\n\n", filename);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
}