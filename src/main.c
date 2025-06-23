#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pre_assembler/pre_assembler.h"  /* For macro expansion stage */
#include "first_pass/first_pass.h"        /* For parsing .am to AST */
#include "AST/ast.h"
#include "AST/print_ast.h"                /* Utility to print AST */

/* Forward declaration for helper function */
char* get_am_filename(const char* input_filename);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file.as>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* input_filename = argv[1];
    char* am_filename = NULL;
    ProgramAST* ast = NULL;
    int status = EXIT_SUCCESS;

    /* --- Stage 1: Pre-Assembler --- */
    /* Expand macros, generate .am file */
    printf("--- Stage 1: Running Pre-Assembler for %s ---\n", input_filename);
    if (run_pre_assembler(input_filename) != 0) {
        fprintf(stderr, "Error: Pre-assembler stage failed.\n");
        status = EXIT_FAILURE;
        goto cleanup;
    }
    printf("Pre-assembler completed successfully.\n\n");

    /* --- Generate .am filename --- */
    am_filename = get_am_filename(input_filename);
    if (!am_filename) {
        fprintf(stderr, "Error: Could not determine .am filename.\n");
        status = EXIT_FAILURE;
        goto cleanup;
    }

    /* --- Stage 2: First Pass (Parse .am into AST) --- */
    printf("--- Stage 2: Parsing %s into AST ---\n", am_filename);
    ast = parse_file_to_ast(am_filename);  /* You must implement this function */
    if (!ast) {
        fprintf(stderr, "Error: First pass (parsing) failed. AST not created.\n");
        status = EXIT_FAILURE;
        goto cleanup;
    }
    printf("Parsing completed successfully. AST created.\n\n");

    /* --- Stage 3: Display AST --- */
    printf("--- Stage 3: Displaying Abstract Syntax Tree ---\n");
    print_ast(ast);
    printf("-----------------------------------------------\n\n");

cleanup:
    /* --- Stage 4: Cleanup --- */
    printf("--- Stage 4: Cleaning up resources ---\n");
    if (ast) {
        free_program_ast(ast);
        printf("AST memory freed.\n");
    }
    if (am_filename) {
        free(am_filename);
        printf("Filename buffer freed.\n");
    }
    printf("Pipeline finished with status: %s\n", (status == EXIT_SUCCESS) ? "SUCCESS" : "FAILURE");

    return status;
}

/**
 * @brief Generates a .am filename from a source filename.
 * For example, "input.as" becomes "input.am".
 * 
 * @param input_filename The original filename.
 * @return A dynamically allocated string with the new filename, or NULL on failure.
 * The caller is responsible for freeing this memory.
 */
char* get_am_filename(const char* input_filename) {
    const char *dot = strrchr(input_filename, '.');
    char *am_filename;
    size_t base_len;

    /* Find the length of the base name (without extension) */
    if (dot != NULL) {
        base_len = (size_t)(dot - input_filename);
    } else {
        base_len = strlen(input_filename);
    }

    /* Allocate memory for base + ".am" + null terminator */
    am_filename = malloc(base_len + 4); /* 3 chars + 1 for '\0' */
    if (!am_filename) {
        perror("Failed to allocate memory for filename");
        return NULL;
    }

    /* Copy base and append .am */
    strncpy(am_filename, input_filename, base_len);
    am_filename[base_len] = '\0'; /* strncpy doesn't add null terminator if not full length */
    strcat(am_filename, ".am");

    return am_filename;
}
