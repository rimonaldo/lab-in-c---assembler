# Syntax Guide: Assembly Language for an Imaginary Computer

This document outlines the syntax and features of the assembly language for the specified imaginary computer, intended as a reference for implementing a full-featured assembler.

## Table of Contents
1.  [Hardware and Data Types](#1-hardware-and-data-types)
2.  [General Syntax](#2-general-syntax)
    - [Statement Structure](#statement-structure)
    - [Labels](#labels)
3.  [Operands and Addressing Modes](#3-operands-and-addressing-modes)
    - [Operands](#operands)
    - [Addressing Modes](#addressing-modes)
4.  [Instruction Set](#4-instruction-set)
    - [Two-Operand Instructions](#two-operand-instructions)
    - [One-Operand Instructions](#one-operand-instructions)
    - [Zero-Operand Instructions](#zero-operand-instructions)
5.  [Assembler Directives](#5-assembler-directives)
6.  [Macros](#6-macros)
7.  [Valid Addressing Modes per Instruction](#7-valid-addressing-modes-per-instruction)
8.  [Errors and Warnings Reference](#8-errors-and-warnings-reference)
    - [🔴 Syntax Errors](#-syntax-errors)
    - [🔴 Semantic Errors](#-semantic-errors)
    - [⚠️ Memory Errors](#️-memory-errors)
    - [⚙️ Pre-Assembler (Macro) Errors and Warnings](#️-pre-assembler-macro-errors-and-warnings)


## 1. Hardware and Data Types

-   **CPU:** Operates on **10-bit words**.
-   **Registers:**
    -   8 General-Purpose Registers (GPRs), 10 bits wide: `r0` through `r7`.
    -   `PSW` (Program Status Word): Contains status flags (e.g., Zero flag `Z`).
-   **Memory (RAM):**
    -   256 cells (addresses 0 to 255).
    -   Each cell (word) is 10 bits wide.
-   **Data Types:**
    -   **Integers:** Signed (2's complement), 10-bit.
    -   **Characters:** ASCII, stored one character per 10-bit word.

## 2. General Syntax

### Statement Structure

-   **Max Line Length:** 80 characters (excluding newline).
-   **Empty Lines:** Ignored by the assembler.
-   **Comment Lines:** Start with a semicolon (`;`) and are ignored.
-   **Statement Types:**
    -   **Directive Statements:** Instructions for the assembler (e.g., `.data`, `.extern`).
    -   **Instruction Statements:** CPU operations (e.g., `mov`, `add`).

### Labels

-   **Definition:** A symbolic name for a memory address.
-   **Syntax:**
    -   Starts with an alphabetic character (a-z, A-Z).
    -   Followed by zero or more alphanumeric characters.
    -   Terminated by a colon (`:`) which is **not** part of the label name (e.g., `LOOP:`).
    -   Maximum length: 30 characters.
    -   Case-sensitive.
    -   Cannot be a reserved word (instruction name, directive name, register name).
-   **Value:** The value of the Instruction Counter (IC) or Data Counter (DC) at the point of definition.
-   **Rules:**
    -   A label must be unique within a source file.
    -   Forward referencing (using a label before it is defined) is allowed.

## 3. Operands and Addressing Modes

### Operands

-   **Numbers:** Decimal integers, optionally preceded by `+` or `-` (e.g., `25`, `#-5`, `+100`).
-   **Strings:** A sequence of visible ASCII characters enclosed in double quotes (e.g., `"Hello"`).
-   **Registers:** `r0` through `r7`.

### Addressing Modes

| Mode # | Name             | Syntax                  | Description                                                        |
| :----- | :--------------- | :---------------------- | :----------------------------------------------------------------- |
| **0** | **Immediate** | `#<number>`             | The operand is the number itself.                                  |
| **1** | **Direct** | `<label>`               | The operand is the content of the memory word at the address of the label. |
| **2** | **Matrix Access** | `<label>[reg1][reg2]`    | Accesses an element in a matrix. `label` is the base address.      |
| **3** | **Direct Register**| `<register>`            | The operand is the content of the specified register.              |

## 4. Instruction Set

All instructions are written in lowercase.

### Two-Operand Instructions
**Format:** `[label:] opcode source_operand, destination_operand`

| Mnemonic | Opcode | Description                                                                |
| :------- | :----- | :------------------------------------------------------------------------- |
| `mov`    | 0      | **Move**: Copies the source operand to the destination operand.            |
| `cmp`    | 1      | **Compare**: Compares source and destination operands, sets `Z` flag if equal. |
| `add`    | 2      | **Add**: Adds operands, stores result in destination. `dest = dest + src`   |
| `sub`    | 3      | **Subtract**: Subtracts source from destination. `dest = dest - src`      |
| `lea`    | 6      | **Load Effective Address**: Stores the memory address of the source operand into the destination. |

### One-Operand Instructions
**Format:** `[label:] opcode destination_operand`

| Mnemonic | Opcode | Description                                                              |
| :------- | :----- | :----------------------------------------------------------------------- |
| `not`    | 4      | **Bitwise NOT**: Performs a 1's complement of the operand.               |
| `clr`    | 5      | **Clear**: Sets the operand to 0.                                        |
| `inc`    | 7      | **Increment**: Increments the operand by 1.                              |
| `dec`    | 8      | **Decrement**: Decrements the operand by 1.                              |
| `jmp`    | 9      | **Jump**: Unconditional jump to the operand's address.                   |
| `bne`    | 10     | **Branch if Not Equal**: Jumps if the `Z` flag is 0.                     |
| `red`    | 11     | **Read**: Reads a character from standard input into the operand.        |
| `prn`    | 12     | **Print**: Prints a character from the operand to standard output.       |
| `jsr`    | 13     | **Jump to Subroutine**: Pushes return address and jumps to the subroutine address. |

### Zero-Operand Instructions
**Format:** `[label:] opcode`

| Mnemonic | Opcode | Description                                                  |
| :------- | :----- | :----------------------------------------------------------- |
| `rts`    | 14     | **Return from Subroutine**: Pops an address from the stack into the PC. |
| `stop`   | 15     | **Stop**: Halts program execution.                           |

## 5. Assembler Directives

-   `.data <num1>[, <num2>, ...]`
    -   Allocates and initializes memory words with integer values.
    -   **Example:** `VALUES: .data 7, -57, +17`
-   `.string "<characters>"`
    -   Allocates memory for each character and appends a null terminator (`\0`).
    -   **Example:** `MSG: .string "Result:"`
-   `.mat [<rows>][<cols>] [<val1>, ...]`
    -   Allocates space for a 2D matrix, with optional initialization.
    -   **Example:** `MATRIX: .mat [2][2] 4, -5, 7, 9`
-   `.entry <label>`
    -   Declares that `<label>` is an entry point, making it visible to other files during linking.
-   `.extern <label>`
    -   Declares that `<label>` is an external symbol defined in another file, to be resolved by the linker.

## 6. Macros

-   **Definition:**
    ```assembly
    mcro <macro_name>
        ; assembly statements...
    mcroend
    ```
-   **Invocation:**
    ```assembly
    <macro_name>
    ```
-   **Restrictions:**
    -   Macros must be defined before they are invoked.
    -   Nested macro definitions are not allowed.

## 7. Valid Addressing Modes per Instruction

| Instruction | Opcode | Legal Source Modes | Legal Destination Modes |
| :---------- | :----: | :----------------: | :---------------------: |
| **mov** | 0      | 0, 1, 2, 3         | 1, 2, 3                 |
| **cmp** | 1      | 0, 1, 2, 3         | 0, 1, 2, 3              |
| **add** | 2      | 0, 1, 2, 3         | 1, 2, 3                 |
| **sub** | 3      | 0, 1, 2, 3         | 1, 2, 3                 |
| **lea** | 6      | 1, 2               | 1, 2, 3                 |
| **not** | 4      | N/A                | 1, 2, 3                 |
| **clr** | 5      | N/A                | 1, 2, 3                 |
| **inc** | 7      | N/A                | 1, 2, 3                 |
| **dec** | 8      | N/A                | 1, 2, 3                 |
| **jmp** | 9      | N/A                | 1, 3                    |
| **bne** | 10     | N/A                | 1, 3                    |
| **red** | 11     | N/A                | 1, 2, 3                 |
| **prn** | 12     | N/A                | 0, 1, 2, 3              |
| **jsr** | 13     | N/A                | 1, 3                    |
| **rts** | 14     | N/A                | N/A                     |
| **stop** | 15     | N/A                | N/A                     |

## 8. Errors and Warnings Reference

This section provides a comprehensive list of potential errors and warnings generated by the assembler.

### 🔴 Syntax Errors
*(Critical - assembly will halt)*

| Code | Description                                                                    |
| :--- | :----------------------------------------------------------------------------- |
| E001 | Invalid label format (not starting with letter, too long, or invalid characters) |
| E002 | Label redefined (duplicate label in the same file)                             |
| E003 | Label is a reserved word (instruction, macro, or directive name)               |
| E004 | Invalid instruction mnemonic                                                   |
| E005 | Invalid directive name (e.g., `.dtaa`)                                         |
| E006 | Invalid operand format (e.g., bad immediate, bad register, wrong string syntax)  |
| E007 | Too many operands for instruction                                              |
| E008 | Too few operands for instruction                                               |
| E009 | Unexpected characters after a valid statement                                  |
| E010 | Missing or malformed matrix brackets for matrix access (e.g., `<label>[rX]`)   |

### 🔴 Semantic Errors
*(Critical - assembly will halt)*

| Code | Description                                                                    |
| :--- | :----------------------------------------------------------------------------- |
| E101 | Undefined label used in an operand                                             |
| E102 | Forward-referenced label was never defined in the file                         |
| E103 | Label marked with `.entry` is not defined within the file                      |
| E104 | Label declared as both `.entry` and `.extern`                                  |
| E105 | Illegal addressing mode for the given instruction (e.g., `mov #5, #3`)         |
| E106 | Register name is out of the valid range `r0`-`r7` (e.g., `r9`)                 |
| E107 | String literal is not closed with a double quote                               |
| E108 | Too many initializers provided for the defined matrix size                     |
| E109 | Invalid matrix size (e.g., zero or negative rows/cols)                         |
| E112 | A label was defined on the same line as an `.entry` or `.extern` directive     |

### ⚠️ Memory Errors
*(Critical - assembly will halt)*

| Code | Description                                                                    |
| :--- | :----------------------------------------------------------------------------- |
| E201 | Program exceeds the 256-word memory limit of the machine                       |
| E202 | A single instruction requires too many memory words (e.g., > 4 words)          |
| E203 | Stack overflow risk detected (optional dynamic check)                          |
| W201 | *[Description missing]* |

### ⚙️ Pre-Assembler (Macro) Errors and Warnings

*(Issues detected during the macro expansion phase)*

| Type    | Code | Description                                                        |
| :------ | :--- | :----------------------------------------------------------------- |
| **Error** 🔴 | E110 | **Macro Not Defined:** A macro is invoked before it was ever defined.    |
| **Error** 🔴 | E111 | **Nested Macro Definition:** A `mcro` block appears inside another.  |
| **Warning** 🟠 | W004 | **Macro Redefined:** A macro with the same name is defined again.      |
| **Warning** 🟠 | W007 | **Empty Macro Definition:** A `mcro ... mcroend` block contains no statements. |

---