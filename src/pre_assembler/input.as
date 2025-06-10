; Sample macro definitions
mcro GOTO
    jmp END
mcroend

mcro LOAD_REG
    mov r1, r2
    mov r3, r4
mcroend

mcro EMPTY_MACRO
mcroend

; Some code before macros
start:  mov r0, #5

; Call to macro GOTO
GOTO

; Normal instructions
add r1, r0
sub r2, r3

; Call to macro LOAD_REG
LOAD_REG

; Call to empty macro
EMPTY_MACRO

; Label and stop
END:    stop

; End of file
