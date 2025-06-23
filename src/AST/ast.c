#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/*
 * Note on String Handling:
 * We use strdup() to copy strings (labels, names). This is crucial because it
 * allocates new memory for the strings on the heap. This prevents the AST from
 * pointing to temporary buffers (like from a parser) that might be freed or
 * changed later. The corresponding free() calls are handled in the free_...
 * functions.
 */

/**
 * @brief Creates an operand of type immediate.
 * @param value The immediate integer value.
 * @return A pointer to the newly created Operand, or NULL on failure.
 */
Operand* create_immediate_operand(int value) {
    Operand *op = (Operand*) malloc(sizeof(Operand));
    if (!op) {
        perror("Failed to allocate memory for immediate operand");
        return NULL;
    }
    op->type = OPERAND_IMMEDIATE;
    op->value.immediate = value;
    return op;
}

/**
 * @brief Creates an operand of type register.
 * @param reg_num The register number.
 * @return A pointer to the newly created Operand, or NULL on failure.
 */
Operand* create_register_operand(int reg_num) {
    Operand *op = (Operand*) malloc(sizeof(Operand));
    if (!op) {
        perror("Failed to allocate memory for register operand");
        return NULL;
    }
    op->type = OPERAND_REGISTER;
    op->value.reg_num = reg_num;
    return op;
}

/**
 * @brief Creates an operand of type label.
 * @param label The string label. The string is duplicated.
 * @return A pointer to the newly created Operand, or NULL on failure.
 */
Operand* create_label_operand(char *label) {
    Operand *op = (Operand*) malloc(sizeof(Operand));
    if (!op) {
        perror("Failed to allocate memory for label operand");
        return NULL;
    }
    op->type = OPERAND_LABEL;
    op->value.label = strdup(label);
    if (!op->value.label) {
        perror("Failed to duplicate label for operand");
        free(op);
        return NULL;
    }
    return op;
}

/**
 * @brief Creates an operand for matrix addressing.
 * @param label The label of the matrix. The string is duplicated.
 * @param row_reg The register holding the row index.
 * @param col_reg The register holding the column index.
 * @return A pointer to the newly created Operand, or NULL on failure.
 */
Operand* create_matrix_operand(char *label, int row_reg, int col_reg) {
    Operand *op = (Operand*) malloc(sizeof(Operand));
    if (!op) {
        perror("Failed to allocate memory for matrix operand");
        return NULL;
    }
    op->type = OPERAND_MATRIX;
    op->value.matrix.label = strdup(label);
    if (!op->value.matrix.label) {
        perror("Failed to duplicate label for matrix operand");
        free(op);
        return NULL;
    }
    op->value.matrix.row_reg = row_reg;
    op->value.matrix.col_reg = col_reg;
    return op;
}


/**
 * @brief Creates a statement of type instruction.
 * @param label The statement label (can be NULL). The string is duplicated.
 * @param name The instruction name (e.g., "mov", "add"). The string is duplicated.
 * @param src The source operand.
 * @param dest The destination operand.
 * @return A pointer to the newly created Statement, or NULL on failure.
 */
Statement* create_instruction_statement(char* label, char* name, Operand* src, Operand* dest) {
    Statement *stmt = (Statement*) malloc(sizeof(Statement));
    if (!stmt) {
        perror("Failed to allocate memory for instruction statement");
        return NULL;
    }

    stmt->label = label ? strdup(label) : NULL;
    stmt->type = STATEMENT_INSTRUCTION;
    stmt->line.instruction.name = strdup(name);
    stmt->line.instruction.src_operand = src;
    stmt->line.instruction.dest_operand = dest;
    stmt->next = NULL;

    return stmt;
}

/**
 * @brief Creates a statement of type directive.
 * @param label The statement label (can be NULL). The string is duplicated.
 * @param name The directive name (e.g., ".data", ".string"). The string is duplicated.
 * @param data A pointer to the directive's data.
 * @return A pointer to the newly created Statement, or NULL on failure.
 */
Statement* create_directive_statement(char* label, char* name, void* data) {
    Statement *stmt = (Statement*) malloc(sizeof(Statement));
    if (!stmt) {
        perror("Failed to allocate memory for directive statement");
        return NULL;
    }

    stmt->label = label ? strdup(label) : NULL;
    stmt->type = STATEMENT_DIRECTIVE;
    stmt->line.directive.name = strdup(name);
    stmt->line.directive.data = data;
    stmt->next = NULL;

    return stmt;
}


/**
 * @brief Creates and initializes an empty ProgramAST.
 * @return A pointer to the newly created ProgramAST, or NULL on failure.
 */
ProgramAST* create_program_ast() {
    ProgramAST *ast = (ProgramAST*) malloc(sizeof(ProgramAST));
    if (!ast) {
        perror("Failed to allocate memory for ProgramAST");
        return NULL;
    }
    ast->head = NULL;
    ast->tail = NULL;
    return ast;
}

/**
 * @brief Appends a statement to the end of the AST's linked list.
 * @param ast The ProgramAST to modify.
 * @param statement The statement to add.
 */
void add_statement_to_ast(ProgramAST *ast, Statement *statement) {
    if (!ast || !statement) {
        return;
    }

    if (ast->head == NULL) {
        /* This is the first statement in the list */
        ast->head = statement;
        ast->tail = statement;
    } else {
        /* Append to the end of the list */
        ast->tail->next = statement;
        ast->tail = statement;
    }
}


/**
 * @brief Frees all memory associated with an Operand.
 * @param operand The operand to free.
 */
void free_operand(Operand *operand) {
    if (!operand) {
        return;
    }
    /* Free any dynamically allocated strings within the operand */
    if (operand->type == OPERAND_LABEL) {
        free(operand->value.label);
    } else if (operand->type == OPERAND_MATRIX) {
        free(operand->value.matrix.label);
    }
    /* Free the operand struct itself */
    free(operand);
}

/**
 * @brief Frees all memory associated with a Statement.
 * @param statement The statement to free.
 */
void free_statement(Statement *statement) {
    if (!statement) {
        return;
    }

    /* Free the optional statement label */
    if (statement->label) {
        free(statement->label);
    }

    /* Free the contents of the line union based on type */
    if (statement->type == STATEMENT_INSTRUCTION) {
        free(statement->line.instruction.name);
        free_operand(statement->line.instruction.src_operand);
        free_operand(statement->line.instruction.dest_operand);
    } else if (statement->type == STATEMENT_DIRECTIVE) {
        free(statement->line.directive.name);
        /* IMPORTANT: The caller is responsible for freeing the `data` pointer
         * since `ast.c` does not know its type or structure.
         */
        if (statement->line.directive.data) {
            free(statement->line.directive.data);
        }
    }

    /* Free the statement struct itself */
    free(statement);
}

/**
 * @brief Frees an entire ProgramAST, including all statements and operands.
 * @param ast The ProgramAST to free.
 */
void free_program_ast(ProgramAST *ast) {
    if (!ast) {
        return;
    }

    Statement *current = ast->head;
    Statement *next;

    while (current != NULL) {
        next = current->next;
        free_statement(current);
        current = next;
    }

    /* Free the AST container itself */
    free(ast);
}
