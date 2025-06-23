# Assembler Errors and Warnings Reference

---

## 🔴 Syntax Errors (Critical - halt assembly)

| Code | Description                                                          |
|-------|----------------------------------------------------------------------|
| E001  | Invalid label format (not starting with letter, too long, or contains invalid chars) |
| E002  | Label redefined (duplicate label in same file)                       |
| E003  | Label is a reserved word (instruction/macro/directive)               |
| E004  | Invalid instruction mnemonic                                         |
| E005  | Invalid directive (e.g., `.dtaa`)                                    |
| E006  | Invalid operand format (e.g., bad immediate, bad register, wrong string syntax) |
| E007  | Too many operands (e.g., 3 instead of 2)                             |
| E008  | Too few operands                                                     |
| E009  | Unexpected characters after valid statement                         |
| E010  | Missing or malformed matrix brackets `[rX][rY]`                     |

---

## 🔴 Semantic Errors (Critical - halt assembly)

| Code | Description                                                           |
|-------|-----------------------------------------------------------------------|
| E101  | Undefined label used (no matching definition)                        |
| E102  | Label used before defined, and it's not resolved in second pass     |
| E103  | `.entry` label not defined in same file                              |
| E104  | Label declared both `.entry` and `.extern`                          |
| E105  | Illegal addressing mode for instruction (e.g., `mov #5, #3`)        |
| E106  | Register name out of range (e.g., `r9`)                             |
| E107  | String not closed (`"unclosed`)                                     |
| E108  | Too many matrix initializers for given size                         |
| E109  | Invalid matrix size (zero or negative rows/cols)                    |
| E110  | Macro used but not defined                                           |
| E111  | Nested `mcro` definitions (not allowed)                             |
| E112  | Label on `.entry` or `.extern` line (label ignored or warned)       |

---

## 🟠 Warnings (Non-critical - log but allow assembly)

| Code | Description                                                          |
|-------|----------------------------------------------------------------------|
| W001  | Label defined but never used                                        |
| W002  | `.entry` used on undefined label (could become error if not resolved) |
| W003  | Label on `.entry` or `.extern` line is ignored                     |
| W004  | Macro redefined (same name)                                        |
| W005  | Forward reference to macro (macro called before defined)           |
| W006  | Line exceeds 80 characters (may be trimmed/ignored)                |
| W007  | Empty macro definition (`mcro ... mcroend` with nothing)           |
| W008  | Multiple instructions on one line (only first parsed)              |

---

## ⚠️ Memory Errors (Assembly stops if violated)

| Code | Description                                                         |
|-------|---------------------------------------------------------------------|
| E201  | Program exceeds 256-word memory limit                              |
| E202  | Instruction too long (e.g., 6 words)                              |
| E203  | Stack overflow risk (e.g., repeated `jsr` with no `rts`) – optional dynamic check |
| W201  | *[Description missing]*                                            |

---



# ⚙️ Pre-Assembler Errors and Warnings (Macro Expansion Phase Only)

This section covers only the issues detected during the **macro expansion phase** before the main assembly process.

---

## 🔴 Errors (Critical – halt assembly)

| Code  | Description                                                           |
|--------|-----------------------------------------------------------------------|
| E110  | **Macro used but not defined** – a macro is invoked but never declared. |
| E111  | **Nested `mcro` definitions** – macro definitions cannot be nested.     |

---

## 🟠 Warnings (Non-critical – allow assembly)

| Code  | Description                                                           |
|--------|-----------------------------------------------------------------------|
| W004  | **Macro redefined (same name)** – previous macro definition is overwritten. |
| W005  | **Forward reference to macro** – macro used before it is defined.         |
| W007  | **Empty macro definition** – `mcro … mcroend` contains no body.          |

---
