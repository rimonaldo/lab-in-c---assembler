#ifndef ERRORS_H
#define ERRORS_H

typedef enum
{
    SEV_ERROR,
    SEV_WARNING
} Severity;

typedef enum
{
    /* ────────────────[ 400–499: Macro Errors & Warnings ]──────────────── */
    E400_MACRO_UNDEFINED = 400, /* Macro invoked before defined */
    E401_MACRO_NESTED,          /* Nested mcro ... mcroend block */
    W402_MACRO_UNNAMED,         /* Macro name not defined. e.g: mcro mcroend  */
    W403_MACRO_REDEFINED,       /* Macro defined more than once, last macro will overide */
    W404_MACRO_EMPTY,           /* Empty macro body */

    /* ────────────────[ 500–599: Label Errors & Warnings ]─────────────── */
    E500_LABEL_INVALID = 500,      /* Invalid label format */
    E501_LABEL_RESERVED,           /* Label is reserved keyword */
    E502_LABEL_REDEFINED,          /* Duplicate label in same file */
    E503_LABEL_UNDEFINED,          /* Label used but never defined */
    E504_LABEL_FORWARD_UNDEFINED,  /* Label referenced before definition and never resolved */
    E505_LABEL_ENTRY_NOT_FOUND,    /* .entry label not defined in file */
    E506_LABEL_ENTRY_AND_EXTERN,   /* Label declared as both .entry and .extern */
    W507_LABEL_ON_ENTRY_OR_EXTERN, /* Label before .entry/.extern directive */
    W508_LABEL_UNUSED,             /* Label defined but never used */

    /* ────────────────[ 600–699: Instruction & Operand Errors ]────────── */
    E600_INSTRUCTION_NAME_INVALID = 600,      /* Unrecognized instruction name */
    E601_INSTRUCTION_FORMAT_INVALID,          /* Too many/few operands */
    E602_INSTRUCTION_TRAILING_CHARS,          /* Extra characters after instruction */
    E603_INSTRUCTION_ADDRESSING_MODE_INVALID, /* Invalid addressing mode */
    /* Immediate operand errors */
    E610_OPERAND_IMMEDIATE_INVALID = 610, /* Bad immediate syntax */
    E611_OPERAND_IMMEDIATE_OUT_OF_BOUNDS, /* Immediate value out of range */
    E612_OPERAND_IMMEDIATE_FLOAT,         /* Immediate value must be integer */
    /* Register operand errors */
    E613_OPERAND_REGISTER_INVALID,       /* Bad register name */
    E614_OPERAND_REGISTER_OUT_OF_BOUNDS, /* Register must be r0–r7 */
    /* Matrix operand errors */
    E615_OPERAND_MAT_INDEX_INVALID,       /* Matrix index not [rX][rY] */
    E616_OPERAND_MAT_INDEX_OUT_OF_BOUNDS, /* Matrix index out of bounds */
    W617_OPERAND_MAT_INITIALIZED_UNDER,   /* Not enough matrix initializers */
    W618_OPERAND_MAT_INITIALIZED_OVER,    /* Too many matrix initializers */

    /* ────────────────[ 700–799: Memory Errors & Warnings ]────────────── */
    E700_MEMORY_PROGRAM_WORD_LIMIT = 700, /* Program exceeds machine memory (e.g., 256 words) */
    E701_MEMORY_LINE_CHAR_LIMIT,          /* Source line exceeds 80 characters */
    E702_MEMORY_STACK_OVERFLOW_RISK,      /* Stack overflow risk detected */
    W703_MEMORY_UNUSED_DATA,               /* Unused .data/.mat values (extra initializers) */

    SUCCESS_100 = 100
} ErrorCode;

typedef struct
{
    ErrorCode code;
    const char *message;
    int line_number;
    Severity sevirity;
} ErrorInfo;

typedef struct
{
    ErrorInfo *error_log; /* מערך דינאמי */
    int error_count;      /* כמה שגיאות קיימות כרגע */
    int warning_count;
    int capacity;         /* כמה מוקצה כרגע בזיכרון */
} StatusInfo;

/* Returns pointer to ErrorInfo for given error code */
ErrorInfo *get_error_log(ErrorCode code);

ErrorInfo write_error_log(StatusInfo *status_info, ErrorCode code, int line_number);

void free_status_info(StatusInfo *status_info);

void print_errors(StatusInfo *status_info);

#endif
