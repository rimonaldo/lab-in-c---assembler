#ifndef ERRORS_H
#define ERRORS_H

typedef enum
{
    SEV_ERROR,
    SEV_WARNING,
    SEV_CRITICAL
} Severity;

typedef enum
{
    /* Syntax Errors (Critical - halt assembly) */
    E001 = 1,
    E002,
    E003,
    E004,
    E005,
    E006,
    E007,
    E008,
    E009,
    E010,

    /* Semantic Errors (Critical - halt assembly) */
    E101 = 101,
    E102,
    E103,
    E104,
    E105,
    E106,
    E107,
    E108,
    E109,
    E110,
    E111,
    E112,

    /* Warnings (Non-critical - allow assembly) */
    W001 = 201,
    W002,
    W003,
    W004,
    W005,
    W006,
    W007,
    W008,

    /* Memory Errors (Assembly stops if violated) */
    MEM_E201 = 301,
    MEM_E202,
    MEM_E203,
    MEM_W201
} ErrorCode;

typedef struct
{
    ErrorCode code;
    const char *message;
    Severity severity;
} ErrorInfo;

/* Returns pointer to ErrorInfo for given error code */
const ErrorInfo *get_error_info(ErrorCode code);

#endif 
