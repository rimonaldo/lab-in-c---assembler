/******************************************************************************
 * File: ast.c
 * Author: Rimon
 * Date: 23 June, 2025
 *
 * Description:
 * This file implements the functionality of the Abstract Syntax Tree (AST) module.
 * It includes functions for creating, managing, and freeing AST nodes that represent
 * the assembly code after parsing.
 *
 ******************************************************************************/

#include <stdio.h>  /* For error printing (fprintf) */
#include <stdlib.h> /* For dynamic memory allocation (malloc, free) */
#include "ast.h"

/**
 * @brief Creates a new AST node for a machine instruction.
 *
 * @param line_num The original line number in the source file.
 * @param label The label associated with the line (or NULL if none).
 * @param instruction Structure containing all information about the instruction.
 * @return ASTNode* Pointer to the newly created node, or NULL if memory allocation fails.
 */
ASTNode *create_instruction_node(int line_num, const char *label, InstructionInfo instruction)
{
    /* 1. Dynamically allocate memory for the new node */
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (node == NULL)
    {
        /* Handle memory allocation error */
        fprintf(stderr, "Error: Memory allocation failed for new AST node.\n");
        return NULL;
    }

    /* 2. Initialize common fields */
    node->line_number = line_num;
    node->next = NULL;

    /* Safely duplicate the label string. my_strdup allocates new memory and copies the string. */
    /* This is important to avoid dependency on the original string and prevent memory issues. */
    if (label != NULL)
    {
        node->label = (char *)malloc(strlen(label) + 1);
        if (node->label == NULL)
        {
            /* Handle memory allocation error for the label */
            free(node);
            return NULL;
        }
    }
    else
    {
        node->label = NULL;
    }

    /* 3. Init instruction-specific fields */
    node->type = INSTRUCTION_STATEMENT;
    node->content.instruction = instruction; /* Copy the entire instruction struct */

    /* 4. Return the new node */
    return node;
}

/**
 * @brief Creates a new AST node for an assembler directive.
 *
 * @param line_num The original line number in the source file.
 * @param label The label associated with the line (or NULL if none).
 * @param directive Structure containing all information about the directive.
 * @return ASTNode* Pointer to the newly created node, or NULL if memory allocation fails.
 */
ASTNode *create_directive_node(int line_num, const char *label, DirectiveInfo directive)
{
    /* 1. Dynamically allocate memory for the new node */
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (node == NULL)
    {
        /* Handle memory allocation error */
        fprintf(stderr, "Error: Memory allocation failed for new AST node.\n");
        return NULL;
    }

    /* 2. Initialize common fields (same logic as previous function) */
    node->line_number = line_num;
    node->next = NULL;
    if (label != NULL)
    {
        node->label = (char *)malloc(strlen(label) + 1);
        if (node->label == NULL)
        {
            free(node);
            return NULL;
        }
    }
    else
    {
        node->label = NULL;
    }

    /* 3. Initialize directive-specific fields */
    node->type = DIRECTIVE_STATEMENT;
    node->content.directive = directive; /* Copy the entire directive struct */

    /* 4. Return the new node */
    return node;
}

/**
 * @brief Frees all memory allocated for the AST (the linked list).
 *
 * @param head Pointer to the head of the linked list (the first node).
 */
void free_ast(ASTNode *head)
{
    ASTNode *current = head;
    ASTNode *next_node;

    while (current != NULL)
    {
        /* save the next pointer before freeing the current node */
        next_node = current->next;

        /* 1. Free dynamically allocated memory inside the node */
        if (current->label != NULL)
        {
            free(current->label);
        }

        if (current->type == INSTRUCTION_STATEMENT)
        {
            /*
            free_instruction_contents(&(current->content.instruction));
            */
        }
        else if (current->type == DIRECTIVE_STATEMENT)
        {
            /*
            free_directive_contents(&(current->content.directive));
            */
        }

        /* 2. Free the node itself */
        free(current);
        /* 3. Move to the next node */
        current = next_node;
    }
}

void free_operand(Operand *op)
{
    if (!op)
        return;

    if (op->mode == DIRECT && op->value.label != NULL)
    {
        free(op->value.label);
    }
    else if (op->mode == MAT_ACCESS && op->value.index.label != NULL)
    {
        free(op->value.index.label);
    }
}

void free_instruction_contents(InstructionInfo *inst)
{
    free_operand(&inst->src_op);
    free_operand(&inst->dest_op);
}

void free_directive_contents(DirectiveInfo *dir)
{
    switch (dir->type)
    {
    case DATA:
        free(dir->params.data.values);
        break;

    case STRING:
        free(dir->params.str);
        break;

    case ENTRY:
    case EXTERN:
    default:
        break;
    }
}
