; Largest assembly program for the imaginary computer

.entry MAIN
.extern EXT_ROUTINE

; ========== Code Segment Starts Here ==========

MAIN:       lea     BIGSTR, r0
            prn     r0
            mov     #100, r1
LOOP:       cmp     r1, #0
            bne     BODY
            stop

BODY:       jsr     COMPUTE
            dec     r1
            jmp     LOOP

COMPUTE:    mov     #3, r2
            add     r2, r3
            mov     MAT[r4][r5], r6
            sub     #1, r6
            red     r7
            prn     r6
            jsr     EXT_ROUTINE
            rts

; Macro to consume space
mcro FILL
    add     #1, r0
    add     #2, r1
    add     #3, r2
    add     #4, r3
    add     #5, r4
    add     #6, r5
mcroend

; Use macro multiple times
        FILL
        FILL
        FILL
        FILL

; Additional matrix access instructions to add word-heavy ops
        mov     MAT[r2][r3], r1
        mov     MAT[r3][r4], r2
        mov     MAT[r4][r5], r3
        mov     MAT[r5][r6], r4

; ========== Data Segment Starts Here ==========

; Long string (21 words)
BIGSTR:     .string "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

; Large .data array (50 words)
BIGDATA:    .data 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
            .data 16,17,18,19,20,21,22,23,24,25
            .data 26,27,28,29,30,31,32,33,34,35
            .data 36,37,38,39,40,41,42,43,44,45
            .data 46,47,48,49

; Several .mat arrays to consume rows x cols words each
MAT:        .mat [5][5] 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
MAT2:       .mat [3][4] 1,1,1,1,2,2,2,2,3,3,3,3
MAT3:       .mat [2][6] 0,0,0,0,0,0,1,1,1,1,1,1
