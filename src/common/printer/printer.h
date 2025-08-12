#ifndef PRINTER_H
#define PRINTER_H

#include <stdio.h>

/* Only declare the real function when DEBUG is enabled */
#ifdef DEBUG
void printer_debug(const char *fmt, ...);
#  define PRINT_DEBUG printer_debug
#else
/* No-op stub — static so each TU gets its own copy, no linker conflicts */
static void printer_debug(const char *fmt, ...) { (void)fmt; }
#  define PRINT_DEBUG printer_debug
#endif

#define PRINT_LINE(n) printf("\n\033[1;36m╔════════════════════════════╗\n" \
                             "║        [ Line %3d ]        ║\n"             \
                             "╚════════════════════════════╝\033[0m\n",     \
                             (n))

#define PRINT_RAW_LINE(s)      printf("  \033[0;37mRaw Line     :\033[0m %s", (s))
#define PRINT_TOKEN(t)         printf("  \033[0;32mToken        :\033[0m %s\n", (t))
#define PRINT_LABEL_FOUND(l)   printf("  \033[1;33mLabel Found  :\033[0m %s\n", (l))
#define PRINT_LABEL_INSERT(l,a) printf("  \033[0;36mLabel Insert :\033[0m '%s' @ %d\n", (l), (a))
#define PRINT_LABEL_EXISTS(l)  printf("  \033[1;31mLabel Error  :\033[0m '%s' already exists\n", (l))
#define PRINT_INSTRUCTION(o)   printf("  \033[1;35mInstruction  :\033[0m opcode = %d\n", (o))
#define PRINT_DIRECTIVE(s)     printf("  \033[1;34mDirective    :\033[0m %s\n", (s))
#define PRINT_OPERAND(n,tok)   printf("  \033[0;32mOperand %-5d:\033[0m %s\n", (n), (tok))
#define PRINT_ADDR_MODE(s)     printf("  \033[0;36mAddr. Mode   :\033[0m %s\n", (s))
#define PRINT_DC(dc)           printf("  \033[1;36mData Counter :\033[0m %d\n", (dc))

#define PRINT_ERR(e)                                              \
    do {                                                          \
        fprintf(stderr, "  \033[1;31mERROR\033[0m Line %d: %s\n", \
                (e).line_number,                                  \
                (e).message ? (e).message : "Unknown error");     \
    } while (0)

#define PRINT_WRN(e)                                                \
    do {                                                            \
        fprintf(stderr, "  \033[1;33mWARNING\033[0m Line %d: %s\n", \
                (e).line_number,                                    \
                (e).message ? (e).message : "Unknown warning");     \
    } while (0)

void print_symbol(const char *key, void *data);
void print_extern(const char *key, void *data);
void print_entry(const char *key, void *data);

#endif /* PRINTER_H */
