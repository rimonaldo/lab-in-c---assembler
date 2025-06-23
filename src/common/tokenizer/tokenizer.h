#ifndef TOKENIZER_H
#define TOKENIZER_H

#define MAX_TOKENS 64
#define MAX_TOKEN_LEN 64

typedef struct {
    char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
    int count;
} Tokens;

/**
 * Tokenizes a line from the assembly source.
 * Tokens are separated by spaces, tabs, or commas.
 * Preserves special tokens like brackets, colons, and hash.
 * Does not modify the original input string.
 *
 * @param line The input line to tokenize.
 * @return A Tokens struct containing all extracted tokens.
 */
Tokens tokenize_line(const char *line);

#endif
