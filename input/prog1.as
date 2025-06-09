; Sample Assembly Program for the Imaginary Computer
; Computes: result = (A + B) * 2 and prints result as ASCII

.extern PRINT_CHAR

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

; Data Section
A:       .data 4
B:       .data 3
RESULT:  .data 0

; Matrix Example (unused in program, for demonstration)
MATRIX1: .mat [2][3] 1, 2, 3, 4, 5, 6

; String Example (unused)
MSG:     .string "Done"

; Macro Example
mcro DOUBLE
    add r1, r1
mcroend
