#include "errors.h"

/* Error info table */
static const ErrorInfo error_table[] = {
    /* Syntax Errors */
    {E001, "Invalid label format (not starting with letter, too long, or contains invalid chars)", SEV_ERROR},
    {E002, "Label redefined (duplicate label in same file)", SEV_ERROR},
    {E003, "Label is a reserved word (instruction/macro/directive)", SEV_ERROR},
    {E004, "Invalid instruction mnemonic", SEV_ERROR},
    {E005, "Invalid directive (e.g., .dtaa)", SEV_ERROR},
    {E006, "Invalid operand format (e.g., bad immediate, bad register, wrong string syntax)", SEV_ERROR},
    {E007, "Too many operands (e.g., 3 instead of 2)", SEV_ERROR},
    {E008, "Too few operands", SEV_ERROR},
    {E009, "Unexpected characters after valid statement", SEV_ERROR},
    {E010, "Missing or malformed matrix brackets [rX][rY]", SEV_ERROR},

    /* Semantic Errors */
    {E101, "Undefined label used (no matching definition)", SEV_ERROR},
    {E102, "Label used before defined, and it's not resolved in second pass", SEV_ERROR},
    {E103, ".entry label not defined in same file", SEV_ERROR},
    {E104, "Label declared both .entry and .extern", SEV_ERROR},
    {E105, "Illegal addressing mode for instruction (e.g., mov #5, #3)", SEV_ERROR},
    {E106, "Register name out of range (e.g., r9)", SEV_ERROR},
    {E107, "String not closed (\"unclosed)", SEV_ERROR},
    {E108, "Too many matrix initializers for given size", SEV_ERROR},
    {E109, "Invalid matrix size (zero or negative rows/cols)", SEV_ERROR},
    {E110, "Macro used but not defined", SEV_ERROR},
    {E111, "Nested mcro definitions (not allowed)", SEV_ERROR},
    {E112, "Label on .entry or .extern line (label ignored or warned)", SEV_WARNING},

    /* Warnings */
    {W001, "Label defined but never used", SEV_WARNING},
    {W002, ".entry used on undefined label (could become error if not resolved)", SEV_WARNING},
    {W003, "Label on .entry or .extern line is ignored", SEV_WARNING},
    {W004, "Macro redefined (same name)", SEV_WARNING},
    {W005, "Forward reference to macro (macro called before defined)", SEV_WARNING},
    {W006, "Line exceeds 80 characters (may be trimmed/ignored)", SEV_WARNING},
    {W007, "Empty macro definition (mcro ... mcroend with nothing)", SEV_WARNING},
    {W008, "Multiple instructions on one line (only first parsed)", SEV_WARNING},

    /* Memory Errors */
    {MEM_E201, "Program exceeds 256-word memory limit", SEV_ERROR},
    {MEM_E202, "Instruction too long (e.g., 6 words)", SEV_ERROR},
    {MEM_E203, "Stack overflow risk (e.g., repeated jsr with no rts)", SEV_ERROR},
    {MEM_W201, "Unused .data or .mat values (e.g., more values than declared size)", SEV_WARNING},
};

#define ERROR_TABLE_SIZE (sizeof(error_table) / sizeof(ErrorInfo))

const ErrorInfo *get_error_info(ErrorCode code)
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
