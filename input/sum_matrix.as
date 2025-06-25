; =================================================================
;           Assembly Program: Sum of Matrix Elements
; =================================================================
;
; This program calculates the sum of all elements in a 2x2 matrix
; and prints the result to standard output.
;

; --- Directive Section: Data Definition ---
.entry MAIN                 ; Declare MAIN as an entry point for the linker

; --- Data Segment ---
SIZE:   .data 2                   ; Constant representing the size of a dimension (2 for a 2x2 matrix)
SUM:    .data 0                   ; Variable to store the final sum, initialized to 0
MAT:    .mat [SIZE][SIZE] 10, -3, 8, 5  ; Define a 2x2 matrix with initial values
                                  ; MAT[0][0]=10, MAT[0][1]=-3, MAT[1][0]=8, MAT[1][1]=5
                                
; --- Code Segment ---
MAIN:   
        ; Initialization
        clr     r0              ; r0 will be the accumulator for the sum. Start with sum = 0.
        mov     SIZE, r1        ; r1 will be the row index. Load size (2) into r1.
        dec     r1              ; Decrement to get the last index (1, since indices are 0-based).
        mov     SIZE, r2        ; r2 will be the column index.
        dec     r2              ; Decrement to get the last index (1).
        mov     r2, r3          ; r3 will hold the initial column index for the inner loop reset.

; ---- Outer loop to iterate over rows (from N-1 down to 0) ----
LOOP_ROWS:
        ; Reset column index for each new row
        mov     r3, r2          ; r2 (column index) = N-1

; ---- Inner loop to iterate over columns (from N-1 down to 0) ----
LOOP_COLS:
        ; Core calculation
        lea     MAT, r4         ; Load the base address of MAT into r4 (for example purposes)
        mov     MAT[r1][r2], r5 ; Access the element at MAT[row][col] and store it in r5
        add     r5, r0          ; Add the element's value to the sum in r0
        
        ; Decrement column index and check if inner loop is done
        dec     r2
        cmp     r2, #-1         ; Compare column index with -1
        bne     LOOP_COLS       ; If not -1, continue inner loop

        ; Decrement row index and check if outer loop is done
        dec     r1
        cmp     r1, #-1         ; Compare row index with -1
        bne     LOOP_ROWS       ; If not -1, continue outer loop

; ---- Finalization ----
FINISH:
        mov     r0, SUM         ; Store the final sum from r0 into the SUM variable
        prn     SUM             ; Print the final sum to the output
        stop                    ; Halt the program