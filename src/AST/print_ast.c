#include <stdio.h>
#include "ast.h"
#include "print_ast.h"

/**/
void print_operand(const Operand* op);

/**
 * @brief Traverses the AST and prints each statement in a readable format.
 */
void print_ast(const ProgramAST* ast) {
    if (!ast || !ast->head) {
        printf("AST is empty or null.\n");
        return;
    }

    const Statement* current = ast->head;
    int line_num = 1;

    /**/
    while (current) {
        printf("Node %-3d: ", line_num++);

        /**/
        if (current->label) {
            printf("%-10s: ", current->label);
        } else {
            printf("%-10s  ", "");
        }

        /**/
        if (current->type == STATEMENT_INSTRUCTION) {
            const struct { char* name; Operand* src_operand; Operand* dest_operand; }* i = &current->line.instruction;
            printf("INST  %-7s ", i->name);
            if (i->src_operand) {
                print_operand(i->src_operand);
            }
            /**/
            if (i->src_operand && i->dest_operand) {
                 printf(", ");
            }
            if (i->dest_operand) {
                print_operand(i->dest_operand);
            }
        } else if (current->type == STATEMENT_DIRECTIVE) {
            const struct { char* name; void* data; }* d = &current->line.directive;
            printf("DIR   %-7s ", d->name);
            /**/
            if (d->data) {
                printf("(data at %p)", d->data);
            }
        }
        printf("\n");
        current = current->next;
    }
}

/**
 * @brief Helper function to print a single operand based on its type.
 */
void print_operand(const Operand* op) {
    if (!op) return;

    switch (op->type) {
        case OPERAND_IMMEDIATE:
            printf("#%d", op->value.immediate);
            break;
        case OPERAND_REGISTER:
            printf("r%d", op->value.reg_num);
            break;
        case OPERAND_LABEL:
            printf("%s", op->value.label);
            break;
        case OPERAND_MATRIX:
            printf("%s[r%d][r%d]", op->value.matrix.label, op->value.matrix.row_reg, op->value.matrix.col_reg);
            break;
        default:
            printf("[Unknown Operand Type]");
            break;
    }
}
