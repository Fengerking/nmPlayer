; ***** BEGIN LICENSE BLOCK *****   
; Source last modified: $Id: asm_mlt.asm,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
;  
; REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM   
; Portions Copyright (c) 1995-2005 RealNetworks, Inc.   
; All Rights Reserved.   
;   
; The contents of this file, and the files included with this file, 
; are subject to the current version of the RealNetworks Community 
; Source License (the "RCSL"), including Attachment G and any 
; applicable attachments, all available at 
; http://www.helixcommunity.org/content/rcsl.  You may also obtain 
; the license terms directly from RealNetworks.  You may not use this 
; file except in compliance with the RCSL and its Attachments. There 
; are no redistribution rights for the source code of this 
; file. Please see the applicable RCSL for the rights, obligations 
; and limitations governing use of the contents of the file. 
;   
; This file is part of the Helix DNA Technology. RealNetworks is the 
; developer of the Original Code and owns the copyrights in the 
; portions it created. 
;   
; This file, and the files included with this file, is distributed 
; and made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY 
; KIND, EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS 
; ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES 
; OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET 
; ENJOYMENT OR NON-INFRINGEMENT. 
;   
; Technology Compatibility Kit Test Suite(s) Location:   
; https://rarvcode-tck.helixcommunity.org   
;   
; Contributor(s):   
;   
; ***** END LICENSE BLOCK ***** 

;-----------------------------------------------------------------------------------------------------------
; 	File 		:	MLT.asm 
;   Description : C55X assembly routines for functions present in mlt.c  WITHOUT DMA support
;	
;------------------------------------------------------------------------------------------------------------
;
;/*
; * Gecko2 stereo audio codec.
; * Developed by Ken Cooke (kenc@real.com)
; * August 2000
; *
; * Last modified by:	Firdaus Janoos (firdaus@ti.com)
; *						14/09/01
; *
; * Fast MLT/IMLT functions.
; *
; * NOTES:
; * Requires inplace FFT module (without normalization) and reorder table.
; *
; */

	.if __OPTIMISE_SECOND_PASS__

	.if !CONST_TABLE_DMA	
	; dma support for constant tables disabled
	.mmregs
	.def _RA_TNI_PreMultipy
	
	.def _RA_TNI_PostMultiply	
	
	.def _RA_TNI_DecWindowNoOverlap 	
	
	.def _RA_TNI_IMLTNoOverlap
	
	.global _RA_TNI_fixcos4tab_tab			; defined in fixtabs.c
	.global _RA_TNI_fixsin4tab_tab
	

;----------------------------------------------------------------------------------
; _RA_TNI_PreMultipy : ( Not C-callable)
; 
;/*
; * Pre-FFT complex multiplication.
; *
; * NOTES:
; * Includes input re-ordering, so cannot operate in-place.
; *
; * Fixed-point changes:
; *  - arithmetic is now fixed-point
; *  - number of integer bits:
; *      input:  zbuf = MLTBITS
; *      output: zfft = MLTBITS + 1
; */
;
; Input : AR0 -> zbuf (long*)
;		: AR1 -> zfft(long*)
;		: T0 -> nsamples (short)
;
;Returns : void
;
; NOTE : Affects the contents of zfft arrays
; 
; Registers Touched : AR0, AR1, T0,
;
; Globals Referenced : _RA_TNI_fixcos4tab_tab, _RA_TNI_fixsin4tab_tab : array of longs (ptrs)
; 

	.text
_RA_TNI_PreMultipy:
	

	AC0 = T0 					; AC0 -> nsamples
	||
	; allocate space on stack to store -ar
	SP = SP- #3					; 2 words for ar
	; XSP -> -ar_h								; SP-3 for alignment 
	
	AC0 = AC0<<#-8				; ( nsamples<<1 ) >>9 : to get offset in words
	||		
	XAR5 = XSP					
	; XAR5 points to top of stack (ar_h)
	
	T1 = AC0					; move offset into T1

	bit(T1,@0) = #0
	; for dword addressing
		
	; load address of approriate RA_TNI_fixcos4tab_tab_x into XAR2
	XAR2 = _RA_TNI_fixcos4tab_tab		; address of RA_TNI_fixcos4tab_tab

	swap (T2, T0) 
	||
	;  T1 -> offset into the RA_TNI_fix***4tab_tab arrays
	;  T2 = nsamples
	T0 = T1 	
	;  T0 -> offset into the RA_TNI_fix***4tab_tab arrays
	
	; load address of approriate RA_TNI_fixsin4tab_tab_x into XAR3
	XAR3 = _RA_TNI_fixsin4tab_tab		; address of RA_TNI_fixsin4tab_tab 

	XAR2 = dbl(*AR2(T0) )		
	;XAR2 -> cosptr
	||
	T1 = T2
	; T1 = nsamples						

	;initialise XAR4 to zbuf2
	XAR4 = XAR0 		
	;XAR4  -> zbuf
		
	XAR3 = dbl( *AR3(T0) )		
	;XAR3 -> sinptr
	||
	T2 = T2<<#1
	;  T2 -> nsamples << 1

	T1 = T1>>1 			; nsamples >> 1	
	
	AR4 = AR4 + T2 		; AR4 = zbuf + nsamples(<<1) 	 | for long offset
	||
	mar ( T1 - #1 )		; (nsamples>>1)  - 1
	
	AR4 = AR4 - #2      ; AR4 = zbuf + (nsamples-1) <<1	 |
	;XAR4 -> zbuf2

	
	
	BRC0 = T1 			
	; BRC0 = (nsamples>>1)  - 1  for (nsamples>>1) iterations
	||
	; load T0 with 2 for pointer advancement
	T0 = #2 
	
	;-------------------------------------
	; so upto now
	;  T0 -> #2
	;  T1 -> (nsamples >> 1) - 1
	;  BRC0 -> (nsamples >>1) - 1
	;	
	;  XAR0 -> zbuf
	;  XAR1 -> zfft
	;  XAR2 -> cosptr
	;  XAR3 -> sinptr
	;  XAR4 -> zbuf2
	;  XAR5 -> top of stack (-ar)
	;--------------------------------------		
	
	mar( *AR3+)  || mar( *AR2+ )
	;   XAR2 -> cr_l
	;   XAR3 -> ci_l
		
	
	blockrepeat {
		;	AR0 ->	ar_h
		;   AR4 ->  ai_h
		;   XAR2 -> cr_l
		;   XAR3 -> ci_l
		
		; To multiply ar * cr :: ai * cr
		XCDP = XAR2		
		; XCDP = cr_l
		
				
		; this code has to be rearranged
		mar( *AR0+ ) || mar( *AR4+) 
		;	AR0 ->	ar_l
		;   AR4 ->  ai_l
		;   XCDP -> cr_l
		;
		;   
			
		AC0 = uns(*AR0-) * uns(coef( *CDP )),
			AC1 = uns( *AR4-) * uns(coef( *CDP))
			
		; AR0 -> ar_h
		; AR4 -> ai_h
		; CDP -> cr_l
		;
		; AC0 = ar_l * cr_l
		; AC1 = ai_l * cr_l
		
		
		
		AC0 = ( AC0>>#16) + ( (*AR0+) * uns(coef( *CDP- )) ),
			AC1 = ( AC1>>#16) + ( (*AR4+) * uns(coef( *CDP- )) )
		; AR0 -> ar_l
		; AR4 -> ai_l
		; CDP -> cr_h
		;
		; AC0 = ar_h * cr_l + (ar_l * cr_l)>>16
		; AC1 = ai_h * cr_l + (ai_l * cr_l)>>16
		
		AC0 =  AC0 + ( uns(*AR0-) * coef( *CDP ) ),
			AC1 =  AC1 + ( uns(*AR4-) * coef( *CDP ) ) 
		; AR0 -> ar_h
		; AR4 -> ai_h
		; CDP -> cr_h
		;
		; AC0 = (ar_l * cr_h + ar_h * cr_l) + (ar_l * cr_l)>>16
		; AC1 = (ai_l * cr_h + ai_h * cr_l) + (ai_l * cr_l)>>16
		
		
		AC0 = ( AC0>>#16) + ( (*AR0) * coef( *CDP+ ) ),
			AC1 = ( AC1>>#16) + ( (*AR4+) * coef( *CDP+ ) )
		; AR0 -> ar_h
		; AR4 -> ai_l
		; CDP -> cr_l
		
		; AC0 ( ar*cr)  = ar_l * cr_h + ((ar_l * cr_h + ar_h * cr_l) + (ar_l * cr_l)>>16)>>16
		; AC1 ( ai*cr)  = ai_l * cr_h + ((ai_l * cr_h + ai_h * cr_l) + (ai_l * cr_l)>>16)>>16
		
		XCDP = XAR3  
		; CDP = ci_l
		;----------------------------------------------------------
		AC2 = dbl( *AR0 ) 		; AC2 = ar
		; to be used for calculating -ar 
		AC2 = -AC2	
		; AC2 = -ar
		dbl(*AR5) = AC2				; put -ar on top of stack
		; AR5 points to -ar_h
		;----------------------------------------------------------
		mar(*AR5+) || mar( *(AR0 + T0 ))
		; AR0 -> (ar+1)_h
		; AR5 -> -ar_l		
		; to be rearranged		
		
		mar( *(AR2+ T0) ) || mar ( *(AR3+T0) )
		
		
		; To multiply -ar * ci :: ai * ci
						
		;	AR0 ->	ar_h		/*/*/*/*/*/*/*
		;   AR4 ->  ai_l
		;   AR5 -> -ar_l
		;   XCDP -> ci_l   
		
		;   XAR2 -> (cr+1)_l 	/****/*/*/*/*/
		;   XAR3 -> (ci+1)_l	/**/*/*/*/*/*
		
			
		AC2 = uns (*AR5-) * uns(coef( *CDP )),
			AC3 = uns( *AR4-) * uns(coef( *CDP)) 
			
		; AR5 -> -ar_h
		; AR4 -> ai_h
		; CDP -> ci_l
		;
		; AC2 = -ar_l * ci_l
		; AC3 = ai_l * ci_l
				
		
		
		
		AC2 = ( AC2>>#16) + ( (*AR5+) * uns(coef( *CDP- ) )),
			AC3 = ( AC3>>#16) + ( (*AR4+) * uns(coef( *CDP- ) ))
		; AR5 -> -ar_l
		; AR4 -> ai_l
		; CDP -> ci_h
		;
		; AC2 = ar_h * ci_l + (ar_l * ci_l)>>16
		; AC3 = ai_h * ci_l + (ai_l * ci_l)>>16
		
		AC2 =  AC2 + ( uns(*AR5-) * coef( *CDP ) ),
			AC3 =  AC3 + ( uns(*AR4-) * coef( *CDP ) )
		; AR5 -> -ar_h
		; AR4 -> ai_h
		; CDP -> ci_h
		;
		; AC2 = (ar_l * ci_h + ar_h * ci_l) + (ar_l * ci_l)>>16
		; AC3 = (ai_l * ci_h + ai_h * ci_l) + (ai_l * ci_l)>>16
		
		
		AC2 = ( AC2>>#16) + ( (*AR5) * coef( *CDP+ ) ),
			AC3 = ( AC3>>#16) + ( (*(AR4-T0)) * coef( *CDP+ ) )
		; AR5 -> -ar_h
		; AR4 -> (ai-1)_l  i.e. : zbuf-2
		; CDP -> ci_l
		
		;
		; AC2 ( -ar*ci)  = ar_l * ci_h + ((ar_l * ci_h + ar_h * ci_l) + (ar_l * ci_l)>>16)>>16
		; AC3 ( ai*ci)  = ai_l * ci_h + ((ai_l * ci_h + ai_h * ci_l) + (ai_l * ci_l)>>16)>>16
		
		
		;--------------------------------------------------
		; so upto now
		;  T0 -> #2
		;  T1 -> (nsamples >> 1) - 1
		;  BRC0 -> (nsamples >>1) - 1
		;
		;  AC0 -> ar*cr
		;  AC1 -> ai*cr
		;  AC2 -> -ar*ci
		;  AC3 -> ai*ci
		;
		;  XAR0 -> zbuf + 2
		;  XAR1 -> zfft
		;  XAR2 -> cosptr + 2
		;  XAR3 -> sinptr + 2
		;  XAR4 -> zbuf2 - 2
		;  XAR5 -> -ar_h (top of stack) 
		;----------------------------------------------------
		
		; calculate ar*cr + ai*ci
		AC0 = AC0 + AC3 || mar( *(AR0 + T0 ))
		; zbuf += 2
		;store in zfft
		dbl(*AR1+ ) = AC0 
		; zfft += 2		
		
		
		; calculate -ar*ci + ai*cr
		AC2 = AC2 + AC1 || mar ( *(AR4 - T0 ))
		; zbuf2 -= 2
		;store in zfft
		dbl(*AR1+ ) = AC2		
		; zfft += 2
		
		;--------------------------------------------------
		; for the next iteration
		;  T0 -> (nsamples >> 1) - 1
		;  T1 -> #2
		;
		;  XAR0 -> zbuf + 4
		;  XAR1 -> zfft + 4
		;  XAR2 -> cosptr + 2
		;  XAR3 -> sinptr + 2
		;  XAR4 -> zbuf2 - 4
		;  XAR5 -> -ar_h (top of stack) 
		;----------------------------------------------------

	}
	; clear up the stack
	SP = SP + #3
	return 			
; end of _RA_TNI_PreMultipy
	
;----------------------------------------------------------------------------------

	
;----------------------------------------------------------------------------------
; _RA_TNI_PostMultiply :  ( Not C-callable)
; 
;
;/*
; * Post-FFT complex multiplication.
; *
; * NOTES:
; * Normalization by sqrt(2/N) is rolled into cos/sin tables.
; * Includes input re-ordering, for power-of-two FFT.
; * Includes output re-ordering, so cannot operate in-place.
; *
; * Fixed-point changes:
; *  - arithmetic is now fixed-point
; *  - number of integer bits:
; *      input:  fft = MLTBITS + 1 + nfftlog2 (may exceed 32, but don't worry)
; *      output: mlt = POSTBITS
; */	
;
;
;  Input : AR0 -> fft (long*)
;		: AR1 -> mlt(long*)
;		: T0 -> nsamples (short)
;
; Returns : void
;
; NOTE : modifies the mlt array, does not affect fft array
; 
; Registers Touched : XAR0, XAR1, XAR2, XAR3, XAR4,XAR5, XAR6, XCDP, AC0, AC1, AC2, AC3, T0,T1, T2, T3
;
; Globals Referenced : RA_TNI_fixcos1tab_tab, RA_TNI_fixcos1tab_tab_x, 
;
; 
	
	.global _RA_TNI_fixcos1tab_tab			; array of long *
	
;	#define MLTBITS				22		/* see note in ScalarDequant() */
;	#define POSTBITS			19		/* never set less than 17, or risk rampant saturation before gain control */
MLTBITS  .set 22
POSTBITS .set 19
	
	; calulate MLTBITS + 8 - POSTBITS
	.eval   MLTBITS+8-POSTBITS , NPOSTSHIFT_CONST
	.cpl_on
	
		
	.sect .const
possat .long 0x7FFFFFFF 	 ;for +ve saturation
negsat .long 0x80000000      ;for -ve saturation

	.text	
	.mmregs
_RA_TNI_PostMultiply:

	;----------------------------------------
	; To Assign
	;
	; XAR0 -> fft (ar)
	; XAR1 -> mlt
	; XAR2 -> fixcosptr (cr)
	; XAR3 -> fixsinptr (ci)
	; XAR4 -> fft + 2 (ai) 
	; XAR5 -> mlt2
	; XAR6 -> *****
	;
	; T0 -> i, npostshifts, and all other temps
	; T1 -> nsamples
	; T2 -> npostshifts
	; T3 -> npostshifts-16
	

	; clear the ARMS bit to use bit reversed addressing
	.ARMS_off

	T1 = T0				
	; nsamples into T1
	||
	AC0 = T0
	AC0 = AC0 << #-8
	T0 = AC0
	; T0 (i) -> (nsamples>>9)<<1
	; for dword (long) addressing
	
	bit( T0, @0 ) = #0
	; to allow for dword addressing
	
	;set XAR2 to fixcosptr, XAR3 fixsinptr
	XAR2 = _RA_TNI_fixcos1tab_tab
	XAR2 = dbl(*AR2(T0) )		
	; XAR2 (fixcosptr) = RA_TNI_fixcos1tab_tab[i] 
	||
	T0 = T0 >> #1	
	; T0 = (nsamples>>9)
	
	; load fixsinptr in XAR3
	XAR3= XAR2
	
	T2 = T0
	
	; T2 {npostshifts} = nsamples >> 9
	mar( T2 + NPOSTSHIFT_CONST	)
	; T2 -> npostshitft
	||
	mar(AR3 + T1)
	; XAR3 (fixsinptr) = fixcosptr + (nsamples>>1)<<1

	
	; calculate mlt2 and store in XAR5
	XAR5 = XAR1
	; XAR5 = mlt
	||
	repeat(#1)			; 2 times
	 AR5 = AR5 + T1  
	; AR5 = mlt + nsamples<<#1
	; {<<1} for dword addressing
	AR5 = AR5 - #2
	;AR5 = mlt + (nsamples-1)<<#1
					
	T0 = T1
	; T0 -> nsamples
	; T0 is used for bit reversed addressing 
	; { nsamples for long addressing }
	

	; calculate trip count
	T1 = T1 >> #1		
	; T1 = nsamples /2
	mar(T1 - #1) 		
	; trip count of nsamples/2 -1
	||
	T3 = T2 
	; set up T3 to npostshifts
	BRC0 = T1 
	; load trip count into BRC0 = nsamples/2 -1	
	||
	mar(T3 - #16) 
	; T3 = npostshifts-16
	
	
	T1 = T2 
	; set up T1 to npostshifts
	
	XAR4 = XAR0
	
	mar( T1 - #31)
	; T1 = npostshifts - 31
	
	XCDP = XAR2						
	; initialize to CDP = cr_h ; cosptr 
	
	AR4 = AR4 +	#2		  ; NEED TO REMEMBER THIS IS DONE OUTSIDE THE LOOP !!
	; AR4 = fft[idx+1]
	||	
	; do nsamples>>1 times		
	blockrepeat{
	;----------------------------------------------
	; current assignments
	; 
	;	XAR0 {ar} -> fft[idx]
	;	XAR1      -> mlt
	;   XAR2 {cr} -> fixcosptr
	;   XAR3 {ci} -> fixsinptr
	;   XAR4 {ai} -> fft[idx + 1]
	;   XAR5 	  -> mlt2
	; 	XAR6 	  -> *****
	;	XCDP 	  -> cr or ci
	;
	;   T0 -> nsamples (used for bit rev. indexing)
	;	T1 -> npostshifts - 31
	;   T2 -> npostshifts
	;   T3 -> npostshift - 16
	;  NOTE : mlt stores ar*cr, mlt2 stores ai*cr
	
	; AR0 -> ar_h
	; AR4 -> ai_h
	; CDP = cr_h
	
	; multipy (ar * cr) :: (ai * cr)
	; to assign
	; AC0  -> ar*cr
	; AC1 -> ai* cr
	
	AR6 = #-1
	; load 0xFFFF in AR6
	||
	AC0 = (*AR0+) * coef(*CDP+),
		AC1 = (*AR4+) * coef(*CDP+)
	;AC0 = ar_h * cr_h
	;AC1 = ai_h * cr_h
	; AR0 -> ar_l  
	; CDP-> cr_l 
	; AR4 -> ai_l			
	AC2 = uns(*AR0-) * uns(coef(*CDP)),
		AC3= uns(*AR4-) * uns( coef(*CDP) )
	; AC2 = ar_l * cr_l    
	; AC3 = ai_l * cr_l	   
	; AR0 -> ar_h
	; CDP -> cr_l
	; AR4 -> ai_h
	AC2 = (AC2 >> #16) + ( (*AR0+) * uns(coef(*CDP-) ) ),
		AC3 = (AC3 >> #16) + ( (*AR4+) * uns(coef(*CDP-) ) )
	; AC2 = (ar_h*cr_l) + (ar_l * cr_l) >> 16
	; AC3 = (ai_h*cr_l) + (ai_l * cr_l) >> 16
	; AR0 -> ar_l
	; CDP -> cr_h
	; AR4 -> ai_l
	AC2 = AC2 + ( uns(*AR0) * coef(*CDP) ),
		AC3 = AC3 + ( uns(*AR4) * coef(*CDP) ) 		
		
	; AC2 = (ar_l*cr_h) + (ar_h*cr_l) + (ar_l * cr_l) >> 16   
	; AC3 = (ai_l*cr_h) + (ai_h*cr_l) + (ai_l * cr_l) >> 16
	; AR0 -> ar_l
	; CDP -> cr_h
	; AR4 -> ai_l
	
	
	AC0 = AC0 + ( AC2 << #-16) || mar(*AR2+)		; fixcosptr++		******for next pass
	AC1 = AC1 + ( AC3 << #-16) || mar(*AR2+)		; AR2 = AR2 + 2		******for next pass
	; AC0 = (ar_h*cr_h) + ((ar_l*cr_h) + (ar_h*cr_l) + (ar_l * cr_l) >> 16) >> 16
	; AC1 = (ai_h*cr_h) + ((ai_l*cr_h) + (ai_h*cr_l) + (ai_l * cr_l) >> 16) >> 16
	; AC2_L, AC3_L -> bits 31-16 of product
	; AR0 -> ar_l
	; CDP -> cr_h
	; AR4 -> ai_l
	; AR6 -> 0xFFFF
	
	
	;********************/*/*/*/*/ NOTE */*/*/*/*//*/
	XCDP = XAR3
	||	
	; CDP -> ci_h				
	AC2 = AC2 & AR6				
	; isolate bits 31-16 of ar*cr
	 
	AR6 = LO(AC3) 	
	; save bits 31-16 of ai*cr in AR6
	||
	AC3 = AC0 << T1
	; AC3 = ar*cr[63-32] >> ( 31-npostshifts) 
	
	;********************/*/*/*/*/ NOTE */*/*/*/*//*/
	mar(*CDP+)
	; CDP -> ci_l
	||
	AC0 = AC0 << T2
	; ar*cr[63-32]  << npostshifts
	
	AC2 = AC2 << T3
	; AC2 = ar*cr[31-16] >> (16-npostshifts)
	
	AC0 = AC0 | AC2
	; AC0 = arcr[63-32]<<npostshifts | arcr[31-16] >> (16-npostshifts)	
	||
	if( AC3 > 0 ) execute(D_Unit)  ; if positive overflow due to shifting
		AC0 = dbl(*(#possat) )	   ; positively saturate AC0
	
	AC3 = AC3 + #1 	; to check for neg. underflow
		
	AC2 = AC1 << T1 
	; AC2 = ai*cr[63-32] >> (31-npostshifts)
	||
	if(AC3 < 0 ) execute(D_Unit) 	; if negative overflow
		AC0 = dbl(*(#negsat))		; neg. saturation
	
_AR6 .set 0016h		; memory mapped reg

	AC3 = uns(*(_AR6))
	; NOTE : The use of uns is imp.		
	; bits 31-16 of ai*cr in AC3
	
	dbl(*AR1) = AC0	
	; save ar*cr in mlt
	||
	AC1 = AC1 << T2 
	; AC1 = ai*cr[63-32] << npostshifts
	
	AC3 = AC3 << T3
	; AC3 = ai*cr[31-16] >> (16-npostshifts)
	
	
	AC1 = AC1 | AC3 
	; AC0 = aicr[63-32]<<npostshifts | aicr[31-16] >> (16-npostshifts)	
	||
	if( AC2 > 0 ) execute(D_Unit)  ; if positive overflow due to shifting
		AC1 = dbl(*(#possat) )	   ; positively saturate AC1
	
	AC2 = AC2 + #1 	; to check for neg. underflow

	
	if(AC2 < 0 ) execute(D_Unit) 	; if negative overflow
		AC1 = dbl(*(#negsat))		; neg. saturation
	
	dbl(*AR5 ) = AC1
	; save ai*cr in mlt2
	
	;----------------------------------------------
	; current assignments
	; 
	;	AR0 -> ar_l
	;	AR1 -> mlt (ar*cr)
	;   AR2 -> cr_h
	;   AR3 -> ci_h
	;   AR4 -> ai_l
	;   AR5 -> mlt2 (ai*cr)
	; 	AR6 -> *****
	;	CDP -> ci_l
	;
	;   T0 -> nsamples (used for bit rev. indexing)
	;	T1 -> npostshifts - 31
	;   T2 -> npostshifts
	;   T3 -> npostshift - 16
	
	; multipy (ar * ci) :: (ai * ci)
	; to assign
	; AC0  -> ar*ci[63-32]
	; AC1 -> ai* ci[63-32]
	; AC2_L -> ar*ci[31-16]
	; AC3_L -> ar*ci[31-16]
	
	
	AR6 = #-1
	; AR6 -> 0xFFFF
	||
	; AR0 -> ar_l  
	; CDP-> ci_l 
	; AR4 -> ai_l			
	AC2 = uns(*AR0-) * uns(coef(*CDP)),
		AC3= uns(*AR4-) * uns( coef(*CDP) )
	; AC2 = ar_l * ci_l    
	; AC3 = ai_l * ci_l	   
	; AR0 -> ar_h
	; CDP -> ci_l
	; AR4 -> ai_h
	AC2 = (AC2 >> #16) + ( (*AR0+) * uns(coef(*CDP-) ) ),
		AC3 = (AC3 >> #16) + ( (*AR4+) * uns(coef(*CDP-) ) )
	; AC2 = (ar_h*ci_l) + (ar_l * ci_l) >> 16
	; AC3 = (ai_h*ci_l) + (ai_l * ci_l) >> 16
	; AR0 -> ar_l
	; CDP -> ci_h
	; AR4 -> ai_l
	AC2 = AC2 + ( uns(*AR0-) * coef(*CDP) ),
		AC3 = AC3 + ( uns(*AR4-) * coef(*CDP) )
	; AC2 = (ar_l*cr_h) + (ar_h*cr_l) + (ar_l * ci_l) >> 16   
	; AC3 = (ai_l*cr_h) + (ai_h*cr_l) + (ai_l * ci_l) >> 16
	; AR0 -> ar_h
	; CDP -> ci_h
	; AR4 -> ai_h
	AC0 = (*AR0) * coef(*CDP+),
		AC1 = (*AR4 ) * coef(*CDP+)
	;AC0 = ar_h * ci_h
	;AC1 = ai_h * ci_h
	; AR0 -> ar_h
	; CDP-> ci_l 
	; AR4 -> ai_h
	
	
	AC0 = AC0 + ( AC2 << #-16) || mar( *AR3-) ; fixsinptr--	****for next pass
	AC1 = AC1 + ( AC3 << #-16) || mar( *AR3-) ; AR3 = AR3 - 2 ********for next pass
	; AC0 = (ar_h*ci_h) + ((ar_l*ci_h) + (ar_h*ci_l) + (ar_l * ci_l) >> 16) >> 16
	; AC1 = (ai_h*ci_h) + ((ai_l*ci_h) + (ai_h*ci_l) + (ai_l * ci_l) >> 16) >> 16
	; AC2_L -> ar*ci[31-16]
	; AC3_L -> ai*ci[31-16]
	; AR0 -> ar_l
	; CDP -> ci_h
	; AR4 -> ai_l
	; AR6 -> 0xFFFF
	
	AC2 = AC2 & AR6				
	; isolate bits 31-16 of ar*ci
	|| 
	AR6 = LO(AC3) 	
	; save bits 31-16 of ai*ci in AR6
	
	AC3 = AC0 << T1
	; AC3 = ar*ci[63-32] >> ( 31-npostshifts) 
	||
	mar( *(AR0+T0B) )
	; AR0 -> next fft[idx] after bit reversal
	
	AC0 = AC0 << T2
	; ar*ci[63-32]  << npostshifts
	||
	AR4 = AR0					
	; modify AR4 = AR0 			*********** for next pass
	
	AC2 = AC2 << T3
	; AC2 = ar*ci[31-16] >> (16-npostshifts)
	||
	mar( AR4 + #2 )
	; modify AR4 = AR0 + #2		**************** for next pass
	
	AC0 = AC0 | AC2
	; AC0 = arci[63-32]<<npostshifts | arci[31-16] >> (16-npostshifts)	
	||
	if( AC3 > 0 ) execute(D_Unit)  ; if positive overflow due to shifting
		AC0 = dbl(*(#possat) )	   ; positively saturate AC0
	
	AC3 = AC3 + #1 	; to check for neg. underflow
		
	AC2 = AC1 << T1 
	; AC2 = ai*ci[63-32] >> (31-npostshifts)
	||
	if(AC3 < 0 ) execute(D_Unit) 	; if negative overflow
		AC0 = dbl(*(#negsat))		; neg. saturation
	
	; AC0 -> arci
	; mlt2(AR5) -> aicr
	AC0 = AC0 - dbl(*AR5)
	; AC0 -> arci - aicr

	
	AC3 = uns( *(_AR6)	)
	; bits 31-16 of ai*ci in AC3

	AC1 = AC1 << T2 
	; AC1 = ai*ci[63-32] << npostshifts
	
	dbl(*AR5 ) = AC0
	; mlt2 = arci - aicr
	||
	AC3 = AC3 << T3
	; AC3 = ai*ci[31-16] >> (16-npostshifts)
	
	
	AC1 = AC1 | AC3 
	; AC0 = aici[63-32]<<npostshifts | aici[31-16] >> (16-npostshifts)	
	||
	if( AC2 > 0 ) execute(D_Unit)  ; if positive overflow due to shifting
		AC1 = dbl(*(#possat) )	   ; positively saturate AC1
	
	AC2 = AC2 + #1 	; to check for neg. underflow
	||
	XCDP = XAR2						
	; cdp = cr_h for next pass
	; initialze to cosptr
	
	AR5 = AR5 - #4 
	; mlt2 = mlt2 - 4 
	||
	if(AC2 < 0 ) execute(D_Unit) 	; if negative overflow
		AC1 = dbl(*(#negsat))		; neg. saturation
	
	
	;AC1 -> ai*ci
	; mlt (AR1) -> ar*cr
	AC1 = dbl(*AR1) + AC1
	;AC1 = ar*cr + ai*ci.
	dbl(*AR1 ) = AC1
	; mlt = ar*cr + ai*ci
	AR1 = AR1 + #4
	; mlt = mlt + 4
	

	}
	
	
	return
	
	
; end of _RA_TNI_PostMultiply
;----------------------------------------------------------------------------------
		
		
		
		
		
		
;----------------------------------------------------------------------------------
; _RA_TNI_DecWindowNoOverlap : ( Not C-callable)
; 
;/*
; * Decode window, without overlap-add.
; * Returns pcm[2N] each call.
; *
; * Fixed-point changes:
; *  - arithmetic is now fixed-point
; *  - number of integer bits:
; *      input:  zbuf = POSTBITS
; *      output: pcm = POSTBITS
; */
;
;  Input : AR0 -> zbuf (long*)
;		: AR1 -> pcm (long*)
;		: T0 -> nsamples (short)
;
; Returns : void
;
; NOTE : modifies the pcm array, does not affect the zbuf array
; 
; Registers Touched : XAR0, XAR1, XAR2, XAR3, XAR4,XAR5, XAR6, XAR7, XCDP, AC0, AC1, AC2, AC3, T0,T1, T2, T3
;
; Globals Referenced : _RA_TNI_fixwindow_tab
;
; 

	.global _RA_TNI_fixwindow_tab		; const long *const RA_TNI_fixwindow_tab[3]
	.mmregs	
	
_RA_TNI_DecWindowNoOverlap:
	
	; prolog code

	.ARMS_off
	;---------------------------------------
	; To Assign
	; 
	; AR0 -> zbuf
	; AR1 -> pcm
	; AR2 -> zbuf2
	; AR3 -> pcm2
	; AR4 -> pcm3
	; AR5 -> pcm4
	; AR6 -> fixwnd
	; AR7 -> fixwnd2
	;
	; T0 -> nsamples
	; T1 -> 
	; T2 -> nmlt
	; T3 ->
	;
		.asg AR0 , zbuf
		.asg AR1 , pcm
		.asg AR2 , zbuf2
		.asg AR3 , pcm2
		.asg AR4 , pcm3
		.asg AR5 , pcm4
		.asg AR6 , fixwnd
		.asg AR7 , fixwnd2
	
_zbuf    .set 0010h 
_pcm     .set 0011h
_zbuf2   .set 0012h
_pcm2    .set 0013h
_pcm3    .set 0014h
_pcm4    .set 0015h
_fixwnd  .set 0016h
_fixwnd2 .set 0017h
_T0 	 .set 0020h 		
	
	 
	T1 = #2
	||
	T2 = T0

	T2 = T2 - #1 				
	; nmlt = nsamples - 1
	T2 = T2<<#1  
	; T2 = (nmlt<<1)
	; for dword addressing
	

	XAR3 = XAR1
	; pcm2 = pcm
	pcm2 = pcm2 + T2 
	; pcm2 = pcm + nmlt<<1
	
	XAR4 = XAR3 
	; pcm3 = pcm2
	pcm3 = pcm3 + T1
	; pcm3 = pcm2 + 2
	||
	mar(zbuf + T0 )
	; zbuf = zbuf + (nsamples>>1)<<1  {for dword addressing}
	
	
	XAR5 = XAR4
	; pcm4 = pcm3
	XAR2 = XAR0
	; zbuf2 = zbuf
		
	pcm4 = pcm4 + T2
	; pcm4 = pcm3 + nmlt<<1
	||
	mar(zbuf2 - T1)
	; zbuf2 = zbuf - 2
	
	XAR6 = _RA_TNI_fixwindow_tab
	; to calculate XAR6
	
	AC0 = uns(*(_T0))<<#-8
	; AC0 = (nsamples >>9 )<<1  for dword addressing
	bit( AC0, @0 ) = #0
	; to allow for dword addressing
	
	T1 = AC0 
	; T1 = (nsamples >> 9 ) << 1
		
	XAR6 = dbl(*AR6(T1))
	; fixwnd = fixwindow_tab[nsamples>>9]
	;************************************
	; ||	NOTE : Parallelizing these
	; 		two instructions causes the T0 >> 1 instruction
	;		to be skipped while executing
	;		No Problem while single stepping !!!!
	;*********************************
	T0 = T0 >> #1 
	;   nsamples >> 1

	
	XAR7 = XAR6 
	; fixwnd2 = fixwnd
	
	mar(T0 - #1)
	; T0 =  (nsamples>>1) - 1
	||
	T1 = #2
	
	

	;---------------------
	; allocate space on stack to store -(*fixwnd)
	;
	SP = SP- #5
	; NOTE : ensure dword alignment
	; 
	; SP -> -(*fixwnd)
	; SP(#2) -> fixwnd
	; 
	
	mar(fixwnd2 + T2 )
	;fixwnd2 = fixwnd + nmlt
	||
	BRC0 = T0 
	; load trip count in BRC0
	; do nsamples>>1 times

	XCDP = XAR2
	; CDP = zbuf2
	||
	blockrepeat{
	
		;-----------------------------------------
		; current assignments
		;
		; CDP -> zbuf2
		;
		; AR0 -> zbuf
		; AR1 -> pcm
		; AR2 -> zbuf2
		; AR3 -> pcm2
		; AR4 -> pcm3
		; AR5 -> pcm4
		; AR6 -> fixwnd
		; AR7 -> fixwnd2
		;
		; T0 -> *****
		; T1 -> 2
		; T2 -> *****
		; T3 -> ****
		
		
		
		; do multiplication
		; (zbuf2 * fixwnd) || ( zbuf2 * fixwnd2)
		; zbuf2 -- 
		; AC0 = hi ( zbuf2 * fixwnd)
		; AC2 = lo (zbuf2 * fixwnd) 
		; AC1 = hi (zbuf2 * fixwnd2)
		; AC3 = lo (zbuf2 * fixwnd2)
		
		
		; fixwnd -> fixwnd_h
		; fixwnd2 -> fixwnd2_h
		; CDP -> zbuf2_h
		
		
		AC0 = (*fixwnd+) * coef( *CDP+) ,
			AC1 = (*fixwnd2+) * coef(*CDP+)
			
		; AC0 = fixwnd_h * zbuf2_h
		; AC1 = fixwnd2_h * zbuf2_h 
		;
		; CDP -> zbuf2_l
		; fixwnd_l
		; fixwnd2_l
		AC2 = uns(*fixwnd-) * uns(coef(*CDP) ),
			AC3 = uns(*fixwnd2-) * uns(coef(*CDP) )
		; AC2 = fixwnd_l * zbuf2_l
		; AC3 = fixwnd2_l * zbuf2_l 
		;
		; CDP -> zbuf2_l
		; fixwnd_h
		; fixwnd2_h
		AC2 = (AC2 >> #16) + ( (*fixwnd+) * uns(coef(*CDP-)) ),
			AC3 = (AC3 >> #16) + ( (*fixwnd2+) * uns(coef(*CDP-)) )
		; AC2 = (fixwnd_h * zbuf2_l) + (fixwnd_l * zbuf2_l)>>16
		; AC3 = (fixwnd2_h * zbuf2_l) + (fixwnd2_l * zbuf2_l)>>16
		;
		; CDP -> zbuf2_h
		; fixwnd_l
		; fixwnd2_l	
		
		AC2 = AC2 + ( uns(*fixwnd-) * coef(*CDP) ),
			AC3 = AC3 + ( uns(*fixwnd2-) * coef(*CDP) )
			
		; AC2 = (fixwnd_l * zbuf2_h) + (fixwnd_h * zbuf2_l) + (fixwnd_l * zbuf2_l)>>16
		; AC3 = (fixwnd2_l * zbuf2_h) + (fixwnd2_h * zbuf2_l) + (fixwnd2_l * zbuf2_l)>>16
		;
		; CDP -> zbuf2_h
		; fixwnd_h
		; fixwnd2_h	
		
		
		AC0 = AC0 + (AC2 << #-16) || mar( *zbuf2-)
		AC1 = AC1 + (AC3 << #-16) || mar( *zbuf2-)
		; AC0 = (fixwnd_h * zbuf2_h) + ((fixwnd_l * zbuf2_h) + (fixwnd_h * zbuf2_l) + (fixwnd_l * zbuf2_l)>>16)>>16
		; AC1 = (fixwnd2_h * zbuf2_h)  + ( (fixwnd2_l * zbuf2_h) + (fixwnd2_h * zbuf2_l) + (fixwnd2_l * zbuf2_l)>>16)>>16
		; AC2_L -> bits 31-16 of product (fixwnd*zbuf2)
		; AC3_L -> bits 31-16 of product (fixwnd2*zbuf2)
		
		; zbuf2 = zbuf2 - 2
		
			
		XCDP = XAR0	
		; CDP = zbuf
		||
		AC0 = AC0 << #1  
		; right shift one (RA_TNI_MULSHIFT1) ( bits 63-32)
		AC2 = AC2 & 0xFFFF
		; isolate bits 31-16 of product
		
		dbl(*SP(#2) ) = XAR6
		; store fixwnd on stack
		||
		AC0 = AC0 | ( AC2 <<< #-15)
		; combine
		
		dbl(*pcm+  ) = AC0
		; pcm++ = zbuf2 * fixwnd
		||
		AC1 = AC1 << #1 
		; right shift one (RA_TNI_MULSHIFT1) ( bits 63-32)
		AC3 = AC3 & 0xFFFF 
		; isolate bits 31-16 of product
		AC1 = AC1 | ( AC3 <<< #-15)
		; combine
		||
		AC0 = dbl(*fixwnd) 
		; load *(fixwnd) in AC0
		
		dbl( *pcm2- ) = AC1
		; pcm2-- = zbuf2 * fixwnd2
		||
		AC0 = -AC0
		; calculate *(-fixwnd) 
		
		XAR6 = XSP 
		; make fixwnd point to -*(fixwnd) 
	
		dbl( *SP ) = AC0 
		; store -(*fixwnd) on stack
		
				
		;-----------------------------------------
		; current assignments
		;
		; CDP -> zbuf
		;
		; AR0 -> zbuf
		; AR1 -> pcm + 2
		; AR2 -> zbuf2 - 2
		; AR3 -> pcm2 - 2
		; AR4 -> pcm3
		; AR5 -> pcm4
		; AR6 -> fixwnd { -*(fixwnd) }
		; AR7 -> fixwnd2
		;
		; T0 -> *****
		; T1 -> 2
		; T2 -> ****
		; T3 -> ****
		
		
		; do multiplication
		; (zbuf * fixwnd) || ( zbuf * fixwnd2)
		; zbuf ++ 
		; AC0 = hi ( zbuf * -fixwnd)
		; AC2 = lo (zbuf *  -fixwnd) 
		; AC1 = hi (zbuf * fixwnd2)
		; AC3 = lo (zbuf * fixwnd2)
		
		
		; fixwnd -> fixwnd_h
		; fixwnd2 -> fixwnd2_h
		; CDP -> zbuf_h		
		AC0 = (*fixwnd+) * coef( *CDP+) ,
			AC1 = (*fixwnd2+) * coef(*CDP+)
		; AC0 = fixwnd_h * zbuf_h
		; AC1 = fixwnd2_h * zbuf_h 
		;
		; CDP -> zbuf_l
		; fixwnd_l
		; fixwnd2_l
		AC2 = uns(*fixwnd-) * uns(coef(*CDP) ),
			AC3 = uns(*fixwnd2-) * uns(coef(*CDP) )
		; AC2 = fixwnd_l * zbuf_l
		; AC3 = fixwnd2_l * zbuf_l 
		;
		; CDP -> zbuf_l
		; fixwnd_h
		; fixwnd2_h
		AC2 = (AC2 >> #16) + ( (*fixwnd+) * uns(coef(*CDP-)) ),
			AC3 = (AC3 >> #16) + ( (*fixwnd2+) * uns(coef(*CDP-)) )
		; AC2 = (fixwnd_h * zbuf_l) + (fixwnd_l * zbuf_l)>>16
		; AC3 = (fixwnd2_h * zbuf_l) + (fixwnd2_l * zbuf_l)>>16
		;
		; CDP -> zbuf_h
		; fixwnd_l
		; fixwnd2_l	
		
		AC2 = AC2 + ( uns(*fixwnd-) * coef(*CDP) ),
			AC3 = AC3 + ( uns(*fixwnd2-) * coef(*CDP) )
			
		; AC2 = (fixwnd_l * zbuf_h) + (fixwnd_h * zbuf_l) + (fixwnd_l * zbuf_l)>>16
		; AC3 = (fixwnd2_l * zbuf_h) + (fixwnd2_h * zbuf_l) + (fixwnd2_l * zbuf_l)>>16
		;
		; CDP -> zbuf_h
		; fixwnd_h
		; fixwnd2_h	
		
		
		AC0 = AC0 + (AC2 << #-16) || mar( *zbuf+)
		AC1 = AC1 + (AC3 << #-16) || mar( *zbuf+)
		; AC0 = (fixwnd_h * zbuf2_h) + ((fixwnd_l * zbuf2_h) + (fixwnd_h * zbuf2_l) + (fixwnd_l * zbuf2_l)>>16)>>16
		; AC1 = (fixwnd2_h * zbuf2_h)  + ( (fixwnd2_l * zbuf2_h) + (fixwnd2_h * zbuf2_l) + (fixwnd2_l * zbuf2_l)>>16)>>16
		; AC2_L -> bits 31-16 of product (fixwnd*zbuf2)
		; AC3_L -> bits 31-16 of product (fixwnd2*zbuf2)
		
		; zbuf = zbuf + 2
		
		
		XCDP = XAR2	
		; CDP = zbuf2
		; for next pass
		||
		AC0 = AC0 << #1  
		; right shift one (RA_TNI_MULSHIFT1) ( bits 63-32)
		AC2 = AC2 & 0xFFFF
		; isolate bits 31-16 of product
		AC0 = AC0 | ( AC2 <<< #-15)
		; combine
		||		
		XAR6 = dbl(*SP(#2) )
		; restore fixwnd from stack
				
		dbl(*pcm4- ) = AC0
		; pcm4-- = zbuf * -(*fixwnd)
		||
		AC1 = AC1 << #1 
		; right shift one (RA_TNI_MULSHIFT1) ( bits 63-32)
		AC3 = AC3 & 0xFFFF 
		; isolate bits 31-16 of product
		AC1 = AC1 | ( AC3 <<< #-15)
		; combine
		
		dbl( *pcm3+ ) = AC1
		; pcm3++ = zbuf * fixwnd2
		
		mar( fixwnd+T1 ) || mar( fixwnd2 - T1 ) 
		; fixwnd ++
		; fixwnd2 --
		
		
	}	
			
	;---------------------
	; deallocate space on stack used to store -(*fixwnd)
	;
	SP = SP + #5


	;-------------------------------------------------------
	; epilog code
	
	return
; end of _RA_TNI_DecWindowNoOverlap
;----------------------------------------------------------------------------------
		
		
		
		
		
;----------------------------------------------------------------------------------
; _RA_TNI_IMLTNoOverlap : C callable function
; 
;/*
; * Inverse MLT transform, without overlap-add.
; * mlt[N] contains input MLT data.
; * pcm[2N] returns output samples.
; *
; * Fixed-point changes:
; *  - buf is now int instead of float
; *  - number of integer bits:
; *      input:  mlt = MLTBITS
; *      output: pcm = POSTBITS
; */
; 
;  Input : AR0 -> mlt (long*)
;		: AR1 -> pcm (long*)
;		: AR2 -> buf (long*) 
;		: T0 -> nsamples (short)
;
; Returns : void
;
; NOTE : modifies all arrays
; 
; Registers Touched : XAR0, XAR1, XAR2, XAR3, XAR4,XAR5, XAR6, XAR7, XCDP, AC0, AC1, AC2, AC3, T0,T1, T2, T3
;
; Calls Functions :
; 		RA_TNI_PreMultipy
;		RA_TNI_FFT
; 		RA_TNI_PostMultiply
;		RA_TNI_DecWindowNoOverlap
		
	.global _RA_TNI_FFT

_RA_TNI_IMLTNoOverlap:
		
	; SP = odd 
	; prolog code
	push( @#03 ) ||mmap()		; save ST1	

	; Save On Entry
	pshboth(XCDP)		; save xcdp ,xar5, ac2, ac3, T2, T3
	||
	bit(ST2, #15) = #0
	; clear the ARMS bit to use DSP Mode addressing
	.ARMS_off
	
	pshboth(XAR5) 
	||
	bit(ST1, #10) = #1
	; set M40 bit
	; to allow 40 bit arithmetic
	pshboth(XAR6) 
	pshboth(XAR7) 
	dbl(push(AC2))
	dbl(push(AC3))
	push(T2, T3)
		
	; SP -> even
	;---------------------------------------	
	; allocate space on stack for in-params
	
	SP = SP - #8
	dbl(*SP)	= XAR0
	dbl(*SP(#2)) = XAR1
	dbl(*SP(#4)) = XAR2
	*SP(#6) = T0
	;
	; SP    -> XAR0 (mlt)
	; SP(2) -> XAR1 (pcm)
	; SP(4) -> XAR2 (buf)
	; SP(6) -> T0   (nsamples)
	;
	
	
	; XAR0 -> mlt
	; XAR1 -> pcm
	; T0 -> nsamples
	call _RA_TNI_PreMultipy
	; RA_TNI_PreMultipy( mlt, pcm, samples)
	
	XAR0 = dbl(*SP(#2))
	T0 = *SP(#6) 
	; XAR0 -> pcm
	; T0 -> nsamples
	call _RA_TNI_FFT 
	; RA_TNI_FFT( pcm, nsamples)
	
	XAR0 = dbl(*SP(#2) )
	XAR1 = dbl(*SP(#4) )
	T0 = *SP(#6)
	; XAR0 -> pcm
	; XAR1 -> buf
	; T0 -> nsamples
	call _RA_TNI_PostMultiply
	; RA_TNI_PostMultiply( pcm, buf, nsamples)
	
	
	XAR0 = dbl(*SP(#4) )
	XAR1 = dbl(*SP(#2) )
	T0 = *SP(#6)
	; XAR0 -> buf
	; XAR1 -> pcm
	; T0 -> nsamples
	call _RA_TNI_DecWindowNoOverlap
	; RA_TNI_DecWindowNoOverlap( buf, pcm, nsamples) 
	
			
	;---------------------------------------	
	;clean up stack
	
	SP = SP + #8
					
									
	;-------------------------------------------------------
	; epilog code
	
	T2, T3 = pop()
	AC3 = dbl(pop())
	AC2 = dbl(pop())
	XAR7 = popboth()
	XAR6 = popboth()
	XAR5 = popboth()
	XCDP = popboth()	
	||
	bit(ST2, #15) = #1
	; set the ARMS bit to enable control mode
	.ARMS_on
	@#03 = pop()||mmap()
	return	
		
		
; end of _RA_TNI_IMLTNoOverlap
;----------------------------------------------------------------------------------
	
		
	.endif	; CONST_TABLE_DMA	
			
	.endif	;__OPTIMISE_SECOND_PASS__














	
		
		
		
		
		
		
		
		