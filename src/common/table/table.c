/*
 * table.c
 *
 * Implementation of the generic table module.
 * The table is implemented as a singly linked list.
 */

#include "table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * @brief The internal structure for a single node in the linked list.
 *
 * Each node holds a key, a pointer to the data (value), and a pointer
 * to the next node in the list.
 */
typedef struct TableNode {
    char* key;              /* The key for this entry (string) */
    void* data;             /* Pointer to the associated data */
    struct TableNode* next; /* Pointer to the next node in the table */
} TableNode;

/**
 * @brief The main table structure that holds the linked list.
 */
struct Table {
    TableNode* head; /* Pointer to the first node in the list */
};

/**
 * @brief Creates and initializes a new, empty table.
 */
Table* table_create() {
    /* Allocate memory for the table structure itself */
    Table* table = (Table*)malloc(sizeof(Table));
    if (table == NULL) {
        perror("Failed to allocate memory for table");
        return NULL;
    }

    /* Initialize the table to be empty */
    table->head = NULL;
    return table;
}

/**
 * @brief Destroys a table and frees all associated memory.
 */
void table_destroy(Table* table, void (*free_data_func)(void*)) {
    TableNode* current;
    TableNode* next;

    if (table == NULL) {
        return;
    }

    current = table->head;
    while (current != NULL) {
        next = current->next;

        /* Free the key string (which was duplicated during insertion) */
        free(current->key);

        /* If a free function is provided, use it to free the data */
        if (free_data_func != NULL) {
            free_data_func(current->data);
        }

        /* Free the node structure itself */
        free(current);
        current = next;
    }

    /* Finally, free the table container */
    free(table);
}

/**
 * @brief Inserts a new key-value pair into the table.
 */
bool table_insert(Table* table, const char* key, void* data) {
    TableNode* new_node;

    if (table == NULL || key == NULL) {
        return false;
    }

    /* First, check if the key already exists to prevent duplicates */
    if (table_lookup(table, key) != NULL) {
        fprintf(stderr, "Error: Key '%s' already exists in the table.\n", key);
        return false;
    }

    /* Allocate memory for the new node */
    new_node = (TableNode*)malloc(sizeof(TableNode));
    if (new_node == NULL) {
        perror("Failed to allocate memory for new table node");
        return false;
    }

    /* Duplicate the key string to be stored in the table */
    /* strdup is equivalent to malloc + strcpy */
    new_node->key = (char*)malloc(strlen(key) + 1);
    if (new_node->key == NULL) {
        perror("Failed to allocate memory for key");
        free(new_node); /* Clean up the allocated node */
        return false;
    }
    strcpy(new_node->key, key);

    /* Set the data pointer */
    new_node->data = data;

    /* Insert the new node at the beginning of the list */
    new_node->next = table->head;
    table->head = new_node;

    return true;
}

/**
 * @brief Looks up a key in the table.
 */
void* table_lookup(Table* table, const char* key) {
    TableNode* current;

    if (table == NULL || key == NULL) {
        return NULL;
    }

    current = table->head;
    while (current != NULL) {
        /* Compare the keys */
        if (strcmp(current->key, key) == 0) {
            return current->data; /* Key found, return the data */
        }
        current = current->next;
    }

    return NULL; /* Key not found */
}

/**
 * @brief Iterates over each entry in the table and applies a callback function.
 */
void table_for_each(Table* table, void (*callback)(const char* key, void* data, void* user_context), void* user_context) {
    TableNode* current;

    if (table == NULL || callback == NULL) {
        return;
    }

    current = table->head;
    while (current != NULL) {
        callback(current->key, current->data, user_context);
        current = current->next;
    }
}
