;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;*************************************************************** 
;static Word16 Interpol(                    /* return result of interpolation */
;     Word16 * x,                           /* input vector                   */
;     Word16 * fir,                         /* filter coefficient             */
;     Word16 frac,                          /* fraction (0..resol)            */
;     Word16 resol,                         /* resolution                     */
;     Word16 nb_coef                        /* number of coefficients         */
;)
;****************************************************************
; *x      ---   r0
; *fir    ---   r1
; frac    ---   r2
; resol   ---   r3
; nb_coef ---   r4            ;nb_coef = 12

          AREA |.text|, CODE, READONLY
          EXPORT Interpol_asm

Interpol_asm FUNCTION

          STMFD         r13!, {r4 - r12, r14} 
          SUB           r4, r0, #22                    ; x -= nb_coef
          ADD           r5, r2, #1                     ; 1 + frac
	  MOV           r7, #24
          SUB           r6, r3, r5                     ; resol - 1 - frac
          MUL           r12, r6, r7                    ; (resol - 1 - frac) * 24

	  CMP           r12, #96
	  BEQ           Lable
          ADD           r5, r1, r12, LSL #1             ; get fir[k] address
          ; r4 ---- x  r5 ---- fir[]
          
          VLD1.S16      {D0, D1, D2, D3}, [r4]!        ;get x[0] ~ x[15]
	  VLD1.S16      {D4, D5}, [r4]!                ;get x[16] ~ x[23]
	  
	  VLD1.S16      {D6, D7, D8, D9}, [r5]!        ;get 16 fir[]
	  VLD1.S16      {D10, D11}, [r5]!              ;get 8 fir[]


	  VQDMULL.S16   Q10, D0, D6
	  VQDMLAL.S16   Q10, D1, D7
          VQDMLAL.S16   Q10, D2, D8
	  VQDMLAL.S16   Q10, D3, D9
	  VQDMLAL.S16   Q10, D4, D10
	  VQDMLAL.S16   Q10, D5, D11

	  VQADD.S32     D20, D20, D21
          VPADD.S32     D20, D20, D20
          MOV           r9, #0x00008000  
          VMOV.S32      r12, D20[0]
          QADD          r8, r12, r12
	  B             Lable1

Lable 
          ADD           r8, r12, #11
	  ADD           r4, r4, #22                   ;x[11] address
          ADD           r5, r1, r8, LSL #1            ;get fir[96 + 11] address
	  LDRSH         r10, [r4]          
	  LDRSH         r11, [r5]
	  MUL           r8, r10, r11                  ;L_sum = x[11] * fir[k+11]
	  QADD          r10, r8, r8
	  MOV           r9, #0x00008000
	  QADD          r8, r10, r10

Lable1
          QADD          r0, r8, r9
          MOV           r0, r0, ASR #16 

Interpol_asm_end
		     
          LDMFD   	    r13!, {r4 - r12, r15} 
          ENDFUNC
          END

