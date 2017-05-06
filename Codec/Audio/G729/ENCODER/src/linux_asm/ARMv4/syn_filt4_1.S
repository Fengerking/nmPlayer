@**************************************************************
@* Copyright 2003~2009 by VisualOn Corporation, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Corporation, Inc. ALL RIGHTS RESERVED.
@*************************************************************** 
@void Syn_filt_1 (
@    Word16 a[],     /* (i)     : a[M+1] prediction coefficients   (M=10)  */
@    Word16 x[],     /* (i)     : input signal                             */
@    Word16 y[],     /* (o)     : output signal                            */
@    Word16 mem[]    
@)
@ r0 --- a[]
@ r1 --- x[]
@ r2 --- y[]
@ r3 --- mem[]
@******************************************************************

       .section  .text
       .global   Syn_filt4_1
       .extern   myMemCopy


Syn_filt4_1:

        stmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r14} 
        sub     r13, r13, #116                   
        mov     r6, r0                           
        mov     r5, r1                           
        mov     r4, r2 
        mov     r8, #0
@memcpy(tmp, mem, 20)@
                         
        add     r0, r13, #8
        STR     r3, [r13, #4]                 
        mov     r2, #20                          
        mov     r1, r3                          
        bl      myMemCopy                         
L102:                       
        add     r9, r13, #28                      @ yy = tmp +10

L104:                       
        ldrsh   r12, [r6]                         @ tmpA = a[0]
        ldrsh   r10, [r5], #2                     @ get x[i]

        ldrsh   r11, [r9, #-2]                    @ yy[-1]
        ldrsh   r3,  [r6, #2]                     @ *(tmpA++)
        MUL     r14, r10, r12

        ldrsh   r12, [r6, #4]                     @
        ldrsh   r10, [r9, #-4]                    @ yy[-2]
        MUL     r2, r3, r11
        SUB     r14, r14, r2

        ldrsh   r3,  [r6, #6]
        ldrsh   r11, [r9, #-6]  
        MUL     r2, r12, r10
        SUB     r14, r14, r2

        ldrsh   r12, [r6, #8]
        ldrsh   r10, [r9, #-8]
        MUL     r2, r3, r11
        SUB     r14, r14, r2

        ldrsh   r3,  [r6, #10]
        ldrsh   r11, [r9, #-10]
        MUL     r2, r12, r10
        SUB     r14, r14, r2

        ldrsh   r12, [r6, #12]
        ldrsh   r10, [r9, #-12]
        MUL     r2, r3, r11
        SUB     r14, r14, r2  

        ldrsh   r3,  [r6, #14]
        ldrsh   r11, [r9, #-14]
        MUL     r2, r12, r10
        SUB     r14, r14, r2

        ldrsh   r12, [r6, #16]
        ldrsh   r10, [r9, #-16]
        MUL     r2, r3, r11
        SUB     r14, r14, r2
  
        ldrsh   r3,  [r6, #18]
        ldrsh   r11, [r9, #-18]
        MUL     r2, r12, r10
        SUB     r14, r14, r2

        ldrsh   r12, [r6, #20]
        ldrsh   r10, [r9, #-20]
        MUL     r2, r3, r11
        SUB     r14, r14, r2
        MUL     r2, r12, r10
        SUB     r14, r14, r2

        @ s = ASM_L_shl(s, 4)
        MOV     r10, r14
        MOV     r14, r10, ASL #4
        TEQ     r10, r14, ASR #4
        MOVNE   r11, #0x7fffffff
        EORNE   r14, r11, r10, ASR #31


        MOV     r10, #0x8000
        ADDS    r14, r14, r10
        MOVVS   r14, #0x7fffffff
        MOV     r14, r14, ASR #16

        ADD     r8, r8, #1
        STRH    r14, [r9], #2
        CMP     r8, #40
        BLT     L104

        ADD     r1, r13, #28
        mov     r2, #80                           
        mov     r0, r4                           
        bl      myMemCopy 
                       
        LDR     r0, [r13, #4]
        ADD     r1, r4, #60
        MOV     r2, #20
        bl      myMemCopy 
        
  
        add     r13, r13, #116                    @  102.1
        ldmfd   r13!, {r4, r5, r6, r7, r8, r9, r10, r11, r15} @  102.1  
        
        @ENDP
        .END



