# Assembly Language Architecture

## 1. Hardware Overview
- **Memory**: 256 words (10 bits each), addresses 0–255.
- **Registers**:
  - General-purpose: `$r0`–`$r7`, 10 bits each.
  - **PSW**: Holds processor flags.
- **Stack**: Allocated in RAM.
- **Data Types**:
  - Signed 2's complement integers.
  - ASCII characters (no real numbers).

## 2. Instruction Format
Each instruction is 1–5 words (10 bits each).

### First Word Format:
| Bits  | Field                        |
|-------|------------------------------|
| 9–6   | Opcode (operation)           |
| 5–4   | Source Addressing Mode       |
| 3–2   | Destination Addressing Mode  |
| 1–0   | A/R/E (Absolute, Relocatable, External) |

### A/R/E Values:
- `00` = Absolute
- `10` = Relocatable
- `01` = External

### Base-4 Encoding (Object File):
Words are written as 5 base-4 digits: `a` = 0, `b` = 1, `c` = 2, `d` = 3.

## 3. Addressing Modes
| Mode | Syntax        | Description                                      |
|------|---------------|--------------------------------------------------|
| 0    | `#5`          | Immediate value                                  |
| 1    | `LABEL`       | Direct (label address)                           |
| 2    | `MAT[r2][r5]` | Matrix element access (2 extra words)            |
| 3    | `r1`          | Register direct                                  |

> **Note**: If both operands need extra words, **source** comes first.

## 4. Instruction Set

### Two-Operand
| Instruction | Opcode | Description                             |
|-------------|--------|-----------------------------------------|
| `mov`       | 0      | Copy source to destination              |
| `cmp`       | 1      | Compare and set `Z` flag                |
| `add`       | 2      | Add source to destination               |
| `sub`       | 3      | Subtract source from destination        |
| `lea`       | 6      | Load effective address of label         |

### One-Operand
| Instruction | Opcode | Description                             |
|-------------|--------|-----------------------------------------|
| `not`       | 4      | Bitwise NOT                             |
| `clr`       | 5      | Clear to 0                              |
| `inc`       | 7      | Increment                               |
| `dec`       | 8      | Decrement                               |
| `jmp`       | 9      | Unconditional jump                      |
| `bne`       | 10     | Branch if `Z` flag is 0                 |
| `red`       | 11     | Read character input                    |
| `prn`       | 12     | Print operand as character              |
| `jsr`       | 13     | Jump to subroutine                      |

### Zero-Operand
| Instruction | Opcode | Description                             |
|-------------|--------|-----------------------------------------|
| `rts`       | 14     | Return from subroutine (pop PC)         |
| `stop`      | 15     | Halt program execution                  |

## 5. Syntax Rules
- **Instruction**: `label: opcode src, dst`
- **Comments**: Start with `;`
- **Labels**:
  - Start with letter, max 30 chars.
  - End with `:`
  - Case-sensitive, unique, not reserved.

## 6. Directives
| Directive   | Purpose                                      |
|-------------|----------------------------------------------|
| `.data`     | Declare integers: `.data 5, -3, 9`            |
| `.string`   | Declare string: `.string "abc"` (null-terminated) |
| `.mat`      | Declare matrix: `MAT: .mat [2][2] 1,2,3,4`    |
| `.entry`    | Mark label as external entry point           |
| `.extern`   | Declare external label used here             |

## 7. Macros
- Start with `mcro name`, end with `mcroend`.
- Must be defined before use.
- Cannot nest or use reserved names.

### Example:
```asm
mcro print_r2
    prn r2
mcroend
