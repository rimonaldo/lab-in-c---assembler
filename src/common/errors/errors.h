#ifndef ERRORS_H
#define ERRORS_H

typedef enum
{
    SEV_ERROR,
    SEV_WARNING,
} Severity;

typedef enum
{
    /* Syntax Errors (Critical - halt assembly) */
    E001 = 501,
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
    MEM_E201_WRD_LIM = 301,
    MEM_E202_LINE_CHAR_LIMIT,
    MEM_E203_SO_RSK,
    MEM_W201_UNSD
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
    int capacity;         /* כמה מוקצה כרגע בזיכרון */
} StatusInfo;

/* Returns pointer to ErrorInfo for given error code */
const ErrorInfo *get_error_log(ErrorCode code);

const ErrorInfo write_error_log(StatusInfo *status_info, ErrorCode code, int line_number);
void free_status_info(StatusInfo *status_info);

#endif

typedef enum
{
    /* Label Errors */
    E501_LABEL_INVALID = 501,
    E502_LABEL_RESERVED,
    E503_LABEL_REDEFINED,
    E504_LABEL_UNDEFINED,
    E505_LABEL_FORWARD_UNDEFINED,
    E506_LABEL__ENTRY_NOT_FOUND,
    E507_LABEL_ENTRY_AND_EXTERN,
    W508_LABEL_OR_ENTRY_OR_EXTERN,

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
    MEM_E201_WRD_LIM = 301,
    MEM_E202_LINE_CHAR_LIMIT,
    MEM_E203_SO_RSK,
    MEM_W201_UNSD
} ErrorCode2;
