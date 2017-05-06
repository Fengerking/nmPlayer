;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;*************************************************************** 
;void Scale_sig(
;               Word16 x[],                           /* (i/o) : signal to scale               */
;               Word16 lg,                            /* (i)   : size of x[]                   */
;               Word16 exp                            /* (i)   : exponent: x = round(x << exp) */
;)
;****************************************************************
;  x[]   ---  r0
;  lg    ---  r1
;  exp   ---  r2

          AREA   |.text|, CODE, READONLY
          EXPORT  Scale_sig_asm

Scale_sig_asm FUNCTION
          STMFD   	r13!, {r4 - r12, r14} 
          ;MOV           r8, #0                          ; i =0
          CMP           r2, #0
          MOV           r14, r0                          ; copy x[] address 
          BGT           LOOP1
          RSB           r3, r2, #0                      ; -exp

LOOP                
          LDRSH         r4, [r14], #2                    
          LDRSH         r5, [r14], #2
          LDRSH         r6, [r14], #2
          LDRSH         r7, [r14], #2         
          MOV           r9, r4, LSL #16
          MOV           r10, r5, LSL #16
          MOV           r11, r6, LSL #16
          MOV           r12, r7, LSL #16
          MOV           r4, r9, ASR r3
          MOV           r5, r10, ASR r3
          MOV           r6, r11, ASR r3
          MOV           r7, r12, ASR r3
          ADD           r9, r4, #0x8000
          ADD           r10, r5, #0x8000
          ADD           r11, r6, #0x8000
          ADD           r12, r7, #0x8000
          MOV           r4, r9, ASR #16
          MOV           r5, r10, ASR #16
          MOV           r6, r11, ASR #16
          MOV           r7, r12, ASR #16
          STRH          r4, [r0], #2
          STRH          r5, [r0], #2
          STRH          r6, [r0], #2
          STRH          r7, [r0], #2
          SUBS          r1, r1, #4
          BGT           LOOP
          B             Scale_sig_asm_end

LOOP1
          LDRSH         r4, [r14], #2                    
          LDRSH         r5, [r14], #2
          LDRSH         r6, [r14], #2
          LDRSH         r7, [r14], #2         
          MOV           r9, r4, LSL #16
          MOV           r10, r5, LSL #16
          MOV           r11, r6, LSL #16
          MOV           r12, r7, LSL #16
          MOV           r4, r9, LSL r2
          MOV           r5, r10, LSL r2
          MOV           r6, r11, LSL r2
          MOV           r7, r12, LSL r2
          ADD           r9, r4, #0x8000
          ADD           r10, r5, #0x8000
          ADD           r11, r6, #0x8000
          ADD           r12, r7, #0x8000
          MOV           r4, r9, ASR #16
          MOV           r5, r10, ASR #16
          MOV           r6, r11, ASR #16
          MOV           r7, r12, ASR #16
          STRH          r4, [r0], #2
          STRH          r5, [r0], #2
          STRH          r6, [r0], #2
          STRH          r7, [r0], #2
          SUBS          r1, r1, #4
          BGT           LOOP1      
                          
Scale_sig_asm_end		     
          LDMFD   	r13!, {r4 - r12, r15} 
          ENDFUNC
          END
 

