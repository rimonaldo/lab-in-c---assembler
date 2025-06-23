; Example program for imaginary computer

MAIN:       mov     r3, COUNT       ; Copy COUNT to r3
            cmp     #0, r3          ; Compare with 0
            bne     PROCESS         ; If not zero, jump to PROCESS
            stop                    ; Otherwise, halt execution

; ---- Subroutine ----
PROCESS:    jsr     PRINT_RESULT    ; Call PRINT_RESULT
            dec     COUNT           ; COUNT--
            jmp     MAIN            ; Repeat

; ---- Subroutine to print result ----
PRINT_RESULT:
            lea     MSG, r1         ; Load address of MSG
            prn     r1              ; Print it
            rts                    ; Return

; ---- Data ----
COUNT:      .data   3
MSG:        .string "Done!"
MATRIX1:    .mat [2][2] 1, 2, 3, 4

; ---- External and Entry ----
.entry MAIN
.extern EXT_FUNC

; ---- Macro definition ----
mcro PRINT_MSG
    prn r1
    prn r2
mcroend

; Usage of macro
            PRINT_MSG
