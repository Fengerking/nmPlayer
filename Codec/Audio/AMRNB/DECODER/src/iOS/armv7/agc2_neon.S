@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@****************************************************************

@void agc2 (
@ Word16 *sig_in,        /* i   : postfilter input signal  */
@ Word16 *sig_out,       /* i/o : postfilter output signal */
@ Word16 l_trm           /* i   : subframe size            */
@)
       #include "voAMRNBDecID.h"
       .text    
       .align 4
       .globl   _agc2_asm
       .globl   _div_agc_s
       .globl   _Inv_sqrt
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
@qx0              .req             q0.S16
@qx1              .req             q1.S16
@qx2              .req             q2.S16
@qx3              .req             q3.S16
@qx4              .req             q4.S16

@dx0              .req             d0.S16
@dx1              .req             d1.S16
@dx2              .req             d2.S16
@dx3              .req             d3.S16
@dx4              .req             d4.S16
@dx5              .req             d5.S16
@dx6              .req             d6.S16
@dx7              .req             d7.S16
@dx8              .req             d8.S16
@dx9              .req             d9.S16

@qy0              .req             q5.S16
@qy1              .req             q6.S16
@qy2              .req             q7.S16
@qy3              .req             q8.S16
@qy4              .req             q9.S16

@dy0              .req             d10.S16
@dy1              .req             d11.S16
@dy2              .req             d12.S16
@dy3              .req             d13.S16
@dy4              .req             d14.S16
@dy5              .req             d15.S16
@dy6              .req             d16.S16
@dy7              .req             d17.S16
@dy8              .req             d18.S16
@dy9              .req             d19.S16


@sum              .req             q10.S32
@sum_lo           .req             d20.S32
@sum_hi           .req             d21.S32
@temp             .req             d22.S32


_agc2_asm:
         stmfd      r13!, {r0 - r12, r14}        
         mov        r4, r0
         mov        r5, r1
         @load all sig_in[] 
         vld1.S16       {d0, d1, d2, d3}, [r4]! 
         vld1.S16       {d4, d5, d6, d7}, [r4]!
         vld1.S16       {d8, d9}, [r4]!
         
         vld1.S16       {d10, d11, d12, d13}, [r5]! 
         vld1.S16       {d14, d15, d16, d17}, [r5]!
         vld1.S16       {d18, d19}, [r5]!
         vadd.S16       q5, q5, q0
         vadd.S16       q6, q6, q1 
         vadd.S16       q7, q7, q2
         vadd.S16       q8, q8, q3
         vadd.S16       q9, q9, q4
     
         vqdmull.S16    q10, d10, d10
         vqdmlal.S16    q10, d11, d11
         vqdmlal.S16    q10, d12, d12
         vqdmlal.S16    q10, d13, d13
         vqdmlal.S16    q10, d14, d14
         vqdmlal.S16    q10, d15, d15
         vqdmlal.S16    q10, d16, d16
         vqdmlal.S16    q10, d17, d17
         vqdmlal.S16    q10, d18, d18
         vqdmlal.S16    q10, d19, d19
         mov            r5, r1
         vqadd.S32      d20, d20, d21
         vext.32        d21, d20, d20, #1
         vqadd.S32      d20, d20, d21

         vst1.S16       {d10, d11, d12, d13}, [r5]!
         vst1.S16       {d14, d15, d16, d17}, [r5]!
         vst1.S16       {d18, d19}, [r5]!

         vmov.S32       r6, d20[0] 
         vshr.S16       q5, q5, #2
         vshr.S16       q6, q6, #2
         vshr.S16       q7, q7, #2
         vshr.S16       q8, q8, #2
         vshr.S16       q9, q9, #2
         
         cmp            r6, #0x7fffffff
         bne            Label

         vqdmull.S16    q10, d10, d10
         vqdmlal.S16    q10, d11, d11
         vqdmlal.S16    q10, d12, d12
         vqdmlal.S16    q10, d13, d13
         vqdmlal.S16    q10, d14, d14
         vqdmlal.S16    q10, d15, d15
         vqdmlal.S16    q10, d16, d16
         vqdmlal.S16    q10, d17, d17
         vqdmlal.S16    q10, d18, d18
         vqdmlal.S16    q10, d19, d19
         vqadd.S32      d20, d20, d21
         vpadd.S32      d20, d20, d20
         vmov.S32       r12, d20[0]
         b              Label1

Label:
         mov        r12, r6, asr #4
         
Label1:
         cmp        r12, #0
         beq        agc2_asm_end
         clz        r9, r12
         sub        r7, r9, #2                  @ exp = (norm_l(s) - 1)
         mov        r9, #0x8000
         mov        r12, r12, lsl r7
         qadd       r10, r12, r9
         mov        r10, r10, asr #16           @ gain_out = round(s << exp)

         @ r10 --- gain_out
         vqdmull.S16    q10, d0, d0
         vqdmlal.S16    q10, d1, d1
         vqdmlal.S16    q10, d2, d2
         vqdmlal.S16    q10, d3, d3
         vqdmlal.S16    q10, d4, d4
         vqdmlal.S16    q10, d5, d5
         vqdmlal.S16    q10, d6, d6
         vqdmlal.S16    q10, d7, d7
         vqdmlal.S16    q10, d8, d8
         vqdmlal.S16    q10, d9, d9

         vqadd.S32      d20, d20, d21
         vext.32    d21, d20, d20, #1
         vqadd.S32      d20, d20, d21
         vmov.S32       r6, d20[0] 

         vshr.S16       q0, q0, #2
         vshr.S16       q1, q1, #2
         vshr.S16       q2, q2, #2
         vshr.S16       q3, q3, #2
         vshr.S16       q4, q4, #2
         
         cmp            r6, #0x7fffffff
         bne            Label2

 
         vqdmull.S16    q10, d0, d0
         vqdmlal.S16    q10, d1, d1
         vqdmlal.S16    q10, d2, d2
         vqdmlal.S16    q10, d3, d3
         vqdmlal.S16    q10, d4, d4
         vqdmlal.S16    q10, d5, d5
         vqdmlal.S16    q10, d6, d6
         vqdmlal.S16    q10, d7, d7
         vqdmlal.S16    q10, d8, d8
         vqdmlal.S16    q10, d9, d9
         vqadd.S32      d20, d20, d21
         vpadd.S32      d20, d20, d20
         vmov.S32       r12, d20[0]
         b              Label3

Label2:
         mov            r12, r6, asr #4
         
Label3:
         cmp            r12, #0
         moveq          r4,  #0                     @ g0 = 0
         beq            Label4
         mov            r8, r1                      @ copy sig_out[] address

         clz            r9, r12 
         sub            r14, r9, #1                 @ norm_l(s)
         mov            r9, #0x8000
         mov            r12, r12, lsl r14
         qadd           r11, r12, r9
         mov            r11, r11, asr #16           @ gain_in = round(s << exp) 
         sub            r7, r7, r14                 @ exp -=i

         mov            r0, r10
         mov            r1, r11
         bl             _div_agc_s
         mov            r12, r0                     @ s = div_s(gain_out, gain_in)  
         ssat           r12, #31, r12, lsl #7       @ s = L_shl2(s, 7)
         cmp            r7, #0
         rsblt          r7, r7, #0
         movgt          r12, r12, asr r7
         movlt          r12, r12, lsl r7

         mov            r0, r12
         bl             _Inv_sqrt
         mov            r12, r0                     @ s = Inv_sqrt(s)

         mov            r9, #0x8000
         ssat           r12, #31, r12, lsl #9       @L_shl2(s, 9)
         qadd           r10, r12, r9
         mov            r4, r10, asr #16            @ i = round(L_shl2(s, 9))
         @ r4 --- g0
         @ r5 --- r2 : sig_out[]
Label4: 
         @for(i=0@ i < l_trm@ i++)
         @gain = (gain * agc_fac)>>15
         @gain +=g0
         @sig_out[i] = extract_h(L_shl2(sig_out[i] * gain), 4)
         mov            r5, r8                       @ sig_out[] address
         mov            r8, #0

LOOP:
         ldrsh          r9, [r5]                     @sig_out[i]
         mul            r10, r9, r4                  @sig_out[i] * g0
         ssat           r11, #32, r10, lsl #4        @L_shl2(sig_out[i] * gain, 4)
         mov            r11, r11, asr #16
         add            r8, #1
         strh           r11, [r5], #2
         cmp            r8, #40
         blt            LOOP

agc2_asm_end:
 
         ldmfd          r13!, {r0 - r12, r15}
         @.ENd
