; --- Macro definitions ---
mcro INIT_REGS
    mov r0, #0
    mov r1, #0
    mov r2, #0
mcroend

mcro CLEAR_FLAGS
    clr flag1
    clr flag2
mcroend

mcro JUMP_TO_END
    jmp END
mcroend

; --- Program Start ---
.extern PRINT_CHAR
.entry MAIN

MAIN:
    INIT_REGS
    CLEAR_FLAGS
    mov r3, r4
    add r3, r5
