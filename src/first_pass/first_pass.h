#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "../AST/ast.h"

/*
 * This is the header for your first pass module.
 * It's responsible for reading the given .am file, tokenizing its contents,
 * parsing the tokens, and building the complete Abstract Syntax Tree.
 */

/**
 * @brief Parses a .am file and builds the corresponding AST.
 * @param filename The name of the .am file to parse.
 * @return A pointer to the fully constructed ProgramAST. Returns NULL if a
 * syntax error or file error occurs.
 */
ProgramAST* parse_file_to_ast(const char* filename);

#endif /* FIRST_PASS_H */
