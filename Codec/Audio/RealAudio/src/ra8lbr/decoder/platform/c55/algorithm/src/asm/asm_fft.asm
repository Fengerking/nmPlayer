; ***** BEGIN LICENSE BLOCK *****   
; Source last modified: $Id: asm_fft.asm,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
; 	File 		:	assembly_FFT.asm 
;   Description : C55X assembly routines for functions present in Fft.c WITHOUT DMA support
;	
;------------------------------------------------------------------------------------------------------------
;/*
; * Gecko2 stereo audio codec.
; * Developed by Ken Cooke (kenc@real.com)
; * August 2000
; *
; * Last modified by:	Firdaus Janoos
; *						17/09/01
; *
; * Power-of-two FFT routines.
; */
	.mmregs

	.if __OPTIMISE_SECOND_PASS__
	.if !CONST_TABLE_DMA		; DMA disabled
	.def _RA_TNI_FFT		
	
	.global _RA_TNI_twidtabs		; defined in twidtabs.c
	
	
_SP   .set 18h
_T0   .set 20h
_T1   .set 21h
_T2   .set 22h
AC0H .set  09h
AC1H .set  0Ch
AC2H .set  25h
AC3H .set  29h
_BRC0 .set 1Ah
	
	
;----------------------------------------------------------------------------------
; _RA_TNI_FFT: ( Not C-callable)
; 
;/*
; * Forward FFT.
; * Input is fftbuf[2*nfft] in interleaved format.
; * Operates in-place.
; *
; * Fixed-point changes:
; *  - math is all fixed-point, rescaling done as we go
; *  - inner loop of fft butterfly made into inline function which can be
; *      rewritten in asm or mixed C/asm for speed
; *  - number of integer bits:
; *      input:  zfft = MLTBITS + 1
; *      output: zfft = MLTBITS + 1 + nfftlog2 (may exceed 32, but don't worry)
; */	
;
; Input : AR0 -> fftbuf (long*)
;		: T0 -> nsamples (short)
;
; Returns : void
;
; Registers touched : AR0, AR1, AR2, AR3, AR4, T0, T1, T2, BRC0
;						AC0 ; AC1, AC2, AC3


_RA_TNI_FFT:
	;--------------------------------------------------------
	; To assign
	; AR1 -> zptr1
	; AR2 -> zptr2 
	; AR3 -> zptr3
	; AR4 -> zptr4
	; T0 -> nsamples
	T0 = T0 >> #1 
	;T0(nfft) = (nsamples >> 2) << 1  for long addressing

	; set the cpl mode 
	bit( ST1, #14) = #1		; NOTE : This maynot be necessary
	.cpl_on				
	;notify the assembler

	bit (T0, @0) = #0		; set lsb of T0 to 0 - long aligned addressing

	XAR1 = XAR0 
	; zptr1 = fftbuf
	
	XAR2 = XAR1
	; zptr2 = zptr1
	AR2 = AR2 + T0 
	; zptr2 = zptr1 + (nfft<<1)

		
	XAR3 = XAR2
	; zptr3 = zptr2 
	AR3 = AR3 + T0 
	; zptr3 = zptr2 + (nfft<<1)
	
	XAR4 = XAR3
	; zptr4 = zptr3 

	
	; T0 = nsamples >> 1 
	AC0 = *(#_T0)<<#-2  ; AC0 = T0 >> #2
	; AC0 = nsamples >> 3
	T1 = AC0 - #1       ; (nsamples >> 3 )- 1
	BRC0 = T1
	; load trip count in BRC0
	||
	T1 = #4  		
	; T1 -> 4 for use in second loop

	AR4 = AR4 + T0 
	; zptr4 = zptr3 + (nfft<<1)
	;------------------------------------------------------------
	||
	; do nsamples >> 3 times  { BRC0 +1 times }
	blockrepeat{
		; AR0 -> zptr1
		; AR1 -> zptr2 
		; AR2 -> zptr3
		; AR3 -> zptr4
		; T0 -> nsamples >> 1
		; T1 -> 4
		
		; AC0 -> ar
		; AC1 -> cr
		; AC2 -> br
		; AC3 -> dr
		.asg AR1 , zptr1
		.asg AR2 , zptr2 
		.asg AR3 , zptr3
		.asg AR4 , zptr4
				
				
		AC0= dbl( *zptr1 )
		; load ar in AC0
		AC0 = AC0 >> #1
		; ar = *(zptr1)>>1
		||
		AC1 = dbl( *zptr3 )
		; cr in AC1
		AC1 = AC1 >> #1
	  	; cr = *(zptr3)>>1
	  	||
		AC2 = dbl( *zptr2 )
	  	; br in AC2
	  	
	  	AC3 = AC0     ; copy ar into AC3
	  	
	  	; calculate arcr_1
	  	AC0 = AC0 + AC1 		; ar + cr
	  	
	  	; calculate arcr_2 (ar -cr) >> 1
	  	AC3 = AC3 - AC1 			; ar -cr 
	  	AC3 = AC3 >> #1				; (ar - cr) >> 1
	  	
	  	AC0 = AC0 >> #1			
	  	; AC0 = (AC0 + AC1) >> 1
	  	||
	  	dbl(*zptr3) = AC3			
	  	; *(zptr3) = (AC0 - AC1) >> 1 
	  								
	  	AC2 = AC2 >> #1
	  	; br = *(zptr2)>>1
	  	||
	  	AC3 = dbl( *zptr4 )
	  	; dr in AC3
	  	AC3 = AC3 >> #1 
	  	; dr = (*zptr4) >> 1
	  	
	  	AC1 = AC2	; copy br into AC1
	  	
	  	; calculate brdr_2 
	  	AC1 = AC1 - AC3			; br - dr 
	  	AC1 = AC1 >> #1			; (br - dr ) >> 1
	  	dbl(*zptr4) = AC1		; parallelise with AC2 = AC2 >> 1
	  	; *(zptr4) = (AC2 - AC3) >> 1 
	  	||	
	  	; calculate brdr_1 
	  	AC2 = AC2 + AC3			; br + dr
	  	AC2 = AC2 >> #1         
	  	; AC2 = (AC2 + AC3) >> 1	  	
	  	
		; AC0 -> arcr_1
		; AC1 -> brdr_2
		; AC2 -> brdr_1
		; AC3 -> dr 
		; *zptr3 -> arcr_2
		; *zptr4 -> brcr_2
		
		; calculate  arcr_1 + brdr_1  
		AC3 = AC0 + dbl(*(#AC2H)) 
		; AC3 = AC0 + AC2
		dbl(*zptr1+) = AC3			; store arcr_1 + brdr_1
		; zptr1 -> zptr1 + 1 
		||
		; calculate  arcr_1 -brdr_1  
		AC0 = AC0 - AC2 
		; AC3 = AC0 + AC2
		dbl(*zptr2+) = AC0			; store arcr_1 - brdr_1
		; zptr2 -> zptr2 + 1 
		
		; AC0 -> ******
		; AC1 -> ******
		; AC2 -> ******
		; AC3 -> ******
		; *zptr1 -> ai
		; *zptr2 -> bi
		; *zptr3 -> arcr_2
		; *zptr4 -> brdr_2
				
		AC1 = dbl( *zptr3(#2) )
		; ci in AC1
		AC1 = AC1 >> #1 ;
		; ci = *(zptr3 + 2 ) >> 1
		||
		AC0= dbl( *zptr1 )
		; load ai in AC0
		AC0 = AC0 >> #1
		; ai = *(zptr1 + 2) >> 1
		||
		AC2 = dbl( *zptr2 ) 
		; bi in AC2
		AC2 = AC2 >> #1 
		; bi = *(zptr2 + 2) >> 1
				
	  	; calculate aici_2   ( ai - ci ) >> 1
	  	AC3 = dbl( *(#AC0H) ) - AC1    ; AC3 = ai - ci
	  	AC3 = AC3 >> #1 				  ; ( ai - ci ) >> 1	
	  	dbl(*zptr3(#2) ) = AC3    		 
	  	;*(zptr3+2) =  ( AC0 - AC1 ) >> 1	
	  	
	  	; calculate aici_1   ( ai + ci ) >> 1
	  	AC0 = AC0 + AC1	   ; AC0 = ai + ci
	  	AC0 = AC0 >> #1 				
	  	; AC0 = (AC0 + AC1) >> 1     
	  	
	  	; AC0 -> aici_1
		; AC1 -> ******
		; AC2 -> bi 
		; AC3 -> aici_2
		; *zptr1 -> ai
		; *zptr2 -> bi
		; *zptr3 -> arcr_2
		; *zptr4 -> brdr_2
		
	  	AC3 = dbl( *zptr4(#2) )	
	  	; di in AC3
	  	AC3 = AC3 >> #1
	  	; di = *(zptr4 + 2 ) >> 1
	  	
	  	; AC0 -> aici_1
		; AC1 -> *****
		; AC2 -> bi 
		; AC3 -> di
		
		; calculate bidi_2 (bi - di ) >> 1
	  	AC1 = AC2 - dbl(*(#AC3H) )		; AC2 - AC3 
	  	AC1 = AC1 >> #1 				; 
	  	; AC1 = (AC2-AC3) >> #1   
	  	
	  	; calculate bidi_1 ( bi + di ) >> 1
	  	AC2 = AC2 + AC3					; bi + di
	  	AC2 = AC2 >> #1
	  	; AC2 = (AC2 + AC3) >> 1
	  	
	  	; AC0 -> aici_1
		; AC1 -> bidi_2
		; AC2 -> bidi_1 
		; AC3 -> *****
		
	  	; calculate aici_1 + bidi_1
	  	AC3 = AC0 + dbl( *(#AC2H) ) 		
	  	dbl( *zptr1+) = AC3  	; store aici_1 + bidi_1 in zptr+2
	  	; zptr1 = zptr1 + 4 
	  	
	  	; calculate aici_1 - bidi_1
	  	||
	  	AC0 = AC0 - AC2 		
	  	dbl( *zptr2+) = AC0  	; store aici_1 - bidi_1 in zptr2+2
	  	; zptr2 = zptr2 + 4 
	  	
	  	; AC0 -> *****
		; AC1 -> bidi_2
		; AC2 -> *****
		; AC3 -> *****
		; *zptr1 -> *zptr1 + 4
		; *zptr2 -> * zptr2 + 4 
		; *zptr3 -> arcr_2
		; *zptr4 -> brdr_2
	  	; *zptr3(2) -> aici_2 
	  	
	  	AC0 = dbl( *zptr3(#2) ) 		; AC0 -> aici_2
	  	
	  	AC3 = dbl( *zptr3 ) + AC1		; calculate arcr_2 + bidi_2
	  	AC2 = dbl( *zptr3 ) - AC1 		;calculate arcr_2 - bidi_2
	  	
	  	dbl( *zptr3+  ) = AC3 ;
	  	; *(zptr3) = arcr_2 + bidi_2 
	  	; zptr3 -> zptr3 + 2
	  	
	  	AC3 = AC0 + dbl(*zptr4)  	; aici_2 + brdr_2
	  	
	  	AC0 = AC0 - dbl(*zptr4) 	; aici_2 - brdr_2
	  	dbl( *zptr3+ ) = AC0
	  	; *(zptr3 + 2) = aici_2 + brdr_2 
	  	; zptr3 + 2 -> zptr3 + 4
	  	
	  		  	
	  	dbl( *zptr4+ ) = AC2			
	  	; *(zptr4 ) = arcr_2 - bidi_2 
	  	; zptr4 -> zptr4 + 2
	  	dbl( *zptr4+ ) = AC3	   ;
	  	; *(zptr4 + 2) = aici_2 + brdr_2 
		; zptr4 + 2 -> zptr4 + 4
	  	
	  	; AC0 -> *****
		; AC1 -> *****
		; AC2 -> *****
		; AC3 -> *****
		; *zptr1 -> *zptr1 + 4
		; *zptr2 -> * zptr2 + 4 
		; *zptr3 -> *zptr3 + 4
		; *zptr4 -> *zptr4 + 4
	  	
	  		
	}
	
	
	;----------------------------------------------------------------------------------------------
	; SECOND loop
	
	; T0 -> nsamples >> 1
	; T1 -> 4
	; XAR0 -> fftbuf
	; AC0 -> *****
	; AC1 -> *****
	; AC2 -> *****
	; AC3 -> *****
	
	repeat( #2 ) 		; 3 iterations
		T0 = T0 >> #1 			
	; T0 = nsamples >> #4 
	
	T2 = T0  ; bg
	; T2 = nsamples >> #4 
	||
	AC0 = T0
	AC0 = AC0<<#-4
	T0 = AC0
	; T0 = nsamples >> #8
			
	bit (T0, @0) = #0		; set lsb of T0 to 0 - long aligned addressing
	; T0 = (nsamples >> #9)<<#1 
		
	XAR1 = _RA_TNI_twidtabs			
	;const long *const RA_TNI_twidtabs[3] = { twidtab_0, twidtab_1, twidtab_2 };
	
	XAR1 = dbl(*AR1(T0) ) 
	; XAR1 -> RA_TNI_twidtabs[ (nsamples>>9) << 1]
	; XAR1 ->ztwidtab

	; T0 -> nsamples >> 8
	; T1 -> 4  (gp)
	; T2 -> bg ( nsamples >>4)
	; XAR0 -> fftbuf
	; XAR1 -> ztwidtab
	;
	; AC0 -> *****
	; AC1 -> *****
	; AC2 -> *****
	; AC3 -> *****
	
	
	;/***************************/
	; || 			// Issue on hardware. * needs to be confirmed*
	;				// Executing with || enabled
	;				// loads incorrect value in 

	;/****************************/	
	
	; to setup the stack
	
	SP = SP - #5
	; note alignment issues
	
	;------------------------------------
	; SP + 0 : zfftbuf
	; SP + 2 : ztwidtab
	;----------------------------------
	
	
	
	dbl( *SP(#0) ) = XAR0   ; zfftbuf
	; set up the trip count in T0
	;/***************************/
	; || 			// Issue on hardware. To confirm
	;				// Executing with || enables
	;				// causes distorted value in T0, BRC0
	;				// No prb while single stepping
	;/****************************/	
	T0 = T0 >> #1 
	; T0 = nsamples >> #9
					
	AR7 = T1			    
	; gp in AR7
	||
	mar( T0 + T1 ) 
	; T0 = (nsamples >> 9) + 4   
	
	
	dbl( *SP(#2) ) = XAR1   ; ztwidtab
	||
	BRC0 = T0 
	; load trip count in BRC0 

	blockrepeat {	

		;------------------------------------------------
		; to assign
		;
		; XAR0 -> zptr1 for RA_TNI_BUTTERFLY 
		; XAR1 -> zptr2 for RA_TNI_BUTTERFLY
		; XAR2 -> bgOffset
		; XAR3 -> *** used in RA_TNI_BUTTERFLY
		; XAR4 -> zptr1
		; XAR5 -> zptr2
		; XAR6 -> ztwidptr
		; AR7 -> gp
		; XCDP -> ztwidptr in RA_TNI_BUTTERFLY
		;
		; T0 -> bg for RA_TNI_BUTTERFLY , bgOffset afterwards
		; T1 -> inner loop count 
		; T2 -> bg 
		;------------------------------------------------

		XAR4 = dbl(*SP(#0))
		; AR4 (zptr1) = zfftbuf
		
		AC0 = XAR4		
		; AC0 = fftbuf
		AC0 = AC0 + ( *(#_T2)<<#2 ) 		
		; fftbuf + (T2 <<1) <<1		 long pointers
		XAR5 = AC0				
		;XAR5-> zptr2
		
		XAR6 = dbl(*SP(#2))
		; ztwidptr = ztwidtab
		||
		AC0 = T2
		AC0 = AC0 << #3 ; left shift 2 + 1 times
		AR2 = AC0 
		; AR2 = (bg << 2) << 1   
		; AR2 -> bgOffset for dword addressing
		
		;------------------------------------------------
		; current assignments
		;
		; XAR0 -> ****
		; XAR1 -> ****
		; XAR2 -> bgOffset
		; XAR3 -> ****
		; XAR4 -> zptr1
		; XAR5 -> zptr2
		; XAR6 -> ztwidptr
		; AR7 -> gp
		; XCDP -> ztwidptr in RA_TNI_BUTTERFLY
		;
		; T0 -> ******
		; T1 -> gp
		; T2 -> bg 
		;
		; SP + 0 : zfftbuf
		; SP + 2 : ztwidtab
		;------------------------------------------------
		
		;***************************************************************
		; 
		;	First pass of the RA_TNI_BUTTERFLY_LOOP is unrolled
		; This is because the 'ci' coeff. of the twiddlefactors is zero, hence computation
		; does not need to be performed
		
				
		XAR0 = XAR4		
		; zptr1 for RA_TNI_BUTTERFLY
		; NOTE : one can use AR not XAR because page ptrs 
		; have been initialized earlier 

		XAR1 = XAR5					; however in this case it in not applicable, as XAR1
		; zptr2 for RA_TNI_BUTTERFLY		; point to the twidle tabs , in a different page						
		; ztwidptr for RA_TNI_BUTTERFLY
		; same logic
		XCDP = XAR6

		;--------------------------------------------------------------------------
		; ******* INLINING THE RA_TNI_BUTTERFLY ROUTINE
		
		
			
		;AR0 -> Re {zptr1}				; ar
		;AR0(2) -> Im {zptr1 }			; ai
		;AR1 -> Re {zptr2}				: br
		;AR1(2) -> Im {zptr2 }			: bi (AR3)
		;CDP -> Re {ztwidptr}			: cr /CDP
		;CDP(2) -> Im {ztwidptr }		: ci /CDP(2)
		
	
		bit(ST1, #10) = #1 ; set M40 bit	; this issue was
							; solved after 1 and 1/2 days of debugging
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																				
		T0 = T2 - #1
		; load bg - 1 in T0
	
		BRC1 = T0 	; repeat bg - 2 times
		||
		T0 = #2			;increment factor
	
		XAR3 = XAR1 ;	
	
		; XAR not AR - since the pages
		; matter -- this caused a problem
		; solved on 12/9/01 at 2300hrs
		AR3 = AR1 + #2	; AR3 = bi
		
		
		blockrepeat {
			; AR1 : br
			; AR3 : bi
			; CDP : cr
			
			; (AR1 *CDP + (AR3* CDP(2) )
			; (AR1 *CDP(2) ) - (AR3 *CDP )
			;
			; multiply AC0 = br * cr :: AC1 = bi * cr 
			
			; AR1 -> br_h  
			; CDP -> cr_h 
			; AR3 ->bi_h
						
			AC0 = (*AR1+) * coef(*CDP+) ,	
				AC1 = (*AR3+) * coef(*CDP+) 	;
			; AR1 -> br_l  
			; CDP-> cr_l 
			; AR2 -> bi_l			
			; AC0 = br_h*cr_h  , AC1 = bi_h * cr_h
			
			AC2 = uns(*AR1-) * uns(coef(*CDP) ),
				AC3 =  uns(*AR3-) * uns(coef(*CDP))
			; AR1 -> br_h 
			; CDP->cr_l 
			; AR3 ->bi_h
			; AC2 = br_l*cr_l  , AC3 = bi_l*cr_l
			
			AC2 = (AC2 >>#16) + ( (*AR1+) * uns(coef(*CDP-)) ),
				AC3 = (AC3 >>#16) + ( (*AR3+) * uns(coef(*CDP-)) )
			; AR1 -> br_l
			; CDP -> cr_h
			; AR3-> bi_l
			; AC2 -> (br_h * cr_l) + ( br_l*cr_l)>>16
			; AC3 -> (bi_h * cr_l) + ( bi_l*cr_l)>>16
			AC2 = AC2 + ( uns(*AR1-) * coef(*CDP) ),
				AC3 = AC3 + ( uns(*AR3-) * coef(*CDP) )
			; AR1 -> br_h 
			; CDP - > cr_h
			; AR3->bi_h
			; AC2 -> (br_l*cr_h) + (br_h * cr_l) + ( br_l*cr_l)>>16
			; AC3 -> (bi_l*cr_h) + (bi_h * cr_l) + ( bi_l*cr_l)>>16
			
			 
			AC0 = AC0 + (AC2<<#-16) 
			||
		    AR3 = AR3 + #4	; AR3 = bi
		    ; advance bi to bi + 1 
		    
			AC1 = AC1 + ( AC3<<#-16)

			; AC0 (br*cr) -> (br_h*cr_h) + ((br_l*cr_h) + (br_h * cr_l) + ( br_l*cr_l)>>16) >>16
			; AC1 (bi*cr)-> (bi_h*cr_h) + ((bi_l*cr_h) + (bi_h * cr_l) + ( bi_l*cr_l)>>16) >>16
								
			; ci = 0 hence br*ci and bi*ci do not need to be calculated
			; AR1 -> br_h 
			; CDP-> cr_h
			; AR3->bi_h
			
			
			; imaginary part = - AC1(bi*cr) ; itemp
			AC2 = - AC1
			; AC0 ->rtemp
			; AC2 ->itemp
			||
			AC1 = dbl(*AR0) 		; AC1 -> ar 	
			AC1 = AC1>> #1			; ar = ar >> 1
			
			AC3 = AC1
			AC1 = AC1 + AC0  ; ar + rtemp
			AC3 = AC3 - AC0  ; ar - rtemp
			||
			dbl(*(AR0+T0)) = AC1       ; zptr1+0 = ar + rtemp
			dbl(*(AR1+T0))= AC3		  ; zptr2+0 = ar - rtemp
			; AR0 -> zptr1+2 (img part)
			; AR1 -> zptr2+2 (img part)
			
			AC3 = dbl(*AR0)		; AC3 -> ai
			AC3 = AC3>> #1 			; ai = ai >> 1
			
			AC1 = AC3		
			AC3 = AC3 - AC2  ; ai - itemp
			AC1 = AC1 + AC2  ; ai + itemp
			||
			dbl(*(AR0+T0)) = AC3	  ; zptr1+2 = ai - itemp
			dbl(*(AR1+T0)) = AC1	  ; zptr2+2 = ai + itemp
			
			; AR0 -> real part of next complex no. in zptr1
			; AR1 -> real part of next complex no. in zptr2
			; CDP -> cr_h
			
					
		}
	
		
		; end of RA_TNI_BUTTERFLY routine
		;***************************************************************************
		; XAR0,1, & 3 are destroyed
		; AR2 -> bgOffset
			
		T0 = AR2
		; T0 -> bgOffset
		; T2 -> bg
		; AR2 -> bgOffset
		;
		
		mar( *(AR4+T0) )		; zptr1 + bgOffset
		||
		mar( *(AR5+T0) )		; zptr2 + bgOffset
				
		AR6 = AR6 + #4  		;ztwidptr += 4
		||
		mar( T1 - #1 )			; loop counter 
								; reduce by one pass
			
		; End of unrolled pass of RA_TNI_BUTTERFLY_LOOP
		;************************************************************************************
		; Execute the remaining RA_TNI_BUTTERFLY passes within the loop
		; br*ci and bi*ci must be computed
		
RA_TNI_BUTTERFLY_LOOP:
		
		XAR0 = XAR4		
		; zptr1 for RA_TNI_BUTTERFLY
		; NOTE : one can use AR not XAR because page ptrs 
		; have been initialized earlier 
	
		XAR1 = XAR5					; however in this case it in not applicable, as XAR1
		; zptr2 for RA_TNI_BUTTERFLY		; point to the twidle tabs , in a different page						
		; ztwidptr for RA_TNI_BUTTERFLY
		; same logic
		XCDP = XAR6
		
		;--------------------------------------------------------------------------
		; ******* INLINING THE RA_TNI_BUTTERFLY ROUTINE
		
			;AR0 -> Re {zptr1}				; ar
		;AR0(2) -> Im {zptr1 }			; ai
		;AR1 -> Re {zptr2}				: br
		;AR1(2) -> Im {zptr2 }			: bi (AR3)
		;CDP -> Re {ztwidptr}			: cr /CDP
		;CDP(2) -> Im {ztwidptr }		: ci /CDP(2)
		
		T0 = T2 - #1
		; load bg - 1 in T0
		BRC1 = T0 	; repeat bg - 2 times
		AR3 = AR1 + #2	; AR3 = bi			

		blockrepeat {
			;AR3 = AR1 + #2	; AR3 = bi
			; AR1 : br
			; AR3 : bi
			; CDP : cr
			
			; (AR1 *CDP + (AR3* CDP(2) )
			; (AR1 *CDP(2) ) - (AR3 *CDP )
			;
			; multiply AC0 = br * cr :: AC1 = bi * cr 
			
			; AR1 -> br_h  
			; CDP -> cr_h 
			; AR3 ->bi_h
						
			AC0 = (*AR1+) * coef(*CDP+) ,	
				AC1 = (*AR3+) * coef(*CDP+) 	;
			; AR1 -> br_l  
			; CDP-> cr_l 
			; AR2 -> bi_l			
			; AC0 = br_h*cr_h  , AC1 = bi_h * cr_h
			|| 
			T0 = #2			;increment factor
			
			
			AC2 = uns(*AR1-) * uns(coef(*CDP) ),
				AC3 =  uns(*AR3-) * uns(coef(*CDP))
			; AR1 -> br_h 
			; CDP->cr_l 
			; AR3 ->bi_h
			; AC2 = br_l*cr_l  , AC3 = bi_l*cr_l
			
			AC2 = (AC2 >>#16) + ( (*AR1+) * uns(coef(*CDP-)) ),
				AC3 = (AC3 >>#16) + ( (*AR3+) * uns(coef(*CDP-)) )
			; AR1 -> br_l
			; CDP -> cr_h
			; AR3-> bi_l
			; AC2 -> (br_h * cr_l) + ( br_l*cr_l)>>16
			; AC3 -> (bi_h * cr_l) + ( bi_l*cr_l)>>16
			
			
			AC2 = AC2 + ( uns(*AR1) * coef(*CDP+) ),
				AC3 = AC3 + ( uns(*AR3) * coef(*CDP+) )
			; AR1 -> br_l 
			; CDP - > cr_l
			; AR3->bi_l
			; AC2 -> (br_l*cr_h) + (br_h * cr_l) + ( br_l*cr_l)>>16
			; AC3 -> (bi_l*cr_h) + (bi_h * cr_l) + ( bi_l*cr_l)>>16
			
			 
			AC0 = AC0 + (AC2<<#-16) || mar( *CDP+)
			AC1 = AC1 + ( AC3<<#-16)|| mar (*CDP+)
			; AC0 (br*cr) -> (br_h*cr_h) + ((br_l*cr_h) + (br_h * cr_l) + ( br_l*cr_l)>>16) >>16
			; AC1 (bi*cr)-> (bi_h*cr_h) + ((bi_l*cr_h) + (bi_h * cr_l) + ( bi_l*cr_l)>>16) >>16
								
			;to calculate : (br*cr) + #(bi*ci)#
			;	and	   : #(br*ci)# - (bi*cr)
				
			; CDP -> cr_l+2 -> ci_l
			; AR1 -> br_l
			; AR3 -> bi_l
			; multiply AC2 = br*ci :: AC3 = bi*ci
			
			AC2 = uns(*AR1-) * uns(coef(*CDP) ),
				AC3 =  uns(*AR3-) * uns(coef(*CDP))
			; AR1 -> br_h 
			; CDP->ci_l 
			; AR3 ->bi_h
			; AC2 = br_l*ci_l  
			; AC3 = bi_l*ci_l
				
			AC2 = (AC2 >>#16) + ( (*AR1+) * uns(coef(*CDP-)) ),
				AC3 = (AC3 >>#16) + ( (*AR3+) * uns(coef(*CDP-)) )
			; AR1 -> br_l
			; CDP -> ci_h
			; AR3-> bi_l
			; AC2 -> (br_h * ci_l) + ( br_l*ci_l)>>16
			; AC3 -> (bi_h * ci_l) + ( bi_l*ci_l)>>16
			
			AC2 = AC2 + ( uns(*AR1-) * coef(*CDP) ),
				AC3 = AC3 + ( uns(*AR3-) * coef(*CDP) )
			; AR1 -> br_h 
			; CDP -> ci_h
			; AR3->bi_h
			; AC2 -> (br_l*ci_h) + (br_h * ci_l) + ( br_l*ci_l)>>16
			; AC3 -> (bi_l*ci_h) + (bi_h * ci_l) + ( bi_l*ci_l)>>16
			
			AC2 = (AC2 >>#16) + ( (*AR1) * coef(*CDP-) ),
				AC3 = (AC3>>#16) + ( (*AR3) * coef(*CDP-) )
			; AR1 -> br_h 
			; CDP-> cr_l
			; AR3->bi_h
			; AC2 (br*ci) -> (br_h*ci_h) + ((br_l*ci_h) + (br_h * ci_l) + ( br_l*ci_l)>>16) >>16
			; AC3 (bi*ci)-> (bi_h*ci_h) + ((bi_l*ci_h) + (bi_h * ci_l) + ( bi_l*ci_l)>>16) >>16
			||
			AR3 = AR3 + #4	; AR3 = bi	
		    ; advance bi to bi + 1 
		    		
			; imaginary part = AC2(br*ci) - AC1(bi*cr)
			AC2 = AC2 - AC1 || mar(*CDP-)	
			; CDP-> cr_h
			
			; real part = AC0(br*cr) + 	AC3(bi*ci)
			AC0 = AC0 + AC3 
			; AC0 ->rtemp
			; AC2 ->itemp
			||
			AC1 = dbl(*AR0) 		; AC1 -> ar 	
			AC1 = AC1>> #1			; ar = ar >> 1
			
			AC3 = AC1
			AC1 = AC1 + AC0  ; ar + rtemp
			AC3 = AC3 - AC0  ; ar - rtemp
			||
			dbl(*(AR0+T0)) = AC1       ; zptr1+0 = ar + rtemp
			dbl(*(AR1+T0))= AC3		  ; zptr2+0 = ar - rtemp
			; AR0 -> zptr1+2 (img part)
			; AR1 -> zptr2+2 (img part)
			
			AC3 = dbl(*AR0)		; AC3 -> ai
			AC3 = AC3>> #1 			; ai = ai >> 1
			
			AC1 = AC3		
			AC3 = AC3 - AC2  ; ai - itemp
			AC1 = AC1 + AC2  ; ai + itemp
			||
			dbl(*(AR0+T0)) = AC3	  ; zptr1+2 = ai - itemp
			dbl(*(AR1+T0)) = AC1	  ; zptr2+2 = ai + itemp
			
			
			
			; AR0 -> real part of next complex no. in zptr1
			; AR1 -> real part of next complex no. in zptr2
			; CDP -> cr_h
			
					
		}
	
		; end of RA_TNI_BUTTERFLY routine
		;***************************************************************************
		; XAR0,1, & 3 are destroyed
		; AR2 -> bgOffset
			
		T0 = AR2
		; T0 -> bgOffset
		; T2 -> bg
		; AR2 -> bgOffset
		;
		
	
		AR6 = AR6 + #4  		;ztwidptr += 4
		||
		mar( T1 - #1 )			; loop counter
	
		mar( *(AR4+T0) )		; zptr1 + bgOffset
		||
		mar( *(AR5+T0) )		; zptr2 + bgOffset
					
		if( T1 != 0 )	goto RA_TNI_BUTTERFLY_LOOP	; repeat loop 4 times
				
		;--------end of inner loop----------
		
	;	XCDP = popboth()
		; restore XCDP
		
		;------------------------------------------------
		; current assignments
		;
		; XAR0 -> ****
		; XAR1 -> ****
		; XAR2 -> *****
		; XAR3 -> ****
		; XAR4 -> zptr1
		; XAR5 -> zptr2
		; XAR6 -> ztwidptr
		; XCDP -> ztwidptr in RA_TNI_BUTTERFLY
		; AR7 -> gp
		;
		; T0 -> bgOffset
		; T1 -> 0
		; T2 -> bg 
		;
		; SP + 0 : zfftbuf
		; SP + 2 : ztwidtab
		;------------------------------------------------
		
		T2 = T2 >> #1		; bg = bg>>#1	
		AR7 = AR7 << #1		; gp = gp<<#1 
		; reload T1
		T1 = AR7
			
	}
	
	; clean up code and return
	
	; deallocate locals
	SP = SP + #5
	
	return
	
	





;------------------------------------------------------------------------------------------------------------
;------------------------------------------------------------------------------------------------------------





	.endif	; CONST_TABLE_DMA		 DMA disabled	
	
	.endif ; __OPTIMISE_SECOND_PASS__