# Syntax Guide: Assembly Language for an Imaginary Computer

This document specifies the syntax, semantics, and error system for the assembly language of a 10-bit imaginary computer.  
It is intended as a **reference for both users and implementers of the assembler**.  
It defines the instruction set, directives, addressing modes, macros, and error handling rules.  

> 📘 **Note:** Examples are provided throughout for clarity.  
> 🔴 **Errors (`E`)** halt assembly.  
> ⚠️ **Warnings (`W`)** indicate issues but assembly may still succeed.

---

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
    - [📖 Legend (Code Ranges)](#-legend-code-ranges)  
    - [🔴 Preprocessor Errors (400–499)](#-preprocessor-errors-400499)  
    - [🔴 Label Errors (500–599)](#-label-errors-500599)  
    - [🔴 Instruction & Operand Errors (600–699)](#-instruction--operand-errors-600699)  
    - [🔴 Memory Errors (700–799, Fatal)](#-memory-errors-700799)  

---

## 1. Hardware and Data Types

- **CPU:** Operates on **10-bit words**.  
- **Registers:**
  - 8 General-Purpose Registers (GPRs), 10 bits wide: `r0` through `r7`.  
  - `PSW` (Program Status Word): contains status flags (e.g., Zero flag `Z`).  
- **Memory (RAM):**
  - 256 cells (addresses `0–255`).  
  - Each cell is 10 bits wide.  
- **Data Types:**
  - **Integers:** Signed (2’s complement), 10-bit.  
  - **Characters:** ASCII, 1 per 10-bit word.  

---

## 2. General Syntax

### Statement Structure

- **Max Line Length:** 80 characters (excluding newline).  
- **Empty Lines:** Ignored.  
- **Comment Lines:** Start with a semicolon `;`.  
- **Statement Types:**
  - **Directive Statements** → tell assembler what to do (`.data`, `.extern`).  
  - **Instruction Statements** → CPU operations (`mov`, `add`).  

Example:
```assembly
; sample program
MAIN:   mov  #5, r1
        add  r1, r2
        stop
```

### Labels

- **Definition:** A symbolic name for a memory address.  
- **Syntax:**
  - Start with a letter (`A–Z` / `a–z`).  
  - Continue with alphanumeric characters.  
  - End with a colon `:` (not part of the name).  
  - Max length: **30 characters**.  
  - Case-sensitive.  
  - Cannot be a reserved word (instruction, directive, or register).  
- **Value:** IC (Instruction Counter) or DC (Data Counter) at definition.  
- **Rules:**
  - Must be unique within file.  
  - Forward referencing allowed.  

Example:
```assembly
LOOP:   cmp  r1, r2
        bne  LOOP
```

⚠️ **See errors [E500–E503](#-label-errors-500599)** for invalid label cases.  

---

## 3. Operands and Addressing Modes

### Operands

- **Numbers:** Decimal integers, with optional sign (`+17`, `#-5`).  
- **Strings:** Double-quoted ASCII (`"Hello"`).  
- **Registers:** `r0`–`r7`.  

### Addressing Modes

| Mode # | Name              | Syntax               | Description |
| :----- | :---------------- | :------------------- | :---------- |
| **0**  | Immediate          | `#<number>`          | Operand is the number itself. |
| **1**  | Direct             | `<label>`            | Operand is the memory at label’s address. |
| **2**  | Matrix Access      | `<label>[reg1][reg2]`| Access element in matrix at offset. |
| **3**  | Direct Register    | `<register>`         | Operand is content of register. |

---

## 4. Instruction Set

All instructions are lowercase.  

### Two-Operand Instructions

**Format:**  
```assembly
[label:] opcode src, dest
```

| Mnemonic | Opcode | Description | Flags |
|----------|--------|-------------|-------|
| `mov`    | 0 | Copy source to dest. | — |
| `cmp`    | 1 | Compare, set `Z` if equal. | Z |
| `add`    | 2 | `dest = dest + src`. | Z |
| `sub`    | 3 | `dest = dest - src`. | Z |
| `lea`    | 6 | Load effective address of src into dest. | — |

### One-Operand Instructions

**Format:**  
```assembly
[label:] opcode dest
```

| Mnemonic | Opcode | Description | Flags |
|----------|--------|-------------|-------|
| `not`    | 4 | 1’s complement. | — |
| `clr`    | 5 | Clear to 0. | Z |
| `inc`    | 7 | Increment. | Z |
| `dec`    | 8 | Decrement. | Z |
| `jmp`    | 9 | Unconditional jump. | — |
| `bne`    | 10 | Branch if `Z=0`. | — |
| `red`    | 11 | Read char into operand. | — |
| `prn`    | 12 | Print operand. | — |
| `jsr`    | 13 | Jump to subroutine. | — |

### Zero-Operand Instructions

**Format:**  
```assembly
[label:] opcode
```

| Mnemonic | Opcode | Description |
|----------|--------|-------------|
| `rts`    | 14 | Return from subroutine. |
| `stop`   | 15 | Halt program. |

---

## 5. Assembler Directives

- **`.data <n1>, <n2>, ...`**  
  Allocate/init memory with integers.  
  ```assembly
  VALUES: .data 7, -57, +17
  ```

- **`.string "..."`**  
  Allocate characters + null terminator.  
  ```assembly
  MSG: .string "Result:"
  ```

- **`.mat [rows][cols] values...`**  
  Allocate/init 2D matrix.  
  ```assembly
  MATRIX: .mat [2][2] 4, -5, 7, 9
  ```

- **`.entry <label>`**  
  Declare entry point (for linker).  

- **`.extern <label>`**  
  Declare external symbol.  

---

## 6. Macros

**Definition:**
```assembly
mcro PRINT
    prn r1
mcroend
```

**Invocation:**
```assembly
PRINT
```

**Restrictions:**
- Must be defined before use.  
- Nested macros not allowed (see [E401](#-preprocessor-errors-400499)).  

---

## 7. Valid Addressing Modes per Instruction

| Instruction | Opcode | Legal Source | Legal Dest |
|-------------|--------|--------------|------------|
| mov | 0 | 0,1,2,3 | 1,2,3 |
| cmp | 1 | 0,1,2,3 | 0,1,2,3 |
| add | 2 | 0,1,2,3 | 1,2,3 |
| sub | 3 | 0,1,2,3 | 1,2,3 |
| lea | 6 | 1,2 | 1,2,3 |
| not | 4 | — | 1,2,3 |
| clr | 5 | — | 1,2,3 |
| inc | 7 | — | 1,2,3 |
| dec | 8 | — | 1,2,3 |
| jmp | 9 | — | 1,3 |
| bne | 10 | — | 1,3 |
| red | 11 | — | 1,2,3 |
| prn | 12 | — | 0,1,2,3 |
| jsr | 13 | — | 1,3 |
| rts | 14 | — | — |
| stop | 15 | — | — |

---

## 8. Errors and Warnings Reference

Errors and warnings are categorized.  
**Errors halt assembly immediately.**  
**Warnings allow continuation.**

---

### 📖 Legend (Code Ranges)

- **400–499** → Preprocessor (macros)  
- **500–599** → Labels & symbols  
- **600–699** → Instructions & operands  
- **700–799** → Memory (⚠️ *fatal*)  

---

### 🔴 Preprocessor Errors (400–499)

| Code | Severity | Description | Implemented |
|------|----------|-------------|-------------|
| **E400** | 🔴 | Macro used but not defined | ✅ |
| **E401** | 🔴 | Nested macro definition | ❌ |
| **W402** | 🟠 | Macro with no name | ✅ |
| **W403** | 🟠 | Macro redefined; only first valid | ✅ |
| **W404** | 🟠 | Empty macro body | ✅ |

---

### 🔴 Label Errors (500–599)

| Code | Severity | Description | Implemented |
|------|----------|-------------|-------------|
| **E500** | 🔴 | Invalid label format | ✅ |
| **E501** | 🔴 | Label is reserved word | ✅ |
| **E502** | 🔴 | Label redefined | ✅ |
| **E503** | 🔴 | Undefined label used | ✅ |
| **E504** | 🔴 | Label both `.entry` and `.extern` | ❌ |
| **W505** | 🟠 | `.entry` label not found | ✅ |
| **W506** | 🟠 | Label defined on `.entry`/`.extern` line | ❌ |
| **W507** | 🟠 | Label unused | ❌ |

---

### 🔴 Instruction & Operand Errors (600–699)

| Code | Severity | Description | Implemented |
|------|----------|-------------|-------------|
| **E600** | 🔴 | Invalid instruction name | ✅ |
| **E601** | 🔴 | Invalid instruction format | ✅ |
| **E602** | 🔴 | Extra characters after instruction | ✅ |
| **E610** | 🔴 | Invalid immediate syntax | ✅ |
| **E611** | 🔴 | Immediate out of range | ✅ |
| **E612** | 🔴 | Immediate must be integer, not float | ✅ |
| **E613** | 🔴 | Invalid register | ✅ |
| **E614** | 🔴 | Invalid matrix index expr. | ✅ |
| **E615** | 🔴 | Matrix index out of range | ✅ |
| **W616** | 🟠 | Matrix initialized under capacity | ✅ |
| **W617** | 🟠 | Matrix initialized over capacity | ❌ |

---

### 🔴 Memory Errors (700–799)

⚠️ **All memory errors are fatal.**

| Code | Severity | Description | Implemented |
|------|----------|-------------|-------------|
| **E700** | 🔴 | Program exceeds 256 words | ✅ |
| **E701** | 🔴 | Source line exceeds max length | ✅ |
| **W703** | 🟠 | Unused data section | ❌ |

---

