@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************

@void agc2 (
@ Word16 *sig_in,        /* i   : postfilter input signal  */
@ Word16 *sig_out,       /* i/o : postfilter output signal */
@ Word16 l_trm           /* i   : subframe size            */
@)
        #include "voAMRNBDecID.h"
       .section .text
       .global   agc2_asm
       .extern   div_agc_s
       .extern   Inv_sqrt
@**********************************
@ constant
@**********************************
@**********************************
@ ARM register
@**********************************
@ *sig_in        RN             1
@ *sig_out       RN             2
@ l_trm          RN             4

@**********************************
@ Neon register
@**********************************
@qx0              .req             Q0.S16
@qx1              .req             Q1.S16
@qx2              .req             Q2.S16
@qx3              .req             Q3.S16
@qx4              .req             Q4.S16

@dx0              .req             D0.S16
@dx1              .req             D1.S16
@dx2              .req             D2.S16
@dx3              .req             D3.S16
@dx4              .req             D4.S16
@dx5              .req             D5.S16
@dx6              .req             D6.S16
@dx7              .req             D7.S16
@dx8              .req             D8.S16
@dx9              .req             D9.S16

@qy0              .req             Q5.S16
@qy1              .req             Q6.S16
@qy2              .req             Q7.S16
@qy3              .req             Q8.S16
@qy4              .req             Q9.S16

@dy0              .req             D10.S16
@dy1              .req             D11.S16
@dy2              .req             D12.S16
@dy3              .req             D13.S16
@dy4              .req             D14.S16
@dy5              .req             D15.S16
@dy6              .req             D16.S16
@dy7              .req             D17.S16
@dy8              .req             D18.S16
@dy9              .req             D19.S16


@sum              .req             Q10.S32
@sum_lo           .req             D20.S32
@sum_hi           .req             D21.S32
@temp             .req             D22.S32


agc2_asm:
         STMFD      r13!, {r0 - r12, r14}        
         MOV        r4, r0
         MOV        r5, r1
         @load all sig_in[] 
         VLD1.S16       {D0, D1, D2, D3}, [r4]! 
         VLD1.S16       {D4, D5, D6, D7}, [r4]!
         VLD1.S16       {D8, D9}, [r4]!
         
         VLD1.S16       {D10, D11, D12, D13}, [r5]! 
         VLD1.S16       {D14, D15, D16, D17}, [r5]!
         VLD1.S16       {D18, D19}, [r5]!
         VADD.S16       Q5, Q5, Q0
         VADD.S16       Q6, Q6, Q1 
         VADD.S16       Q7, Q7, Q2
         VADD.S16       Q8, Q8, Q3
         VADD.S16       Q9, Q9, Q4
     
         VQDMULL.S16    Q10, D10, D10
         VQDMLAL.S16    Q10, D11, D11
         VQDMLAL.S16    Q10, D12, D12
         VQDMLAL.S16    Q10, D13, D13
         VQDMLAL.S16    Q10, D14, D14
         VQDMLAL.S16    Q10, D15, D15
         VQDMLAL.S16    Q10, D16, D16
         VQDMLAL.S16    Q10, D17, D17
         VQDMLAL.S16    Q10, D18, D18
         VQDMLAL.S16    Q10, D19, D19
         MOV            r5, r1
         VQADD.S32      D20, D20, D21
         VEXT.32        D21, D20, D20, #1
         VQADD.S32      D20, D20, D21

         VST1.S16       {D10, D11, D12, D13}, [r5]!
         VST1.S16       {D14, D15, D16, D17}, [r5]!
         VST1.S16       {D18, D19}, [r5]!

         VMOV.S32       r6, D20[0] 
         VSHR.S16       Q5, Q5, #2
         VSHR.S16       Q6, Q6, #2
         VSHR.S16       Q7, Q7, #2
         VSHR.S16       Q8, Q8, #2
         VSHR.S16       Q9, Q9, #2
         
         CMP        r6, #0x7fffffff
         BNE        Label

         VQDMULL.S16    Q10, D10, D10
         VQDMLAL.S16    Q10, D11, D11
         VQDMLAL.S16    Q10, D12, D12
         VQDMLAL.S16    Q10, D13, D13
         VQDMLAL.S16    Q10, D14, D14
         VQDMLAL.S16    Q10, D15, D15
         VQDMLAL.S16    Q10, D16, D16
         VQDMLAL.S16    Q10, D17, D17
         VQDMLAL.S16    Q10, D18, D18
         VQDMLAL.S16    Q10, D19, D19
         VQADD.S32      D20, D20, D21
         VPADD.S32      D20, D20, D20
         VMOV.S32       r12, D20[0]
         B          Label1

Label:
         MOV        r12, r6, ASR #4
         
Label1:
         CMP        r12, #0
         BEQ        agc2_asm_end
         CLZ        r9, r12
         SUB        r7, r9, #2                  @ exp = (norm_l(s) - 1)
         MOV        r9, #0x8000
         MOV        r12, r12, LSL r7
         QADD       r10, r12, r9
         MOV        r10, r10, ASR #16           @ gain_out = round(s << exp)

         @ r10 --- gain_out
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
         VEXT.32    D21, D20, D20, #1
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

Label2:
         MOV            r12, r6, ASR #4
         
Label3:
         CMP            r12, #0
         MOVEQ          r4,  #0                     @ g0 = 0
         BEQ            Label4
         MOV            r8, r1                      @ copy sig_out[] address

         CLZ            r9, r12 
         SUB            r14, r9, #1                 @ norm_l(s)
         MOV            r9, #0x8000
         MOV            r12, r12, LSL r14
         QADD           r11, r12, r9
         MOV            r11, r11, ASR #16           @ gain_in = round(s << exp) 
         SUB            r7, r7, r14                 @ exp -=i

         MOV            r0, r10
         MOV            r1, r11
         BL             div_agc_s
         MOV            r12, r0                     @ s = div_s(gain_out, gain_in)  
         SSAT           r12, #31, r12, LSL #7       @ s = L_shl2(s, 7)
         CMP            r7, #0
         RSBLT          r7, r7, #0
         MOVGT          r12, r12, ASR r7
         MOVLT          r12, r12, LSL r7

         MOV            r0, r12
         BL             Inv_sqrt
         MOV            r12, r0                     @ s = Inv_sqrt(s)

         MOV            r9, #0x8000
         SSAT           r12, #31, r12, LSL #9       @L_shl2(s, 9)
         QADD           r10, r12, r9
         MOV            r4, r10, ASR #16            @ i = round(L_shl2(s, 9))
         @ r4 --- g0
         @ r5 --- r2 : sig_out[]
Label4: 
         @for(i=0@ i < l_trm@ i++)
         @gain = (gain * agc_fac)>>15
         @gain +=g0
         @sig_out[i] = extract_h(L_shl2(sig_out[i] * gain), 4)
         MOV            r5, r8                       @ sig_out[] address
         MOV            r8, #0

LOOP:
         LDRSH          r9, [r5]                     @sig_out[i]
         MUL            r10, r9, r4                  @sig_out[i] * g0
         SSAT           r11, #32, r10, LSL #4        @L_shl2(sig_out[i] * gain, 4)
         MOV            r11, r11, ASR #16
         ADD            r8, #1
         STRH           r11, [r5], #2
         CMP            r8, #40
         BLT            LOOP

agc2_asm_end:
 
         LDMFD          r13!, {r0 - r12, r15}
         .END
