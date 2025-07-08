; Sample Assembly Program for the Imaginary Computer
; Computes: result = (A + B) * 2 and prints result as ASCII

.extern PRINT_CHAR




CODE:    mov  A, r1
         add  B, r1
         bne  MAIN


MAIN:    mov  A, r1
         add  B, r1
         mov  r1, r2
         add  r2, r1
         mov  r1, RESULT
         lea  RESULT, r3
         jsr  PRINT_CHAR
         stop

; Subroutine defined in external file
.entry MAIN
; data ERROR example (missing values)
A:       .data 4, 2, 6, ,,, 12
; Data Section
B:       .data 3
RESULT:  .data 0

; Matrix ERROR Example (missing row or col size)
MATRIX1: .mat [3]

; Matrix INIT Example (initialize rest of values to 0)
MATRIX2: .mat [3][2] 1, 2, 3

; String Example (unused)
MSG:     .string "Done"

MAIN2:    mov  A, r1
         add  B, r1
         mov  r1, r2
         add  r2, r1
         mov  r1, RESULT
         lea  RESULT, r3
         jsr  PRINT_CHAR
         stop


; Macro Example
mcro DOUBLE
    add r1, r1
mcroend
