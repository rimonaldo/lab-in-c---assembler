#include "first_pass.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* read_file_to_string(const char* filename);
static ProgramAST* parse_string_to_ast(const char* source);

/**
 * @brief Parses a .am file and builds the corresponding AST.
 * @param filename The name of the .am file to parse.
 * @return A pointer to the fully constructed ProgramAST. Returns NULL if a
 * syntax error or file error occurs.
 */
ProgramAST* parse_file_to_ast(const char* filename) {
    if (!filename) {
        fprintf(stderr, "parse_file_to_ast: filename is NULL\n");
        return NULL;
    }

    char* source_code = read_file_to_string(filename);
    if (!source_code) {
        fprintf(stderr, "parse_file_to_ast: failed to read file %s\n", filename);
        return NULL;
    }

    ProgramAST* ast = parse_string_to_ast(source_code);

    free(source_code);
    return ast;
}

/**
 * @brief Reads the entire file contents into a null-terminated string.
 * @param filename The file to read.
 * @return Pointer to the allocated string with file contents, or NULL on failure.
 * The caller must free the returned string.
 */
static char* read_file_to_string(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        perror("fseek failed");
        fclose(file);
        return NULL;
    }
    long file_size = ftell(file);
    if (file_size < 0) {
        perror("ftell failed");
        fclose(file);
        return NULL;
    }
    rewind(file);

    char* buffer = malloc(file_size + 1);
    if (!buffer) {
        perror("malloc failed");
        fclose(file);
        return NULL;
    }

    size_t read_bytes = fread(buffer, 1, file_size, file);
    if (read_bytes != (size_t)file_size) {
        fprintf(stderr, "Error reading file %s\n", filename);
        free(buffer);
        fclose(file);
        return NULL;
    }
    buffer[file_size] = '\0';  

    fclose(file);
    return buffer;
}

/**
 * @brief Parses the assembly source code string and builds the AST.
 * This is a stub implementation — you need to implement tokenization,
 * syntax analysis, operand parsing, and building the linked list of Statements.
 *
 * @param source The null-terminated assembly source code string.
 * @return Pointer to the constructed ProgramAST or NULL if parsing fails.
 */
static ProgramAST* parse_string_to_ast(const char* source) {
    /* TODO: Implement tokenizer, parser, error handling and AST builder here.*/

    /* For now, just print a message and return NULL.*/
    fprintf(stderr, "parse_string_to_ast: Parsing not yet implemented.\n");
    (void)source; 

    return NULL;
}
