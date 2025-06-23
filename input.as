; Test file for the pre-processor (macro expansion phase)

; --- VALID MACRO DEFINITIONS ---

mcro inc_two_regs
    inc r1
    inc r2
mcroend

mcro load_data
    lea STR, r6
    mov LENGTH, r5
mcroend


; --- START OF CODE ---
; This section tests basic, valid macro expansion.

MAIN:   mov M1[r2][r7],LENGTH
        add r2,STR

; First macro call
inc_two_regs

LOOP:   jmp END
        prn #-5

; Second macro call
load_data

END:    stop

; --- DATA DEFINITIONS (Should be unaffected) ---

STR:    .string "abcdef"
LENGTH: .data 6,-9,15
M1: .mat [2][2] 1,2,3,4


; --- INVALID MACRO DEFINITIONS (Should cause errors) ---

; Error 1: Macro name is a reserved instruction name
mcro mov
    sub r1, r1
mcroend

; Error 2: Extra text on the 'mcro' definition line
mcro bad_def some_extra_text
    clr r3
mcroend

; Error 3: Extra text on the 'mcroend' line
mcro another_bad_def
    not r4
mcroend this_is_invalid