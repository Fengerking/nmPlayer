@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@****************************************************************

@int agc (
@    agcState *st,      /* i/o : agc state                        */
@    Word16 *sig_in,    /* i   : postfilter input signal  (l_trm) */
@    Word16 *sig_out,   /* i/o : postfilter output signal (l_trm) */
@    Word16 agc_fac,    /* i   : AGC factor                       */
@    Word16 l_trm       /* i   : subframe size                    */
@)
       #include "voAMRNBDecID.h"
       .text    
       .align 4
       .globl   _agc_asm
       .globl   _div_agc_s
       .globl   _Inv_sqrt

@**********************************
@ constant
@**********************************
@**********************************
@ ARM register
@**********************************
@ *st            RN             0
@ *sig_in        RN             1
@ *sig_out       RN             2
@ agc_fac        RN             3
@ l_trm          RN             4

@**********************************
@ Neon register
@**********************************
@qx0              .req             q0.s16
@qx1              .req             q1.s16
@qx2              .req             q2.s16
@qx3              .req             q3.s16
@qx4              .req             q4.s16

@dx0              .req             d0.s16
@dx1              .req             d1.s16
@dx2              .req             d2.s16
@dx3              .req             d3.s16
@dx4              .req             d4.s16
@dx5              .req             d5.s16
@dx6              .req             d6.s16
@dx7              .req             d7.s16
@dx8              .req             d8.s16
@dx9              .req             d9.s16


@sum              .req             q10.s32
@sum_lo           .req             d20.s32
@sum_hi           .req             d21.s32
@temp             .req             d22.s32

agc_asm:

         stmfd          r13!, {r0 - r12, r14}     
         mov            r4, r1
         mov            r5, r2
         @vmov          d22, #0x7ffffff
         @load all 
         vld1.s16       {d0, d1, d2, d3}, [r5]! 
         vld1.s16       {d4, d5, d6, d7}, [r5]!
         vld1.s16       {d8, d9}, [r5]!
         mov            r8, #0                          @ r8 = 0
         vqdmull.s16    q10, d0, d0
         vqdmlal.s16    q10, d1, d1
         vqdmlal.s16    q10, d2, d2
         vqdmlal.s16    q10, d3, d3
         vqdmlal.s16    q10, d4, d4
         vqdmlal.s16    q10, d5, d5
         vqdmlal.s16    q10, d6, d6
         vqdmlal.s16    q10, d7, d7
         vqdmlal.s16    q10, d8, d8
         vqdmlal.s16    q10, d9, d9

         vqadd.s32      d20, d20, d21
         vext.32        d21, d20, d20, #1
         vqadd.s32      d20, d20, d21
         vmov.s32       r6, d20[0] 
         
         vshr.s16       q0, q0, #2
         vshr.s16       q1, q1, #2
         vshr.s16       q2, q2, #2
         vshr.s16       q3, q3, #2
         vshr.s16       q4, q4, #2
         
         cmp            r6, #0x7fffffff
         bne            Label

         vqdmull.s16    q10, d0, d0
         vqdmlal.s16    q10, d1, d1
         vqdmlal.s16    q10, d2, d2
         vqdmlal.s16    q10, d3, d3
         vqdmlal.s16    q10, d4, d4
         vqdmlal.s16    q10, d5, d5
         vqdmlal.s16    q10, d6, d6
         vqdmlal.s16    q10, d7, d7
         vqdmlal.s16    q10, d8, d8
         vqdmlal.s16    q10, d9, d9
         vqadd.s32      d20, d20, d21
         vpadd.s32      d20, d20, d20
         vmov.s32       r12, d20[0]
         b              Label1

Label:
         mov            r12, r6, asr #4
         
Label1:
         cmp            r12, #0
         beq            Label5
         clz            r9, r12
         sub            r7, r9, #2                  @ exp = (norm_l(s) - 1)
         mov            r9, #0x8000
         mov            r12, r12, LSL r7
         qadd           r10, r12, r9
         mov            r10, r10, asr #16           @ gain_out = round(s << exp)

         @ r10 --- gain_out
         vld1.s16       {d0, d1, d2, d3}, [r4]! 
         vld1.s16       {d4, d5, d6, d7}, [r4]!
         vld1.s16       {d8, d9}, [r4]!
         vqdmull.s16    q10, d0, d0
         vqdmlal.s16    q10, d1, d1
         vqdmlal.s16    q10, d2, d2
         vqdmlal.s16    q10, d3, d3
         vqdmlal.s16    q10, d4, d4
         vqdmlal.s16    q10, d5, d5
         vqdmlal.s16    q10, d6, d6
         vqdmlal.s16    q10, d7, d7
         vqdmlal.s16    q10, d8, d8
         vqdmlal.s16    q10, d9, d9

         vqadd.s32      d20, d20, d21
         vext.32        d21, d20, d20, #1
         vqadd.s32      d20, d20, d21
         vmov.s32       r6, d20[0] 

         vshr.s16       q0, q0, #2
         vshr.s16       q1, q1, #2
         vshr.s16       q2, q2, #2
         vshr.s16       q3, q3, #2
         vshr.s16       q4, q4, #2
         
         cmp            r6, #0x7fffffff
         bne            Label2

 
         vqdmull.s16    q10, d0, d0
         vqdmlal.s16    q10, d1, d1
         vqdmlal.s16    q10, d2, d2
         vqdmlal.s16    q10, d3, d3
         vqdmlal.s16    q10, d4, d4
         vqdmlal.s16    q10, d5, d5
         vqdmlal.s16    q10, d6, d6
         vqdmlal.s16    q10, d7, d7
         vqdmlal.s16    q10, d8, d8
         vqdmlal.s16    q10, d9, d9
         vqadd.s32      d20, d20, d21
         vpadd.s32      d20, d20, d20
         vmov.s32       r12, d20[0]
         b              Label3

Label2:
         mov            r12, r6, asr #4
         
Label3:
         cmp            r12, #0
         moveq          r4,  #0                     @ g0 = 0
         beq            Label4
         mov            r5, r3
         mov            r4, r2
         mov            r8, r1
         clz            r9, r12 
         sub            r14, r9, #1                 @ norm_l(s)
         mov            r9, #0x8000
         mov            r12, r12, LSL r14
         qadd           r11, r12, r9
         mov            r11, r11, asr #16           @ gain_in = round(s << exp) 
         sub            r7, r7, r14                 @ exp -=i
         mov            r6, r0
         mov            r0, r10
         mov            r1, r11
         bl             div_agc_s
         mov            r12, r0                     @ s = div_s(gain_out, gain_in)  
         ssat           r12, #31, r12, LSL #7       @ s = L_shl2(s, 7)
         cmp            r7, #0
         rsblt          r7, r7, #0
         movgt          r12, r12, asr r7
         movlt          r12, r12, LSL r7

         mov            r0, r12
         bl             _Inv_sqrt
         mov            r12, r0                     @ s = Inv_sqrt(s)
         mov            r3, r5
         mov            r1, r8
        
         ldr            r8, TabLabl
       
         ;mov           r8, r9                      @ get 32767

         ssat           r12, #31, r12, LSL #9
         mov            r9, #0x8000
         qadd           r10, r12, r9
         mov            r10, r10, asr #16           @ i = round(L_shl2(s, 9))
         mov            r2, r4 
         sub            r9, r8, r3                  @ 32767 - agc_fac
         mul            r11, r9, r10
         ssat           r4, #16, r11, asr #15 
         @ r4 --- g0
         @ r5 --- r2 : sig_out[]
         mov            r0, r6                       @ get st address copy
Label4: 
         ldrsh          r7, [r0]                     @ gain = st->past_gain
         @for(i=0@ i < l_trm@ i++)
         @gain = (gain * agc_fac)>>15
         @gain +=g0
         @sig_out[i] = extract_h(L_shl2(sig_out[i] * gain), 4)
         mov            r8, #0
         mov            r5, r2                       @ sig_out[] address
LOOP:
         ldrsh          r9, [r5]                     @sig_out[i]
         mul            r10, r7, r3
         mov            r7, r10, asr #15             @gain = (gain * agc_fac)>>15
         add            r7, r7, r4                   @gain += g0
         mul            r10, r9, r7                  @sig_out[i] * gain
         ssat           r11, #32, r10, LSL #4        @L_shl2(sig_out[i] * gain, 4)
         mov            r11, r11, asr #16
         add            r8, #1
         strh           r11, [r5], #2
         cmp            r8, #40
         blt            LOOP
         strh           r7, [r0]
         b              agc_asm_end

Label5:
         strh           r8, [r0]                    @st->past_gain

agc_asm_end: 
         ldmfd          r13!, {r0 - r12, r15}

TabLabl:
         .word          0x7fff
         @.ENd
