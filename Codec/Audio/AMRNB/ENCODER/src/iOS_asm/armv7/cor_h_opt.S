@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVEd.
@****************************************************************
@void cor_h (
@    Word16 h[],         /* (i) : impulse response of weighted synthesis
@                                 filter                                  */
@    Word16 sign[],      /* (i) : sign of d[n]                            */
@    Word16 rr[][L_COdE] /* (o) : matrix of autocorrelation               */
@)

@**************************************
@ constants parameter
@**************************************
.set  SIZEOF_s16           , 2
.set  LOG_SIZEOF_s16       , 1
.set  SIZEOF_s32           , 4
.set  LOG_SIZEOF_s32       , 2
.set  L_COdE               , 40
.set  H2_BUFFER_SIZE       , 80 @ L_COdE * SIZEOF_s16
.set  H2_BUFFER_OFS        , 0 @ Offset from stack pointer to h2 buffer

          #include "voAMRNBEncID.h"
         .text    .align   4
         .globl   _cor_h_asm
         .globl  _Inv_sqrt1

@ Constants
init_const:
         .word              32440, 32440, 32440, 32440            @ const32440: 0.99 rsetired by scaling h[] to get h2[].
         .word              0x7ffe, 0x7ffe, 0x7ffe, 0x7ffe        @ d15: mult (32767, 32767) = mult(-32767, -32767) = 32766      
         .word              0x8001, 0x8001, 0x8001, 0x8001        @ d16: mult (32767, -32767) = mult(-32767, 32767) =-32767
         .word              0, 0, 0, 0xffff                       @ d17.u16: ensure signs are not applied along the diagonal
         .word              1, 1, 1, 1                            @ d18.u16: current sign bit to inspect

_cor_h_asm:

         stmfd            sp!, {r4-r12, lr}
@ Stack registers and allocate h2 array on the stack 
         sub              sp, sp, #H2_BUFFER_SIZE
@ Offset to Bottom Right Corner, rr[L_COdE-1][L_COdE-1])
@ ------------------------------------------------------
         add              r2, r2, #3200                            @ L_COdE * L_COdE * SIZEOF_s16   
         sub              r2, r2, #SIZEOF_s16
@ Load constants
@ --------------
         adr              lr, init_const                                   @ (tc workaround) : use vldR
         vld1.s16         {d14, d15, d16}, [lr]!   
         vld1.u16         {d17, d18}, [lr]!
        
@ Scaling for maximum precision
@ -----------------------------
@ for (i = 0@ i < L_COdE@ i ++)
@ s = L_mac(q12, h[i], h[i])@

         vld1.s16         {d0, d1, d2, d3}, [r0]!
         vld1.s16         {d4, d5, d6, d7}, [r0]!
         vld1.s16         {d8, d9}, [r0]!
        
         vqdmull.s16       q12, d0, d0
         vqdmlal.s16       q12, d1, d1
         vqdmlal.s16       q12, d2, d2
         vqdmlal.s16       q12, d3, d3
         vqdmlal.s16       q12, d4, d4
         vqdmlal.s16       q12, d5, d5
         vqdmlal.s16       q12, d6, d6
         vqdmlal.s16       q12, d7, d7
         vqdmlal.s16       q12, d8, d8
         vqdmlal.s16       q12, d9, d9

@ sum across the SIMd lanes to get the final result.

         vqadd.s32         d24, d24, d25
         vext.s32          d20, d24, d24, #1                           @replace hi and lo data
         vqadd.s32         d24, d24, d20

         vmov.s32          lr, d24[0]
         mov               r4, #2
         qadd              lr, lr, r4                                  @s = 2 add two to accumulator
         mov               r4, lr, asr #16                             @extract_h(s)
         sub               r4, r4, #0x7f00
         cmp               r4, #0xff
         beq               case_saturating                             @j ==0 branch
@ Call inverse square root function.
         stmfd             sp!, {r0 - r3}
         mov               r0, lr, asr #1                              @ s = L_shr(q12,1)
         bl                _Inv_sqrt1
         vdup.s32          q12, r0
         ldmfd             sp!, {r0 - r3}

         vqshl.s32         q12, q12, #7                                  @ L_shl(Inv_sqrt(s), 7)
         vshrn.s32         d20, q12, #16                                 @ k = extract_h(L_shl(Inv_sqrt(s), 7))
         vmull.s16         q12, d14, d20                                 @ k= mult(k, 32440)        
         vshrn.s32         d20, q12, #15
                
         vqdmull.s16       q12, d0, d20                                    @ d20 = k                
         vqrshrn.s32       d0,  q12, #7
         vqdmull.s16       q12, d1, d20
         vqrshrn.s32       d1,  q12, #7
         vqdmull.s16       q12, d2, d20
         vqrshrn.s32       d2,  q12, #7
         vqdmull.s16       q12, d3, d20
         vqrshrn.s32       d3,  q12, #7
         vqdmull.s16       q12, d4, d20
         vqrshrn.s32 	   d4,  q12, #7
         vqdmull.s16 	   q12, d5, d20
         vqrshrn.s32 	   d5,  q12, #7
         vqdmull.s16 	   q12, d6, d20
         vqrshrn.s32       d6,  q12, #7
         vqdmull.s16       q12, d7, d20
         vqrshrn.s32       d7,  q12, #7
         vqdmull.s16       q12, d8, d20
         vqrshrn.s32       d8,  q12, #7
         vqdmull.s16       q12, d9, d20
         vqrshrn.s32       d9,  q12, #7
         b                 build_rr_matrix
@--------------------------------------------------------
case_saturating:
@ Scale input array right one place.
@      for(i=0@ i<L_COdE@ i++)
@      {
@         h2[i] = shr(h[i], 1)@
@       }
         vshr.s16           q0, q0, #1
         vshr.s16           q1, q1, #1
         vshr.s16           q2, q2, #1
         vshr.s16           q3, q3, #1
         vshr.s16           q4, q4, #1 

build_rr_matrix:

@ Reverse h2 array, because we shall be running through it backwards.

         vrev64.s16         d0, d0
         vrev64.s16         d1, d1 
         vrev64.s16         d2, d2
         vrev64.s16         d3, d3
         vrev64.s16         d4, d4
         vrev64.s16         d5, d5
         vrev64.s16         d6, d6
         vrev64.s16         d7, d7
         vrev64.s16         d8, d8
         vrev64.s16         d9, d9
       
@ Store the scaled array h2[]
         mov                 r7, sp                            @get the H2 buffer first address     
         vst1.s16           {d0, d1, d2, d3}, [r7]!           @(tc workaround) this should be a normal STR [sp,#H2_BUFFER_OFS]     
         vst1.s16           {d4, d5, d6, d7}, [r7]!      
         vst1.s16           {d8, d9}, [r7]
       
         vmov.u16           d4, #0
         mov                ip, #10
          
loop_signs:
         vld1.s16            d0, [r1]!          
         vshr.u16.u16        d0, d0, #15                                                     
         vshl.u16            d4, d4, #1
         vorr.u16            d4, d4, d0                             
         subs                ip, ip, #1
         bne                 loop_signs
          
         vshr.u16            d20, d4, #1                               
         vext.u16            d5, d20, d4, #3
         vext.u16            d6, d20, d4, #2
         vext.u16            d7, d20, d4, #1
          

         vdup.u16            d8,   d4[3]
         vdup.u16            d9,   d4[2]
         vdup.u16            d10,  d4[1]
         vdup.u16            d11,  d4[0]

         veor.u16            d8,  d8,  d4
         veor.u16     	     d9,  d9,  d5
         veor.u16 	     d10, d10, d6
         veor.u16 	     d11, d11, d7
         add 		     r8, sp, #H2_BUFFER_OFS
         mvn 		     r6,  #81                                             @ -(L_COdE + 1) * SIZEOF_s16
         mov                 r5,  #9
         vmov.s32            d24,   #0
	 vmov.s32            d25,   #0
         sub                 r9, r2, #6                                      @ 3 * SIZEOF_s16
	 vld1.s16            d0, [r8]!
	     
loop_first:

	 vld1.s16            d1, [r8]!	      
@ Update correlation 
	 vqdmlal.s16         q12, d0, d0[3]
	 vqrshrn.s32         d20, q12, #16
@ Check sign bit for each SIMd lane
	 vtst                d22.s16, d8.u16, d18.u16	      
@ Multiply by +32766 or -32767, depending on sign
	 vbsl.s16            d22, d16, d15
	 vqdmulh.s16         d22, d20, d22
	    
@ Along the diagonal, do not multiply by the product of the sign bits.
	 vbif                d20.s16, d22.s16, d17.u16
@ Store four results
	 vst1.s16            d20, [r9], r6
	 vext.s16            d2, d1, d0, #3
	 vqdmlal.s16         q12, d2, d0[2]
	 vqrshrn.s32         d20, q12, #16
	 vtst.s16            d22, d9, d18
	 vbsl.s16            d22, d16, d15
	 vqdmulh.s16         d22, d20, d22
	 vbif.s16            d20, d22, d17
	 vst1.s16            d20, [r9], r6
	 vext.s16            d2, d1, d0, #2
	 vqdmlal.s16         q12, d2, d0[1]
         vqrshrn.s32         d20, q12, #16
	 vtst.s16            d22, d10, d18
	 vbsl.s16            d22, d16, d15
	 vqdmulh.s16         d22, d20, d22
	 vbif.s16            d20, d22, d17
	 vst1.s16            d20, [r9], r6
	 vext.s16            d2,  d1, d0, #1
	 vqdmlal.s16         q12, d2, d0[0]
	 vqrshrn.s32         d20, q12, #16
	 vtst.s16            d22, d11, d18
	 vbsl.s16            d22, d16, d15
	 vqdmulh.s16         d22, d20, d22
	 vbif.s16            d20, d22, d17
	 vst1.s16            d20, [r9], r6
	 vshl.u16            d18, d18, #1
	 vmov.s16            d0, d1
	 subs                r5, r5, #1
	 bne                 loop_first
	     
@ Fixup at end of loop
@ --------------------
	 vld1.s16            d1, [r8]!
	 vqdmlal.s16         q12, d0, d0[3]
	 vqrshrn.s32         d20, q12, #16
	 
	 vtst                d22.s16, d8.u16, d18.u16
	 vbsl.s16            d22, d16, d15
	 vqdmulh.s16         d22, d20, d22
	 vbif.s16            d20, d22, d17.u16
	 vst1.s16            d20, [r9], r6
	 vext.16              d2, d1, d0, #3
	 vqdmlal.s16          q12, d2, d0[2]
	 vqrshrn.s32          d20, q12, #16
	 vtst                 d22.s16, d9.u16, d18.u16
	 vbsl.s16             d22, d16, d15
	 vqdmulh.s16          d22, d20, d22
	 vbif                 d20.s16, d22.s16, d17.u16
	 add                  r7, r9, #2 @ 1 * SIZEOF_s16
	 vst1.s16             d20[1], [r7]! @ use limited store
	 vst1.s16             d20[2], [r7]!
	 vst1.s16             d20[3], [r7]!
	 add                  r9, r9, r6
	 vext.16              d2, d1, d0, #2
	 vqdmlal.s16          q12, d2, d0[1]
	 vqrshrn.s32          d20, q12, #16
	 vtst                 d22.s16, d10.u16, d18.u16
	 vbsl.s16             d22, d16, d15
	 vqdmulh.s16          d22, d20, d22
	 vbif                 d20.s16, d22.s16, d17.u16
	 add                  r7, r9, #4 @ 2 * SIZEOF_s16
	 vst1.s16             d20[2], [r7]! @ use limited store
	 vst1.s16             d20[3], [r7]!
	 add                  r9, r9, r6
	 vext.16              d2, d1, d0, #1
	 vqdmlal.s16          q12, d2, d0[0]
	 vqrshrn.s32          d20, q12, #16
	 add                  r7, r9, #6 @ 3 * SIZEOF_s16
	 vst1.s16             d20[3], [r7], r6
	     
@ Finish matrix off diagonal
    	 mov                  r1, #1

outer_loop:
	 sub                  r2, r2, #8
	 rsb                  r7, r1, #0 @ negate to shift right by variable amount
	 vdup.s16             d22, r7
	 add                  r7, sp, #H2_BUFFER_OFS
	 add                  r8, r7, r1, LSL #3 @ = r1 * 4 * SIZEOF_s16
	 vdup.u16             d8, d4[3]              @ get ssetence of signbit[j]
	 
	 vshl.u16             d20, d4, d22         @ calculate ssetence of signbit[i]
	 veor.u16             d8, d8, d20           @ exclusive-or to multiply signs.
	 vdup.u16             d9, d4[2]
	 vshl.u16             d20, d5, d22
	 veor.u16             d9, d9, d20
	 vdup.u16             d10, d4[1]
	 vshl.u16             d20, d6, d22
	 veor.u16             d10, d10, d20
	 vdup.u16             d11, d4[0]
	 vshl.u16             d20, d7, d22
	 
	 veor                 d11.u16, d11.u16, d20.u16
	 vmov                 d18.u16, #0x0001
	 vmov.s32             d24, #0
	 vmov.s32             d25, #0
	 sub                  r9, r2, #6 @ 3 * SIZEOF_s16
	 vld1.s16             d0, [r8]!
	 rsb                  r5, r1, #10
	 subs                 r5, r5, #1
	 beq                  fixup_end_loop 
	 
loop_main:

	 vld1.s16             d3, [r7]!
	 vld1.s16             d1, [r8]!
@ Update correlation
	 vqdmlal.s16          q12, d0, d3[3]
	 vqrshrn.s32          d20, q12, #16
@ Check signbit for each SIMd lane
	 vtst                 d22.s16, d8.u16, d18.u16
@ Multiply by +32766 or -32767, depending on sign
	 vbsl.s16             d22, d16, d15
	 vqdmulh.s16          d20, d20, d22
@ Store four results

	 vst1.s16             d20, [r9], r6
	 vext.s16             d2, d1, d0, #3
	 vqdmlal.s16          q12, d2, d3[2]
	 vqrshrn.s32          d20, q12, #16
	 vtst.s16             d22, d9, d18
	 vbsl.s16             d22, d16, d15
	 vqdmulh.s16          d20, d20, d22
	 vst1.s16             d20, [r9], r6
	 vext.s16             d2, d1, d0, #2
	 vqdmlal.s16          q12, d2, d3[1]
	 vqrshrn.s32          d20, q12, #16
	 vtst.s16             d22, d10, d18
	 vbsl.s16             d22, d16, d15
	 vqdmulh.s16          d20, d20, d22
	 vst1.s16             d20, [r9], r6
	 vext.s16             d2, d1, d0, #1
	 vqdmlal.s16          q12, d2, d3[0]
	 vqrshrn.s32          d20, q12, #16
	 vtst.s16             d22, d11, d18
	 vbsl.s16             d22, d16, d15
	 vqdmulh.s16          d20, d20, d22
	 vst1.s16             d20, [r9], r6
	 vshl                 d18.u16, d18.u16, #1
	 vmov.s16             d0, d1
	 subs                 r5, r5, #1	 
	 bne                  loop_main
	     	     
fixup_end_loop:

	 vld1.s16             d3, [r7]!
	 vld1.s16             d1, [r8]!
	 vqdmlal.s16          q12, d0, d3[3]
	 vrshrn.s32           d20, q12, #16
	 vtst                 d22.s16, d8.u16, d18.u16
	 vbsl.s16             d22, d16, d15
	 vqdmulh.s16          d20, d20, d22
	 vst1.s16             d20, [r9], r6
	 vext.16              d2, d1, d0, #3
	 vqdmlal.s16          q12, d2, d3[2]
	 vqrshrn.s32          d20, q12, #16
	 vtst                 d22.s16, d9.u16, d18.u16
	 vbsl.s16             d22, d16, d15
	 vqdmulh.s16          d20, d20, d22
	 add                  r7, r9, #2 @ 1 * SIZEOF_s16
	 vst1.s16             d20[1], [r7]! @ use limited store
	 vst1.s16             d20[2], [r7]!
	 vst1.s16             d20[3], [r7]!
	 add                  r9, r9, r6
	 vext.16              d2, d1, d0, #2
	 vqdmlal.s16          q12, d2, d3[1]
	 vqrshrn.s32          d20, q12, #16
	 vtst                 d22.s16, d10.u16, d18.u16
	 vbsl.s16             d22, d16, d15
	 vqdmulh.s16          d20, d20, d22
	 add                  r7, r9, #4 @ 2 * SIZEOF_s16
	 vst1.s16             d20[2], [r7]! @ use limited store
	 vst1.s16             d20[3], [r7]!
	 add                  r9, r9, r6
	 vext.16              d2, d1, d0, #1
	 vqdmlal.s16          q12, d2, d3[0]
	 vqrshrn.s32          d20, q12, #16
	 add                  r7, r9, #6 @ 3 * SIZEOF_s16
	 vtst                 d22.s16, d11.u16, d18.u16
	 vbsl.s16             d22, d16, d15
	 vqdmulh.s16          d20, d20, d22
	 vst1.s16             d20[3], [r7], r6
	 add                  r1, r1, #1
	 cmp                  r1, #10
	 
	 bne                  outer_loop

         add                  sp, sp, #H2_BUFFER_SIZE
         ldmfd                sp!, {r4-r12, pc} 
         @ENdP
         @.ENd  


