;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;void cor_h (
;    Word16 h[],         /* (i) : impulse response of weighted synthesis
;                                 filter                                  */
;    Word16 sign[],      /* (i) : sign of d[n]                            */
;    Word16 rr[][L_CODE] /* (o) : matrix of autocorrelation               */
;)

;**************************************
; constants parameter
;**************************************
SIZEOF_S16           EQU           2
LOG_SIZEOF_S16       EQU           1
SIZEOF_S32           EQU           4
LOG_SIZEOF_S32       EQU           2
L_CODE               EQU           40
H2_BUFFER_SIZE       EQU           80 ; L_CODE * SIZEOF_S16
H2_BUFFER_OFS        EQU           0 ; Offset from stack pointer to h2 buffer

         AREA    |.text|, CODE, READONLY
         EXPORT cor_h_asm
         IMPORT Inv_sqrt1

; Constants
init_const
         DCW              32440, 32440, 32440, 32440            ; const32440: 0.99 required by scaling h[] to get h2[].
         DCW              0x7ffe, 0x7ffe, 0x7ffe, 0x7ffe        ; D15: mult (32767, 32767) = mult(-32767, -32767) = 32766      
         DCW              0x8001, 0x8001, 0x8001, 0x8001        ; D16: mult (32767, -32767) = mult(-32767, 32767) =-32767
         DCW              0, 0, 0, 0xffff                       ; D17.U16: ensure signs are not applied along the diagonal
         DCW              1, 1, 1, 1                            ; D18.U16: current sign bit to inspect

cor_h_asm PROC

         STMFD            sp!, {r4-r12, lr}
; Stack registers and allocate h2 array on the stack 
         SUB              sp, sp, #H2_BUFFER_SIZE
; Offset to Bottom Right Corner, rr[L_CODE-1][L_CODE-1])
; ------------------------------------------------------
         ADD              r2, r2, #3200                            ; L_CODE * L_CODE * SIZEOF_S16   
         SUB              r2, r2, #SIZEOF_S16
; Load constants
; --------------
         ADR              lr, init_const                                   ; (tc workaround) : use VLDR
         VLD1.S16         {D14, D15, D16}, [lr]!   
         VLD1.U16         {D17, D18}, [lr]!
        
; Scaling for maximum precision
; -----------------------------
; for (i = 0; i < L_CODE; i ++)
; s = L_mac(Q12, h[i], h[i]);

         VLD1.S16         {D0, D1, D2, D3}, [r0]!
         VLD1.S16         {D4, D5, D6, D7}, [r0]!
         VLD1.S16         {D8, D9}, [r0]!
        
         VQDMULL.S16       Q12, D0, D0
         VQDMLAL.S16       Q12, D1, D1
         VQDMLAL.S16       Q12, D2, D2
         VQDMLAL.S16       Q12, D3, D3
         VQDMLAL.S16       Q12, D4, D4
         VQDMLAL.S16       Q12, D5, D5
         VQDMLAL.S16       Q12, D6, D6
         VQDMLAL.S16       Q12, D7, D7
         VQDMLAL.S16       Q12, D8, D8
         VQDMLAL.S16       Q12, D9, D9

; sum across the SIMD lanes to get the final result.

         VQADD.S32         D24, D24, D25
         VEXT.S32          D20, D24, D24, #1                           ;replace hi and lo data
         VQADD.S32         D24, D24, D20

         VMOV.S32          lr, D24[0]
         MOV               r4, #2
         QADD              lr, lr, r4                                  ;s = 2 add two to accumulator
         MOV               r4, lr, ASR #16                             ;extract_h(s)
         SUB               r4, r4, #0x7f00
         CMP               r4, #0xff
         BEQ               case_saturating                             ;j ==0 branch
; Call inverse square root function.
         STMFD             sp!, {r0 - r3}
         MOV               r0, lr, ASR #1                              ; s = L_shr(Q12,1)
         BL                Inv_sqrt1
         VDUP.S32          Q12, r0
         LDMFD             sp!, {r0 - r3}

         VQSHL.S32         Q12, Q12, #7                                  ; L_shl(Inv_sqrt(s), 7)
         VSHRN.S32         D20, Q12, #16                                 ; k = extract_h(L_shl(Inv_sqrt(s), 7))
         VMULL.S16         Q12, D14, D20                                 ; k= mult(k, 32440)        
         VSHRN.S32         D20, Q12, #15
                
         VQDMULL.S16       Q12, D0, D20                                    ; D20 = k                
         VQRSHRN.S32       D0,  Q12, #7
         VQDMULL.S16       Q12, D1, D20
         VQRSHRN.S32       D1,  Q12, #7
         VQDMULL.S16       Q12, D2, D20
         VQRSHRN.S32       D2,  Q12, #7
         VQDMULL.S16       Q12, D3, D20
         VQRSHRN.S32       D3,  Q12, #7
         VQDMULL.S16       Q12, D4, D20
         VQRSHRN.S32 	   D4,  Q12, #7
         VQDMULL.S16 	   Q12, D5, D20
         VQRSHRN.S32 	   D5,  Q12, #7
         VQDMULL.S16 	   Q12, D6, D20
         VQRSHRN.S32       D6,  Q12, #7
         VQDMULL.S16       Q12, D7, D20
         VQRSHRN.S32       D7,  Q12, #7
         VQDMULL.S16       Q12, D8, D20
         VQRSHRN.S32       D8,  Q12, #7
         VQDMULL.S16       Q12, D9, D20
         VQRSHRN.S32       D9,  Q12, #7
         B                 build_rr_matrix
;--------------------------------------------------------
case_saturating
; Scale input array right one place.
;      for(i=0; i<L_CODE; i++)
;      {
;         h2[i] = shr(h[i], 1);
;       }
         VSHR.S16           Q0, Q0, #1
         VSHR.S16           Q1, Q1, #1
         VSHR.S16           Q2, Q2, #1
         VSHR.S16           Q3, Q3, #1
         VSHR.S16           Q4, Q4, #1 

build_rr_matrix
; Reverse h2 array, because we shall be running through it backwards.

         VREV64.S16         D0, D0
         VREV64.S16         D1, D1 
         VREV64.S16         D2, D2
         VREV64.S16         D3, D3
         VREV64.S16         D4, D4
         VREV64.S16         D5, D5
         VREV64.S16         D6, D6
         VREV64.S16         D7, D7
         VREV64.S16         D8, D8
         VREV64.S16         D9, D9
       
; Store the scaled array h2[]
         MOV                 r7, sp                            ;get the H2 buffer first address     
         VST1.S16           {D0, D1, D2, D3}, [r7]!           ;(tc workaround) this should be a normal STR [sp,#H2_BUFFER_OFS]     
         VST1.S16           {D4, D5, D6, D7}, [r7]!      
         VST1.S16           {D8, D9}, [r7]
       
         VMOV.U16           D4, #0
         MOV                ip, #10
          
loop_signs
         VLD1.S16            D0, [r1]!          
         VSHR.U16.U16        D0, D0, #15                                                     
         VSHL.U16            D4, D4, #1
         VORR.U16            D4, D4, D0                             
         SUBS                ip, ip, #1
         BNE                 loop_signs
          
         VSHR.U16            D20, D4, #1                               
         VEXT.U16            D5, D20, D4, #3
         VEXT.U16            D6, D20, D4, #2
         VEXT.U16            D7, D20, D4, #1
          

         VDUP.U16            D8,   D4[3]
         VDUP.U16            D9,   D4[2]
         VDUP.U16            D10,  D4[1]
         VDUP.U16            D11,  D4[0]

         VEOR.U16            D8,  D8,  D4
         VEOR.U16     	     D9,  D9,  D5
         VEOR.U16 	     D10, D10, D6
         VEOR.U16 	     D11, D11, D7
         ADD 		     r8, sp, #H2_BUFFER_OFS
         MVN 		     r6,  #81                                             ; -(L_CODE + 1) * SIZEOF_S16
         MOV                 r5,  #9
         VMOV.S32            D24,   #0
	 VMOV.S32            D25,   #0
         SUB                 r9, r2, #6                                      ; 3 * SIZEOF_S16
	 VLD1.S16            D0, [r8]!
	     
loop_first
	 VLD1.S16            D1, [r8]!	      
; Update correlation 
	 VQDMLAL.S16         Q12, D0, D0[3]
	 VQRSHRN.S32         D20, Q12, #16
; Check sign bit for each SIMD lane
	 VTST                D22.S16, D8.U16, D18.U16	      
; Multiply by +32766 or -32767, depending on sign
	 VBSL.S16            D22, D16, D15
	 VQDMULH.S16         D22, D20, D22
	    
; Along the diagonal, do not multiply by the product of the sign bits.
	 VBIF                D20.S16, D22.S16, D17.U16
; Store four results
	 VST1.S16            D20, [r9], r6
	 VEXT.S16            D2, D1, D0, #3
	 VQDMLAL.S16         Q12, D2, D0[2]
	 VQRSHRN.S32         D20, Q12, #16
	 VTST.S16            D22, D9, D18
	 VBSL.S16            D22, D16, D15
	 VQDMULH.S16         D22, D20, D22
	 VBIF.S16            D20, D22, D17
	 VST1.S16            D20, [r9], r6
	 VEXT.S16            D2, D1, D0, #2
	 VQDMLAL.S16         Q12, D2, D0[1]
         VQRSHRN.S32         D20, Q12, #16
	 VTST.S16            D22, D10, D18
	 VBSL.S16            D22, D16, D15
	 VQDMULH.S16         D22, D20, D22
	 VBIF.S16            D20, D22, D17
	 VST1.S16            D20, [r9], r6
	 VEXT.S16            D2,  D1, D0, #1
	 VQDMLAL.S16         Q12, D2, D0[0]
	 VQRSHRN.S32         D20, Q12, #16
	 VTST.S16            D22, D11, D18
	 VBSL.S16            D22, D16, D15
	 VQDMULH.S16         D22, D20, D22
	 VBIF.S16            D20, D22, D17
	 VST1.S16            D20, [r9], r6
	 VSHL.U16            D18, D18, #1
	 VMOV.S16            D0, D1
	 SUBS                r5, r5, #1
	 BNE                 loop_first
	     
; Fixup at end of loop
; --------------------
	 VLD1.S16            D1, [r8]!
	 VQDMLAL.S16         Q12, D0, D0[3]
	 VQRSHRN.S32         D20, Q12, #16
	 
	 VTST                D22.S16, D8.U16, D18.U16
	 VBSL.S16            D22, D16, D15
	 VQDMULH.S16         D22, D20, D22
	 VBIF.S16            D20, D22, D17.U16
	 VST1.S16            D20, [r9], r6
	 VEXT.16              D2, D1, D0, #3
	 VQDMLAL.S16          Q12, D2, D0[2]
	 VQRSHRN.S32          D20, Q12, #16
	 VTST                 D22.S16, D9.U16, D18.U16
	 VBSL.S16             D22, D16, D15
	 VQDMULH.S16          D22, D20, D22
	 VBIF                 D20.S16, D22.S16, D17.U16
	 ADD                  r7, r9, #2 ; 1 * SIZEOF_S16
	 VST1.S16             D20[1], [r7]! ; use limited store
	 VST1.S16             D20[2], [r7]!
	 VST1.S16             D20[3], [r7]!
	 ADD                  r9, r9, r6
	 VEXT.16              D2, D1, D0, #2
	 VQDMLAL.S16          Q12, D2, D0[1]
	 VQRSHRN.S32          D20, Q12, #16
	 VTST                 D22.S16, D10.U16, D18.U16
	 VBSL.S16             D22, D16, D15
	 VQDMULH.S16          D22, D20, D22
	 VBIF                 D20.S16, D22.S16, D17.U16
	 ADD                  r7, r9, #4 ; 2 * SIZEOF_S16
	 VST1.S16             D20[2], [r7]! ; use limited store
	 VST1.S16             D20[3], [r7]!
	 ADD                  r9, r9, r6
	 VEXT.16              D2, D1, D0, #1
	 VQDMLAL.S16          Q12, D2, D0[0]
	 VQRSHRN.S32          D20, Q12, #16
	 ADD                  r7, r9, #6 ; 3 * SIZEOF_S16
	 VST1.S16             D20[3], [r7], r6
	     
; Finish matrix off diagonal
    	 MOV                  r1, #1
outer_loop

	 SUB                  r2, r2, #8
	 RSB                  r7, r1, #0 ; negate to shift right by variable amount
	 VDUP.S16             D22, r7
	 ADD                  r7, sp, #H2_BUFFER_OFS
	 ADD                  r8, r7, r1, LSL #3 ; = r1 * 4 * SIZEOF_S16
	 VDUP.U16             D8, D4[3]              ; get sequence of signbit[j]
	 
	 VSHL.U16             D20, D4, D22         ; calculate sequence of signbit[i]
	 VEOR.U16             D8, D8, D20           ; exclusive-or to multiply signs.
	 VDUP.U16             D9, D4[2]
	 VSHL.U16             D20, D5, D22
	 VEOR.U16             D9, D9, D20
	 VDUP.U16             D10, D4[1]
	 VSHL.U16             D20, D6, D22
	 VEOR.U16             D10, D10, D20
	 VDUP.U16             D11, D4[0]
	 VSHL.U16             D20, D7, D22
	 
	 VEOR                 D11.U16, D11.U16, D20.U16
	 VMOV                 D18.U16, #0x0001
	 VMOV.S32             D24, #0
	 VMOV.S32             D25, #0
	 SUB                  r9, r2, #6 ; 3 * SIZEOF_S16
	 VLD1.S16             D0, [r8]!
	 RSB                  r5, r1, #10
	 SUBS                 r5, r5, #1
	 BEQ                  fixup_end_loop 
	 
loop_main
	 VLD1.S16             D3, [r7]!
	 VLD1.S16             D1, [r8]!
; Update correlation
	 VQDMLAL.S16          Q12, D0, D3[3]
	 VQRSHRN.S32          D20, Q12, #16
; Check signbit for each SIMD lane
	 VTST                 D22.S16, D8.U16, D18.U16
; Multiply by +32766 or -32767, depending on sign
	 VBSL.S16             D22, D16, D15
	 VQDMULH.S16          D20, D20, D22
; Store four results

	 VST1.S16             D20, [r9], r6
	 VEXT.S16             D2, D1, D0, #3
	 VQDMLAL.S16          Q12, D2, D3[2]
	 VQRSHRN.S32          D20, Q12, #16
	 VTST.S16             D22, D9, D18
	 VBSL.S16             D22, D16, D15
	 VQDMULH.S16          D20, D20, D22
	 VST1.S16             D20, [r9], r6
	 VEXT.S16             D2, D1, D0, #2
	 VQDMLAL.S16          Q12, D2, D3[1]
	 VQRSHRN.S32          D20, Q12, #16
	 VTST.S16             D22, D10, D18
	 VBSL.S16             D22, D16, D15
	 VQDMULH.S16          D20, D20, D22
	 VST1.S16             D20, [r9], r6
	 VEXT.S16             D2, D1, D0, #1
	 VQDMLAL.S16          Q12, D2, D3[0]
	 VQRSHRN.S32          D20, Q12, #16
	 VTST.S16             D22, D11, D18
	 VBSL.S16             D22, D16, D15
	 VQDMULH.S16          D20, D20, D22
	 VST1.S16             D20, [r9], r6
	 VSHL                 D18.U16, D18.U16, #1
	 VMOV.S16             D0, D1
	 SUBS                 r5, r5, #1	 
	 BNE                  loop_main
	     	     
fixup_end_loop

	 VLD1.S16             D3, [r7]!
	 VLD1.S16             D1, [r8]!
	 VQDMLAL.S16          Q12, D0, D3[3]
	 VRSHRN.S32           D20, Q12, #16
	 VTST                 D22.S16, D8.U16, D18.U16
	 VBSL.S16             D22, D16, D15
	 VQDMULH.S16          D20, D20, D22
	 VST1.S16             D20, [r9], r6
	 VEXT.16              D2, D1, D0, #3
	 VQDMLAL.S16          Q12, D2, D3[2]
	 VQRSHRN.S32          D20, Q12, #16
	 VTST                 D22.S16, D9.U16, D18.U16
	 VBSL.S16             D22, D16, D15
	 VQDMULH.S16          D20, D20, D22
	 ADD                  r7, r9, #2 ; 1 * SIZEOF_S16
	 VST1.S16             D20[1], [r7]! ; use limited store
	 VST1.S16             D20[2], [r7]!
	 VST1.S16             D20[3], [r7]!
	 ADD                  r9, r9, r6
	 VEXT.16              D2, D1, D0, #2
	 VQDMLAL.S16          Q12, D2, D3[1]
	 VQRSHRN.S32          D20, Q12, #16
	 VTST                 D22.S16, D10.U16, D18.U16
	 VBSL.S16             D22, D16, D15
	 VQDMULH.S16          D20, D20, D22
	 ADD                  r7, r9, #4 ; 2 * SIZEOF_S16
	 VST1.S16             D20[2], [r7]! ; use limited store
	 VST1.S16             D20[3], [r7]!
	 ADD                  r9, r9, r6
	 VEXT.16              D2, D1, D0, #1
	 VQDMLAL.S16          Q12, D2, D3[0]
	 VQRSHRN.S32          D20, Q12, #16
	 ADD                  r7, r9, #6 ; 3 * SIZEOF_S16
	 VTST                 D22.S16, D11.U16, D18.U16
	 VBSL.S16             D22, D16, D15
	 VQDMULH.S16          D20, D20, D22
	 VST1.S16             D20[3], [r7], r6
	 ADD                  r1, r1, #1
	 CMP                  r1, #10
	 
	 BNE outer_loop

         ADD                  sp, sp, #H2_BUFFER_SIZE
         LDMFD                sp!, {r4-r12, pc} 
         ENDP
         END  

