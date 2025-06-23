; --- Continue program logic ---

    JUMP_TO_END

    ; This should be skipped
    sub r5, r6
    mov r7, r8

END:
    stop

; --- Data Section ---
A:      .data 4
B:      .data 3
RESULT: .data 0

MATRIX1: .mat [2][3] 1, 2, 3, 4, 5, 6
MSG:     .string "Done"
