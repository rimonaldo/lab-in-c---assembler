/*
 * table.h
 *
 * Defines the interface for a generic symbol table module.
 *
 * This module provides a generic key-value store, implemented as a linked list.
 * It's designed to be reusable for different tables needed in the assembler,
 * such as the symbol table and the macro table.
 *
 * The key is always a C-string (const char*).
 * The value is a void pointer (void*), allowing any data type to be stored.
 * The table handles memory allocation for keys internally. The user is responsible
 * for managing the memory of the data pointed to by the value.
 */

#ifndef TABLE_H
#define TABLE_H

#include <stdbool.h> /* For the bool type */

/*
 * An opaque pointer to the table structure.
 * The implementation details are hidden in table.c
 */
typedef struct Table Table;

/**
 * @brief Creates and initializes a new, empty table.
 *
 * @return A pointer to the newly created table, or NULL if memory allocation fails.
 */
Table* table_create();

/**
 * @brief Destroys a table and frees all associated memory.
 *
 * Iterates through all entries in the table, freeing the key and the node itself.
 * If a `free_data_func` is provided, it is called on each data pointer to free
 * the memory associated with the value.
 *
 * @param table The table to destroy.
 * @param free_data_func A function pointer to a function that can free the
 * memory of the data stored in the table. If NULL, the
 * data pointers are not freed.
 */
void table_destroy(Table* table, void (*free_data_func)(void*));

/**
 * @brief Inserts a new key-value pair into the table.
 *
 * The function checks for duplicate keys. If a key already exists, the insertion fails.
 * The table creates an internal copy of the key, so the caller does not need
 * to maintain the key string after the call.
 *
 * @param table The table to insert into.
 * @param key The key for the new entry. Must not be NULL.
 * @param data The data (value) to associate with the key.
 * @return `true` if the insertion was successful, `false` otherwise (e.g., key
 * already exists, or memory allocation failed).
 */
bool table_insert(Table* table, const char* key, void* data);

/**
 * @brief Looks up a key in the table.
 *
 * @param table The table to search in.
 * @param key The key to look for.
 * @return A pointer to the data associated with the key if found, otherwise NULL.
 */
void* table_lookup(Table* table, const char* key);

/**
 * @brief Iterates over each entry in the table and applies a callback function.
 *
 * This allows processing all elements without exposing the internal structure
 * of the table.
 *
 * @param table The table to iterate over.
 * @param callback The function to call for each entry. It receives the key,
 * the data, and a user-provided context pointer.
 * @param user_context A pointer to user data that will be passed to the callback
 * on each invocation.
 */
void table_for_each(Table* table, void (*callback)(const char* key, void* data, void* user_context), void* user_context);


void table_print(Table *table, void (*print_func)(const char *key, void *data));

#endif /* TABLE_H */
