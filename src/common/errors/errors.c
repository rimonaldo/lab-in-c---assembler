#include "errors.h"
#include "../printer/printer.h"
#include <stdio.h>
#define UNINIT_LINE_NUM -1

/* Error info table */
static const ErrorInfo error_table[] = {

    /* ──────── Macro Errors & Warnings ──────── */
    {E400_MACRO_UNDEFINED, "Macro used before it was defined", UNINIT_LINE_NUM},
    {E401_MACRO_NESTED, "Nested macro definitions are not allowed", UNINIT_LINE_NUM},
    {W402_MACRO_UNNAMED, "Macro unnamed definition ", UNINIT_LINE_NUM, SEV_WARNING},
    {W403_MACRO_REDEFINED, "Macro redefined with the same name", UNINIT_LINE_NUM, SEV_WARNING},
    {W404_MACRO_EMPTY, "Empty macro definition (mcro ... mcroend with no content)", UNINIT_LINE_NUM, SEV_WARNING},

    /* ──────── Label Errors & Warnings ──────── */
    {E500_LABEL_INVALID, "Invalid label format (must start with a letter, be alphanumeric, max 31 chars)", UNINIT_LINE_NUM},
    {E501_LABEL_RESERVED, "Label name is reserved (e.g., instruction or directive name)", UNINIT_LINE_NUM},
    {E502_LABEL_REDEFINED, "Label redefined in the same file", UNINIT_LINE_NUM},
    {E503_LABEL_UNDEFINED, "Undefined label used in operand", UNINIT_LINE_NUM},
    {E504_LABEL_ENTRY_AND_EXTERN, "Label declared as both .entry and .extern", UNINIT_LINE_NUM},
    {W505_LABEL_ENTRY_NOT_FOUND, ".entry label is not defined within the file", UNINIT_LINE_NUM,SEV_WARNING},
    {W507_LABEL_ON_ENTRY_OR_EXTERN, "Label defined on a line with .entry/.extern (ignored)", UNINIT_LINE_NUM, SEV_WARNING},
    {W508_LABEL_UNUSED, "Label defined but never used", UNINIT_LINE_NUM, SEV_WARNING},

    /* ──────── Instruction & Operand Errors ──────── */
    {E600_INSTRUCTION_NAME_INVALID, "Invalid instruction mnemonic", UNINIT_LINE_NUM},
    {E601_INSTRUCTION_FORMAT_INVALID, "Wrong number of operands (too many or too few)", UNINIT_LINE_NUM},
    {E602_INSTRUCTION_TRAILING_CHARS, "Unexpected characters after valid instruction", UNINIT_LINE_NUM},
    {E603_INSTRUCTION_ADDRESSING_MODE_INVALID, "Illegal addressing mode for instruction", UNINIT_LINE_NUM},

    {E610_OPERAND_IMMEDIATE_INVALID, "Invalid immediate value syntax ", UNINIT_LINE_NUM},
    {E611_OPERAND_IMMEDIATE_OUT_OF_BOUNDS, "Immediate value out of range (-512 to +511)", UNINIT_LINE_NUM},
    {E612_OPERAND_IMMEDIATE_FLOAT, "Immediate value cannot be a float", UNINIT_LINE_NUM},

    {E613_OPERAND_REGISTER_INVALID, "Invalid register format", UNINIT_LINE_NUM},
    {E614_OPERAND_REGISTER_OUT_OF_BOUNDS, "Register must be r0 to r7", UNINIT_LINE_NUM},

    {E615_OPERAND_MAT_INDEX_INVALID, "Invalid matrix index format (should be [rX][rY])", UNINIT_LINE_NUM},
    {E616_OPERAND_MAT_INDEX_OUT_OF_BOUNDS, "Matrix indices must use valid registers (r0–r7)", UNINIT_LINE_NUM},
    {W617_OPERAND_MAT_INITIALIZED_UNDER, "Not enough matrix initializers for defined size", UNINIT_LINE_NUM, SEV_WARNING},
    {W618_OPERAND_MAT_INITIALIZED_OVER, "Too many matrix initializers for defined size", UNINIT_LINE_NUM, SEV_WARNING},

    /* ──────── Memory Errors & Warnings ──────── */
    {E700_MEMORY_PROGRAM_WORD_LIMIT, "Program exceeds machine memory limit (e.g., 256 words)", UNINIT_LINE_NUM},
    {E701_MEMORY_LINE_CHAR_LIMIT, "Line exceeds 80-character limit", UNINIT_LINE_NUM},
    {E702_MEMORY_STACK_OVERFLOW_RISK, "Stack overflow risk detected (e.g., many JSRs without RTS)", UNINIT_LINE_NUM},
    {W703_MEMORY_UNUSED_DATA, "Unused .data or .mat values (more initializers than needed)", UNINIT_LINE_NUM, SEV_WARNING},
};

#define ERROR_TABLE_SIZE (sizeof(error_table) / sizeof(ErrorInfo))

ErrorInfo *get_error_log(ErrorCode code)
{
    int i;
    for (i = 0; i < (int)ERROR_TABLE_SIZE; i++)
    {
        if (error_table[i].code == code)
        {
            return &error_table[i];
        }
    }
    return (const ErrorInfo *)0; /* NULL */
}

void print_errors(StatusInfo *status_info)
{
    int i;
    if (!status_info || !status_info->error_log)
        return;

    for (i = 0; i < status_info->error_count + status_info->warning_count; i++)
    {
        ErrorInfo *err = &status_info->error_log[i];
        const char *sev_str = (err->sevirity == SEV_ERROR) ? "Error" : "Warning";

        if (err->line_number >= 0)
            printf("[%s] Line %d: %s (Code: %d)\n", sev_str, err->line_number, err->message, err->code);
        else
            printf("[%s] %s (Code: %d)\n", sev_str, err->message, err->code);
    }
}

ErrorInfo write_error_log(StatusInfo *status_info, ErrorCode code, int line_number)
{
    /* dynamicaly increase error log memory space when needed */
    if (status_info->error_count >= status_info->capacity)
    {
        status_info->capacity = (status_info->capacity == 0) ? 4 : status_info->capacity * 2;
        int new_size = sizeof(ErrorInfo) * status_info->capacity;
        ErrorInfo *new_log = realloc(status_info->error_log, new_size);

        if (!new_log)
        {
            printf("Memory allocation failed while expanding error log\n");
            exit(1);
        }
        status_info->error_log = new_log;
    }

    const ErrorInfo new_err = {
        .code = code,
        .line_number = line_number,
        .message = get_error_log(code)->message,
        .sevirity = get_error_log(code)->sevirity};

    status_info->error_log[status_info->error_count + status_info->warning_count] = new_err;
    if (new_err.sevirity == SEV_WARNING)
    {
        PRINT_WRN(new_err);
        status_info->warning_count++;
    }
    else
    {
        PRINT_ERR(new_err);
        status_info->error_count++;
    }
    return new_err;
}

void free_status_info(StatusInfo *status_info)
{
    free(status_info->error_log);
    free(status_info);
}
