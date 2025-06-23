#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H
#define MAX_MACROS 100
#define MAX_MACRO_LINES 50
#define MAX_MACROS 100
#define MAX_LINE_LEN 81 /*TODO: should be in centralized definitions file*/

int run_pre_assembler(const char *filename);

int is_macro_start(const char *line); /*Detects 'mcro'*/
int is_macro_end(const char *line);   /*Detects 'mcroend'*/
int is_macro_call(const char *line);  /*Checks if line matches macro name*/
int is_comment(const char *line);

/*void expand_macro(const char *line, FILE *output); Expands macro into output*/

#endif
