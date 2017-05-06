;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************

;int agc (
;    agcState *st,      /* i/o : agc state                        */
;    Word16 *sig_in,    /* i   : postfilter input signal  (l_trm) */
;    Word16 *sig_out,   /* i/o : postfilter output signal (l_trm) */
;    Word16 agc_fac,    /* i   : AGC factor                       */
;    Word16 l_trm       /* i   : subframe size                    */
;)

       AREA     |.text|, CODE, READONLY
       EXPORT   agc_asm
       IMPORT   div_agc_s
       IMPORT   Inv_sqrt
;**********************************
; ARM register
;**********************************
; *st            RN             0
; *sig_in        RN             1
; *sig_out       RN             2
; agc_fac        RN             3
; l_trm          RN             4

agc_asm     PROC
         STMFD          r13!, {r0 - r12, r14}    
         MOV            r4, r1
         MOV            r5, r2

         VLD1.S16       {D0, D1, D2, D3}, [r5]! 
         VLD1.S16       {D4, D5, D6, D7}, [r5]!
         VLD1.S16       {D8, D9}, [r5]!
         MOV            r8, #0 
         VQDMULL.S16    Q10, D0, D0
         VQDMLAL.S16    Q10, D1, D1
         VQDMLAL.S16    Q10, D2, D2
         VQDMLAL.S16    Q10, D3, D3
         VQDMLAL.S16    Q10, D4, D4
         VQDMLAL.S16    Q10, D5, D5
         VQDMLAL.S16    Q10, D6, D6
         VQDMLAL.S16    Q10, D7, D7
         VQDMLAL.S16    Q10, D8, D8
         VQDMLAL.S16    Q10, D9, D9

         VQADD.S32      D20, D20, D21
         VEXT.32        D21, D20, D20, #1
         VQADD.S32      D20, D20, D21
         VMOV.S32       r6, D20[0] 
         
         VSHR.S16       Q0, Q0, #2
         VSHR.S16       Q1, Q1, #2
         VSHR.S16       Q2, Q2, #2
         VSHR.S16       Q3, Q3, #2
         VSHR.S16       Q4, Q4, #2
         
         CMP            r6, #0x7fffffff
         BNE            Label

         VQDMULL.S16    Q10, D0, D0
         VQDMLAL.S16    Q10, D1, D1
         VQDMLAL.S16    Q10, D2, D2
         VQDMLAL.S16    Q10, D3, D3
         VQDMLAL.S16    Q10, D4, D4
         VQDMLAL.S16    Q10, D5, D5
         VQDMLAL.S16    Q10, D6, D6
         VQDMLAL.S16    Q10, D7, D7
         VQDMLAL.S16    Q10, D8, D8
         VQDMLAL.S16    Q10, D9, D9
         VQADD.S32      D20, D20, D21
         VPADD.S32      D20, D20, D20
         VMOV.S32       r12, D20[0]
         B              Label1
Label
         MOV        r12, r6, ASR #4
         
Label1
         CMP        r12, #0
         BEQ        Label5
         CLZ        r9, r12
         SUB        r7, r9, #2 
         MOV        r9, #0x8000
         MOV        r12, r12, LSL r7
         QADD       r10, r12, r9
         MOV        r10, r10, ASR #16 

         VLD1.S16       {D0, D1, D2, D3}, [r4]! 
         VLD1.S16       {D4, D5, D6, D7}, [r4]!
         VLD1.S16       {D8, D9}, [r4]!
         VQDMULL.S16    Q10, D0, D0
         VQDMLAL.S16    Q10, D1, D1
         VQDMLAL.S16    Q10, D2, D2
         VQDMLAL.S16    Q10, D3, D3
         VQDMLAL.S16    Q10, D4, D4
         VQDMLAL.S16    Q10, D5, D5
         VQDMLAL.S16    Q10, D6, D6
         VQDMLAL.S16    Q10, D7, D7
         VQDMLAL.S16    Q10, D8, D8
         VQDMLAL.S16    Q10, D9, D9

         VQADD.S32      D20, D20, D21
         VEXT.32        D21, D20, D20, #1
         VQADD.S32      D20, D20, D21
         VMOV.S32       r6, D20[0] 

         VSHR.S16       Q0, Q0, #2
         VSHR.S16       Q1, Q1, #2
         VSHR.S16       Q2, Q2, #2
         VSHR.S16       Q3, Q3, #2
         VSHR.S16       Q4, Q4, #2
         
         CMP            r6, #0x7fffffff
         BNE            Label2

         VQDMULL.S16    Q10, D0, D0
         VQDMLAL.S16    Q10, D1, D1
         VQDMLAL.S16    Q10, D2, D2
         VQDMLAL.S16    Q10, D3, D3
         VQDMLAL.S16    Q10, D4, D4
         VQDMLAL.S16    Q10, D5, D5
         VQDMLAL.S16    Q10, D6, D6
         VQDMLAL.S16    Q10, D7, D7
         VQDMLAL.S16    Q10, D8, D8
         VQDMLAL.S16    Q10, D9, D9
         VQADD.S32      D20, D20, D21
         VPADD.S32      D20, D20, D20
         VMOV.S32       r12, D20[0]
         B              Label3

Label2
         MOV        r12, r6, ASR #4
         
Label3
         CMP        r12, #0
         MOVEQ      r4,  #0                     ; g0 = 0
         BEQ        Label4
         MOV        r5, r3
         MOV        r4, r2
         MOV        r8, r1
         CLZ        r9, r12 
         SUB        r14, r9, #1                 ; norm_l(s)
         MOV        r9, #0x8000
         MOV        r12, r12, LSL r14
         QADD       r11, r12, r9
         MOV        r11, r11, ASR #16           ; gain_in = round(s << exp) 
         SUB        r7, r7, r14                 ; exp -=i
         MOV        r6, r0
         MOV        r0, r10
         MOV        r1, r11
         BL         div_agc_s
         MOV        r12, r0                     ; s = div_s(gain_out, gain_in)  
         SSAT       r12, #31, r12, LSL #7       ; s = L_shl2(s, 7)
         CMP        r7, #0
         RSBLT      r7, r7, #0
         MOVGT      r12, r12, ASR r7
         MOVLT      r12, r12, LSL r7

         MOV        r0, r12
         BL         Inv_sqrt
         MOV        r12, r0                     ; s = Inv_sqrt(s)
         MOV        r3, r5
         MOV        r1, r8
         
         LDR        r8, =0x7fff
         
         ;MOV        r8, r9                      ; get 32767

         SSAT       r12, #31, r12, LSL #9
         MOV        r9, #0x8000
         QADD       r10, r12, r9
         MOV        r10, r10, ASR #16           ; i = round(L_shl2(s, 9))
         MOV        r2, r4 
         SUB        r9, r8, r3                  ; 32767 - agc_fac
         MUL        r11, r9, r10
         SSAT       r4, #16, r11, ASR #15 
         ; r4 --- g0
         ; r5 --- r2 : sig_out[]
         MOV        r0, r6                       ; get st address copy
Label4 
         LDRSH      r7, [r0]                     ; gain = st->past_gain
         ;for(i=0; i < l_trm; i++)
         ;gain = (gain * agc_fac)>>15
         ;gain +=g0
         ;sig_out[i] = extract_h(L_shl2(sig_out[i] * gain), 4)
         MOV        r8, #0
         MOV        r5, r2                       ; sig_out[] address
LOOP
         LDRSH      r9, [r5]                     ;sig_out[i]
         MUL        r10, r7, r3
         MOV        r7, r10, ASR #15             ;gain = (gain * agc_fac)>>15
         ADD        r7, r7, r4                   ;gain += g0
         MUL        r10, r9, r7                  ;sig_out[i] * gain
         SSAT       r11, #32, r10, LSL #4        ;L_shl2(sig_out[i] * gain, 4)
         MOV        r11, r11, ASR #16
         ADD        r8, #1
         STRH       r11, [r5], #2
         CMP        r8, #40
         BLT        LOOP
         STRH       r7, [r0]
         B          agc_asm_end
Label5
         STRH       r8, [r0]                    ;st->past_gain

agc_asm_end 
         LDMFD      r13!, {r0 - r12, r15}
         ENDP
         END
