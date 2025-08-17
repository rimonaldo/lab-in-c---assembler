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

This section lists all assembler errors and warnings grouped by category.  
**Errors (`E`)** are critical and will halt assembly.  
**Warnings (`W`)** indicate issues but assembly may still succeed.

---

### 📖 Legend (Code Ranges)

- **400–499** → Preprocessor (macros)  
- **500–599** → Labels and symbol table  
- **600–699** → Instructions and operands  
- **700–799** → Memory (⚠️ *all memory errors halt immediately*)  

---

### 🔴 Preprocessor Errors (400–499)

| Code | Description                                                                 |
| :--- | :-------------------------------------------------------------------------- |
| **E400** | Macro used but not defined (`E400_MACRO_UNDEFINED`) ✅ |
| **E401** | Nested macro definitions are not allowed (`E401_MACRO_NESTED`) ❌ |
| **W402** | Macro defined without a name (`W402_MACRO_UNNAMED`) ✅ |
| **W403** | Macro redefined – only the *first* definition is valid (`W403_MACRO_REDEFINED`) ✅ |
| **W404** | Empty macro body (`W404_MACRO_EMPTY`) ✅ |

---

### 🔴 Label Errors (500–599)

| Code | Description                                                                 |
| :--- | :-------------------------------------------------------------------------- |
| **E500** | Invalid label format (must start with letter, legal chars, max length) ✅ |
| **E501** | Label name is reserved (instruction, directive, or macro name) ✅ |
| **E502** | Label redefined in the same file (`E502_LABEL_REDEFINED`) ✅ |
| **E503** | Undefined label referenced (`E503_LABEL_UNDEFINED`) ✅ |
| **E504** | Label declared both `.entry` and `.extern` (`E504_LABEL_ENTRY_AND_EXTERN`) ❌ |
| **W505** | `.entry` label not found in file (`W505_LABEL_ENTRY_NOT_FOUND`) ✅ |
| **W506** | Label defined on the same line as `.entry`/`.extern` (`W506_LABEL_ON_ENTRY_OR_EXTERN`) ❌ |
| **W507** | Label defined but never used (`W507_LABEL_UNUSED`) ❌ (planned for 2nd pass) |

---

### 🔴 Instruction & Operand Errors (600–699)

| Code | Description                                                                 |
| :--- | :-------------------------------------------------------------------------- |
| **E600** | Invalid instruction mnemonic (`E600_INSTRUCTION_NAME_INVALID`) ✅ |
| **E601** | Invalid instruction format (wrong operand count/structure) ✅ |
| **E602** | Unexpected characters after valid instruction (`E602_INSTRUCTION_TRAILING_CHARS`) ✅ |
| **E610** | Invalid immediate value syntax (`E610_OPERAND_IMMEDIATE_INVALID`) ✅ |
| **E611** | Immediate value out of range (`E611_OPERAND_IMMEDIATE_OUT_OF_BOUNDS`) ✅ |
| **E612** | Immediate value must be integer, not float (`E612_OPERAND_IMMEDIATE_FLOAT`) ✅ |
| **E613** | Invalid register name (`E613_OPERAND_REGISTER_INVALID`) ✅ |
| **E614** | Invalid matrix index expression (`E614_OPERAND_MAT_INDEX_INVALID`) ✅ |
| **E615** | Matrix index out of range (`E615_OPERAND_MAT_INDEX_OUT_OF_BOUNDS`) ✅ |
| **W616** | Matrix initialized with too few values (`W616_OPERAND_MAT_INITIALIZED_UNDER`) ✅ |
| **W617** | Matrix initialized with too many values (`W617_OPERAND_MAT_INITIALIZED_OVER`) ❌ |

---

### 🔴 Memory Errors (700–799)

⚠️ **All memory-related errors are fatal and will cause immediate program halt.**

| Code | Description                                                                 |
| :--- | :-------------------------------------------------------------------------- |
| **E700** | Program exceeds 256-word memory limit (`E700_MEMORY_PROGRAM_WORD_LIMIT`) ✅ |
| **E701** | Source line exceeds maximum character limit (`E701_MEMORY_LINE_CHAR_LIMIT`) ✅ |
| **W703** | Unused data section detected (`W703_MEMORY_UNUSED_DATA`) ❌ |

---

✅ = Implemented  
❌ = Not yet implemented


---
