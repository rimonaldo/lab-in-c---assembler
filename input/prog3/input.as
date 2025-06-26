; Macro definitions

mcro INIT_REGS
    mov r0, #0
    mov r1, #0
    mov r2, #0
mcroend

mcro CLEAR_FLAGS
    clr flag1
    clr flag2
mcroend

mcro EMPTY_MACRO
mcroend

mcro JUMP_TO_END
    jmp END
mcroend

; Program start

start:
    ; Initialize registers
    INIT_REGS

    ; Clear flags
    CLEAR_FLAGS

    ; Call empty macro - should produce no output
    EMPTY_MACRO

    ; Normal instructions
    mov r3, r4
    add r3, r5

    ; Jump to end
    JUMP_TO_END

    ; These instructions should be skipped by jump
    sub r5, r6
    mov r7, r5

END:
    stop
