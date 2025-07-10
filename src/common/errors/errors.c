#include "errors.h"
#define UNINIT_LINE_NUM -1
/* Error info table */
static const ErrorLog error_table[] = {
    /* Syntax Errors */
    {E001, "Invalid label format (not starting with letter, too long, or contains invalid chars)",UNINIT_LINE_NUM},
    {E002, "Label redefined (duplicate label in same file)",UNINIT_LINE_NUM},
    {E003, "Label is a reserved word (instruction/macro/directive)",UNINIT_LINE_NUM},
    {E004, "Invalid instruction mnemonic",UNINIT_LINE_NUM},
    {E005, "Invalid directive (e.g., .dtaa)",UNINIT_LINE_NUM},
    {E006, "Invalid operand format (e.g., bad immediate, bad register, wrong string syntax)",UNINIT_LINE_NUM},
    {E007, "Too many operands (e.g., 3 instead of 2)",UNINIT_LINE_NUM},
    {E008, "Too few operands",UNINIT_LINE_NUM},
    {E009, "Unexpected characters after valid statement",UNINIT_LINE_NUM},
    {E010, "Missing or malformed matrix brackets [rX][rY]",UNINIT_LINE_NUM},

    /* Semantic Errors */
    {E101, "Undefined label used (no matching definition)",UNINIT_LINE_NUM},
    {E102, "Label used before defined, and it's not resolved in second pass",UNINIT_LINE_NUM},
    {E103, ".entry label not defined in same file",UNINIT_LINE_NUM},
    {E104, "Label declared both .entry and .extern",UNINIT_LINE_NUM},
    {E105, "Illegal addressing mode for instruction (e.g., mov #5, #3)",UNINIT_LINE_NUM},
    {E106, "Register name out of range (e.g., r9)",UNINIT_LINE_NUM},
    {E107, "String not closed (\"unclosed)",UNINIT_LINE_NUM},
    {E108, "Too many matrix initializers for given size",UNINIT_LINE_NUM},
    {E109, "Invalid matrix size (zero or negative rows/cols)",UNINIT_LINE_NUM},
    {E110, "Macro used but not defined",UNINIT_LINE_NUM},
    {E111, "Nested mcro definitions (not allowed)",UNINIT_LINE_NUM},
    {E112, "Label on .entry or .extern line (label ignored or warned)",UNINIT_LINE_NUM},

    /* Warnings */
    {W001, "Label defined but never used",UNINIT_LINE_NUM},
    {W002, ".entry used on undefined label (could become error if not resolved)",UNINIT_LINE_NUM},
    {W003, "Label on .entry or .extern line is ignored",UNINIT_LINE_NUM},
    {W004, "Macro redefined (same name)",UNINIT_LINE_NUM},
    {W005, "Forward reference to macro (macro called before defined)",UNINIT_LINE_NUM},
    {W006, "Line exceeds 80 characters (may be trimmed/ignored)",UNINIT_LINE_NUM},
    {W007, "Empty macro definition (mcro ... mcroend with nothing)",UNINIT_LINE_NUM},
    {W008, "Multiple instructions on one line (only first parsed)",UNINIT_LINE_NUM},

    /* Memory Errors */
    {MEM_E201_WRD_LIM, "Program exceeds 256-word memory limit",UNINIT_LINE_NUM},
    {MEM_E202_LN_LIM, "Instruction too long (e.g., 6 words)",UNINIT_LINE_NUM},
    {MEM_E203_SO_RSK, "Stack overflow risk (e.g., repeated jsr with no rts)",UNINIT_LINE_NUM},
    {MEM_W201_UNSD, "Unused .data or .mat values (e.g., more values than declared size)",UNINIT_LINE_NUM},
};

#define ERROR_TABLE_SIZE (sizeof(error_table) / sizeof(ErrorLog))

const ErrorLog *get_error_log(ErrorCode code)
{
    int i;
    for (i = 0; i < (int)ERROR_TABLE_SIZE; i++)
    {
        if (error_table[i].code == code)
        {
            return &error_table[i];
        }
    }
    return (const ErrorLog *)0; /* NULL */
}
