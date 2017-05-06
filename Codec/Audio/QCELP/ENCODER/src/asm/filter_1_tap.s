;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;**************************************************************

;**********************************************************************
;  do_pole_filter_1_tap_interp  function
;**********************************************************************
;void do_pole_filter_1_tap_interp(
;		short                     *input,
;		short                     *output,
;		short                     numsamples,
;		struct POLE_FILTER_1_TAP  *filter,
;		short                     update_flag 
;		)


	AREA	|.text|, CODE, READONLY

        EXPORT   filter_1_tap_interp_asm
;******************************
; constant
;******************************
; filter->delay         Offset      0
; filter->coeff         Offset      2
; filter->max_order     Offset      4
; filter->frac          Offset      6
; filter->memory        Offset      8

;******************************
; ARM register 
;******************************
; *input               RN           0
; *output              RN           1
; numsamples           RN           2
; *filter              RN           3
; update_flag          RN           4

filter_1_tap_interp_asm     FUNCTION

          STMFD      r13!, {r4 - r12, r14}
          SUB        r13, r13, #620                   ; *tmpbuf = gArray0
          ;LDR        r4, [r13, #840]
          LDR        r4, [r3, #8]                     ; filter->memory
          LDRSH      r5, [r3, #4]                     ; filter->max_order
          MOV        r6, r13                          ; tmpbuf
          ADD        r4, r4, r5, LSL #1               ; filter->memory + filter->max_order - 1 
          SUB        r4, r4, #2  
          MOV        r12, r5      
LOOP1
          LDRSH      r7,  [r4], #-2                   ; *indata--
          LDRSH      r8,  [r4], #-2
          LDRSH      r9,  [r4], #-2 
          LDRSH      r10, [r4], #-2
          LDRSH      r11, [r4], #-2
          STRH       r7,  [r6], #2
          STRH       r8,  [r6], #2
          STRH       r9,  [r6], #2
          STRH       r10, [r6], #2
          STRH       r11, [r6], #2
          LDRSH      r7,  [r4], #-2                   ; *indata--
          LDRSH      r8,  [r4], #-2
          LDRSH      r9,  [r4], #-2 
          LDRSH      r10, [r4], #-2
          LDRSH      r11, [r4], #-2
          STRH       r7,  [r6], #2
          STRH       r8,  [r6], #2
          STRH       r9,  [r6], #2
          STRH       r10, [r6], #2
          STRH       r11, [r6], #2
          SUBS       r12, r12, #10
          BNE        LOOP1

          ADD        r4, r13, r5, LSL #1              ; outdata = tmpbuf + filter->max_order
          MOV        r6, r0
          MOV        r12, r2                          ; i = numsamples
LOOP2
          LDR        r7,  [r6], #4
          LDR        r8,  [r6], #4
          LDR        r9,  [r6], #4
          LDR        r10, [r6], #4 
          LDR        r11, [r6], #4
          STR        r7,  [r4], #4
          STR        r8,  [r4], #4
          STR        r9,  [r4], #4
          STR        r10, [r4], #4
          STR        r11, [r4], #4
          SUBS       r12, r12, #10
          BNE        LOOP2

          ; need store register : r4--filter->memory
          ; r5 --- filter->max_order, 
          LDRSH      r6, [r3, #6]                   ; r6 --- filter->frac
          LDRSH      r7, [r3, #2]                   ; r7 --- filter->coeff
          LDRSH      r8, [r3]                       ; r8 --- filter->delay
          CMP        r6, #0
          BNE        Lable
          ADD        r9, r13, r5, LSL #1            ; outdata = tmpbuf + filter->max_order
          SUB        r11, r5, r8
          ADD        r10, r13, r11, LSL #1          ; indata = tmpbuf + filter->max_order-filter->delay
          ; need store register
          ; r4 --- filter->memory r5 --- filter->max_order; 
          ; r9 --- outdata     r10 --- indata   r7 --- filter->coeff
          LDR        r4, [r3, #8]                     ; filter->memory

          LDR        r14, =0x800
          MOV        r3,  r2                        ; i = numsamples
LOOP3
          LDRSH      r6,  [r10], #2                 ; *indata ++
          LDRSH      r8,  [r9]                      ; *outdata
          MLA        r12, r6, r7, r14
          LDRSH      r11, [r10], #2                 ; *indata ++
          MOV        r12, r12, ASR #12
          ADD        r12, r12, r8
          STRH       r12, [r9], #2

          LDRSH      r8,  [r9]
          MLA        r12, r11, r7, r14
          LDRSH      r6,  [r10], #2
          MOV        r12, r12, ASR #12
          ADD        r12, r12, r8
          STRH       r12, [r9], #2

          LDRSH      r8,  [r9]
          MLA        r12, r6, r7, r14
          LDRSH      r11, [r10], #2
          MOV        r12, r12, ASR #12
          ADD        r12, r12, r8
          STRH       r12, [r9], #2

          LDRSH      r8, [r9]
          MLA        r12, r11, r7, r14
          LDRSH      r6, [r10], #2
          MOV        r12, r12, ASR #12
          ADD        r12, r12, r8
          STRH       r12, [r9], #2

          LDRSH      r8,  [r9]
          MLA        r12, r6, r7, r14
          LDRSH      r11, [r10], #2
          MOV        r12, r12, ASR #12
          ADD        r12, r12, r8
          STRH       r12, [r9], #2

 
          LDRSH      r8, [r9]
          MLA        r12, r11, r7, r14
          LDRSH      r6, [r10], #2
          MOV        r12, r12, ASR #12
          ADD        r12, r12, r8
          STRH       r12, [r9], #2          
           
 
          LDRSH      r8,  [r9]
          MLA        r12, r6, r7, r14
          LDRSH      r11, [r10], #2
          MOV        r12, r12, ASR #12
          ADD        r12, r12, r8
          STRH       r12, [r9], #2

 
          LDRSH      r8, [r9]
          MLA        r12, r11, r7, r14
          LDRSH      r6, [r10], #2
          MOV        r12, r12, ASR #12
          ADD        r12, r12, r8
          STRH       r12, [r9], #2 

          LDRSH      r8,  [r9]
          MLA        r12, r6, r7, r14
          LDRSH      r11, [r10], #2
          MOV        r12, r12, ASR #12
          ADD        r12, r12, r8
          STRH       r12, [r9], #2

          LDRSH      r8,  [r9]
          MLA        r12, r11, r7, r14
          SUBS       r3, r3, #10
          MOV        r12, r12, ASR #12
          ADD        r12, r12, r8

          STRH       r12, [r9], #2
          BNE        LOOP3
          ; need store register
          ; r4 --- filter->memory r5 --- filter->max_order; 
          LDR        r14, [r13, #660]                ;load updata_flag
Lable   
          ADD        r9, r13, r5, LSL #1              ;indata
          MOV        r10, r1                          ;outdata = output
          MOV        r3, r2
LOOP4

          LDR        r6,  [r9], #4
          LDR        r7,  [r9], #4
          LDR        r8,  [r9], #4
          LDR        r11, [r9], #4
          LDR        r12, [r9], #4
          STR        r6,  [r10], #4
          STR        r7,  [r10], #4
          STR        r8,  [r10], #4
          STR        r11, [r10], #4
          STR        r12, [r10], #4
          SUBS       r3, r3, #10
          BNE        LOOP4

          CMP        r14, #1
          BNE        filter_1_tap_interp_end
 
          ADD        r7, r5, r2
          SUB        r7, r7, #1
          ADD        r8, r13, r7, LSL #1

          ; r4 --- filter->memory       r5 --- filter->max_order
          ; r8 --- indata = tmpbuf + filter->max_order + numsamples - 1
          ; outdata = filter->memory
LOOP5
          LDRSH      r6,  [r8], #-2
          LDRSH      r7,  [r8], #-2
          LDRSH      r9,  [r8], #-2
          LDRSH      r10, [r8], #-2
          LDRSH      r11, [r8], #-2
          STRH       r6,  [r4], #2
          STRH       r7,  [r4], #2
          STRH       r9,  [r4], #2
          STRH       r10, [r4], #2
          STRH       r11, [r4], #2   
          LDRSH      r6,  [r8], #-2
          LDRSH      r7,  [r8], #-2
          LDRSH      r9,  [r8], #-2
          LDRSH      r10, [r8], #-2
          LDRSH      r11, [r8], #-2
          STRH       r6,  [r4], #2
          STRH       r7,  [r4], #2
          STRH       r9,  [r4], #2
          STRH       r10, [r4], #2
          STRH       r11, [r4], #2
          SUBS       r5, r5, #10
          BNE        LOOP5      
           
filter_1_tap_interp_end
    
          ADD        r13, r13, #620
          LDMFD      r13!, {r4 - r12, r15} 
          ENDFUNC
        
          END


