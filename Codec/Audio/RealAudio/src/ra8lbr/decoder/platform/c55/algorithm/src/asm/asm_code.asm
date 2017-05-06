; ***** BEGIN LICENSE BLOCK *****   
; Source last modified: $Id: asm_code.asm,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
; 	File 		:	assembly_code.asm 
;   Description : C55X assembly code versions of functions present in Assembly.c
;	
;------------------------------------------------------------------------------------------------------------
		
	.mmregs
	.if __OPTIMISE_FIRST_PASS__
	
	
	.def _RA_TNI_MULSHIFT1
		

	.text
	 
	.if !DECODEVECTORS_ASM   ; otherwise RA_TNI_MULSHIFT0 not needed

	.def _RA_TNI_MULSHIFT0
	;----------------------------------------------------------------------------------
	; _RA_TNI_MULSHIFT0 : C callable function
	; 
	;		
	; to multiply 2x32 bit signed nos. and shift left by 0
	; Input : AR0 -> xh xl
	; Input : AR1-> yh yl
	; return AC0 -> shifted product
	;
	
	
_RA_TNI_MULSHIFT0:
		; AR0 -> xh xl
		; AR1-> yh yl
		
		; AC0-> upper 32-bits
		; AC3-> lower 32-bits
		; T0 -> h
		; perform multiplication
		; copy into memory 
		
		;dbl(push(AC1))		; XSP points to AC1 (31-16)
		;||XAR1 = XSP 		; XAR1 points to AC1 (31-16)
		
		;dbl( push(AC0) )	
		;||XAR0 = XSP		; XAR0 points to AC0 (31-16)
		
		mar( *AR0+  ) || mar( *AR1+ )	; modify AR0 and AR1
		; AR0 -> xl
		; AR1 -> yl
		
		AC3 = uns( *AR0- ) * uns( *AR1 )	; xl * yl
		; AR0 -> xh
		; AR1 -> yl
		
		; AC3 = (xl*yl)>>16  +  xh*yl + xl * yh 
		 AC3 = (AC3 >> #16) + ( (*AR0+) * uns(*AR1-) )	; xh*yl
		; AR0 -> xl
		; AR1 -> xh
		AC3 = AC3 + ( uns(*AR0-) * (*AR1) ) 	;xl*yh
		
		;AC0 = AC3>>16 + xh*yh
		AC0 = (AC3>>#16 ) + ( (*AR0) * (*AR1) )   ; xh*yh
		
		;AC2 = dbl(pop())  
		;AC1 = dbl(pop())
		return                                                         
		
		
	   .endif ;!DECODEVECTORS_ASM   ; otherwise RA_TNI_MULSHIFT0 not needed
	;----------------------------------------------------------------------------------
	; _RA_TNI_MULSHIFT1 : C callable function
	; 
	;		
	; to multiply 2x32 bit signed nos. and shift left by 1
	; Input : AR0 -> xh xl
	; Input : AR1-> yh yl
	; return AC0 -> shifted product
	;
	
_RA_TNI_MULSHIFT1:

		; AR0 -> xh xl
		; AR1-> yh yl
		
		; AC0-> upper 32-bits
		; AC3-> lower 32-bits
		; T0 -> h
		; perform multiplication
		; copy into memory 
		
		;dbl(push(AC1))		; XSP points to AC1 (31-16)
		;||XAR1 = XSP 		; XAR1 points to AC1 (31-16)
		
		;dbl( push(AC0) )	
		;||XAR0 = XSP		; XAR0 points to AC0 (31-16)
		
		mar( *AR0+  ) || mar( *AR1+ )	; modify AR0 and AR1
		; AR0 -> xl
		; AR1 -> yl
		
		AC3 = uns( *AR0- ) * uns( *AR1 )	; xl * yl
		; AR0 -> xh
		; AR1 -> yl
		
		; AC3 = (xl*yl)>>16  +  xh*yl + xl * yh 
		 AC3 = (AC3 >> #16) + ( (*AR0+) * uns(*AR1-) )	; xh*yl
		; AR0 -> xl
		; AR1 -> xh
		AC3 = AC3 + ( uns(*AR0-) * (*AR1) ) 	;xl*yh
		
		;AC0 = AC3>>16 + xh*yh
		AC0 = (AC3>>#16 ) + ( (*AR0) * (*AR1) )   ; xh*yh
		
		;AC2 = dbl(pop())  
		;AC1 = dbl(pop())
		;shifting
		AC3 = AC3 & 0xFFFF
		AC0  = AC0<<#1 
		AC0 = AC0|(AC3 <<< #-15)
		
		return 


	.if !INTERP_ASM				; needed only in C-code of Interpolate() 
	;----------------------------------------------------------------------------------
	; _RA_TNI_MULSHIFT2 : C callable function
	; 
	;		
	; to multiply 2x32 bit signed nos. and shift left by 2
	; Input : AR0 -> xh xl
	; Input : AR1-> yh yl
	; return AC0 -> shifted product
	;
	.def _RA_TNI_MULSHIFT2
	
_RA_TNI_MULSHIFT2:

		; AR0 -> xh xl
		; AR1-> yh yl
		
		; AC0-> upper 32-bits
		; AC3-> lower 32-bits
		; T0 -> h
		; perform multiplication
		; copy into memory 
		
		;dbl(push(AC1))		; XSP points to AC1 (31-16)
		;||XAR1 = XSP 		; XAR1 points to AC1 (31-16)
		
		;dbl( push(AC0) )	
		;||XAR0 = XSP		; XAR0 points to AC0 (31-16)
		
		mar( *AR0+  ) || mar( *AR1+ )	; modify AR0 and AR1
		; AR0 -> xl
		; AR1 -> yl
		
		AC3 = uns( *AR0- ) * uns( *AR1 )	; xl * yl
		; AR0 -> xh
		; AR1 -> yl
		
		; AC3 = (xl*yl)>>16  +  xh*yl + xl * yh 
		 AC3 = (AC3 >> #16) + ( (*AR0+) * uns(*AR1-) )	; xh*yl
		; AR0 -> xl
		; AR1 -> xh
		AC3 = AC3 + ( uns(*AR0-) * (*AR1) ) 	;xl*yh
		
		;AC0 = AC3>>16 + xh*yh
		AC0 = (AC3>>#16 ) + ( (*AR0) * (*AR1) )   ; xh*yh
		
		;AC2 = dbl(pop())  
		;AC1 = dbl(pop())
		;shifting
		AC3 = AC3 & 0xFFFF
		AC0  = AC0<<#2 
		AC0 = AC0|(AC3 <<< #-14)
		
		return 
     .endif


	.if !__OPTIMISE_SECOND_PASS__		; otherwise use optimised RA_TNI_PostMultiply
	;----------------------------------------------------------------------------------
	; _RA_TNI_MULSHIFTNCLIP : C callable function
	; 
	;		
	; to multiply 2x32 bit signed nos. and shift left by 'clip' and saturate
	; Input : AR0 -> xh xl
	; Input : AR1-> yh yl
	; Input : T0 -> clip
	; return AC0 -> shifted product
	;
	
	
	
	.def _RA_TNI_MULSHIFTNCLIP
	.sect .const
possat .long 0x7FFFFFFF 	 ;for +ve saturation
negsat .long 0x80000000      ;for -ve saturation

	.text	
_RA_TNI_MULSHIFTNCLIP:

		; AR0 -> xh xl
		; AR1-> yh yl
		
		; AC0-> upper 32-bits
		; AC3-> lower 32-bits
		; T0 -> clip
		; perform multiplication
		; copy into memory 
		
		;dbl(push(AC1))		; XSP points to AC1 (31-16)
		;||XAR1 = XSP 		; XAR1 points to AC1 (31-16)
		
		;dbl( push(AC0) )	
		;||XAR0 = XSP		; XAR0 points to AC0 (31-16)
		
		mar( *AR0+  ) || mar( *AR1+ )	; modify AR0 and AR1
		; AR0 -> xl
		; AR1 -> yl
				 
		T1 = T0	; copy 'clip' value in T1
		||
		AC3 = uns( *AR0- ) * uns( *AR1 )	; xl * yl in AC3
		; AC3 -> bits 15-0 of product in AC3_L
		; AR0 -> xh
		; AR1 -> yl
		
		
		; AC1 = (xl*yl)>>16  +  xh*yl + xl * yh 
		AC1 = (AC3 >> #16) + ( (*AR0+) * uns(*AR1-) )	; xh*yl
		; AR0 -> xl
		; AR1 -> xh
				
		AC1 = AC1 + ( uns(*AR0-) * (*AR1) ) 	;xl*yh
		;AC1 -> bits 31-16 of product in lower AC1_L. AC1_H contains an overflow
		
		
		;AC0 = AC1>>16 + xh*yh
		AC0 = (AC1>>#16 ) + ( (*AR0) * (*AR1) )   ; xh*yh
		;AC0 -> bits 63-32 of product
		
		;shifting
		AC3 = AC3 & 0xFFFF
		
	;	AC2 = dbl(pop())   ; clear the stack
	;	AC2 = dbl(pop())   
		AC1 = AC1 & 0xFFFF 		; to isolate bits 31-16 or product
		mar (T1 - #31 )	; shift factor to detect saturation for calc. of temp_xh
		||				
		AC3 = AC3 | (AC1 <<< #16)	; AC3 contains bit # 31-0 of product i.e y
		
		AC2 = AC0 << T1			; AC2 = temp_xh = x >> ( 31 - clip),
		
		mar( T1 - #1 )			;  T1 = shift value for AC3	ie y
		||
		AC0 = AC0 << T0		;x = (x << clip) 
		
		AC3 = AC3 <<< T1	; y = y >>> (32-clip)
		AC0 = AC0 | AC3 	; x = x | y
		||
		if( AC2 > 0 ) execute (D_Unit)		; ; if temp_xh >0 , saturate AC0 upwards
			AC0 = dbl(*(#possat))
		
		AC2 = AC2 + #1
		if( AC2 < 0 ) execute (D_Unit)	; if temp_xh < -1 i.e temp_xh+1 < 0 , saturate downwards
			AC0 = dbl(*(#negsat))
			

		;return x = (x<<clip) | (y >> (32-clip) ) or saturate values
		return 
	.endif			; __OPTIMISE_SECOND_PASS__ 
											
	;----------------------------------------------------------------------------------
	; _RA_TNI_BUTTERFLY : C callable function
	; 
	;	to perform butterfly operation on a stream using twiddle factors		
	; 
	; Input : AR0 -> zptr1
	; Input : AR1 -> zptr2 
	; Input : AR2 -> ztwidptr
	; Input : T0 -> bg
	; return void
	;
	; Registers touched : XCDP,XAR0, XAR1, XAR2, XAR3, T0, BRC0, AC0, AC1, AC2, AC3
	
	
	; This function has been inlined in RA_TNI_FFT : No requirement now
	
	.if !__OPTIMISE_SECOND_PASS__
	.def _RA_TNI_BUTTERFLY	
	
_RA_TNI_BUTTERFLY
		
				
		;AR0 -> Re {zptr1}				; ar
		;AR0(2) -> Im {zptr1 }			; ai
		;AR1 -> Re {zptr2}				: br
		;AR1(2) -> Im {zptr2 }			: bi (AR3)
		;AR2 -> Re {ztwidptr}			: cr /CDP
		;AR2(2) -> Im {ztwidptr }		: ci /CDP(2)
		push(@#03) ||mmap() 			; save the ST1 register
		mar(T0 - #1) 	; BRC0 + 1 = bg 
		; XAR not AR - since the pages
			; matter -- this caused a problem
			; solved on 12/9/01 at 2300hrs
		||	
		bit(ST1, #10) = #1 ; set M40 bit	; this issue was
							; solved after 1 and 1/2 days of debugging
		XAR3 = XAR1 ;	
		|| pshboth( XCDP )	; save on entry
		BRC0 = T0 
		
		XCDP = XAR2		; for dual mac operations
		||
		blockrepeat {
			AR3 = AR1 + #2	; AR3 = bi
			; AR1 : br
			; AR3 : bi
			; CDP : cr
			
			; (AR1 *CDP + (AR3* CDP(2) )
			; (AR1 *CDP(2) ) - (AR3 *CDP )
			;
			; multiply AC0 = br * cr :: AC1 = bi * cr 
			
			;mar( *AR1+  ) || mar( *CDP+ )	; modify AR0 and AR1
			
			; AR1 -> br_h  
			; CDP -> cr_h 
			; AR3 ->bi_h
						
			AC0 = (*AR1+) * coef(*CDP+) ,	
				AC1 = (*AR3+) * coef(*CDP+) 	;
			; AR1 -> br_l  
			; CDP-> cr_l 
			; AR2 -> bi_l			
			; AC0 = br_h*cr_h  , AC1 = bi_h * cr_h
			|| T0 = #2			;increment factor
			
			
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
			; AC3 -> (bi_h * cir_l) + ( bi_l*ci_l)>>16
			
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
			
			
			; imaginary part = AC2(br*ci) - AC1(bi*cr)
			AC2 = AC2 - AC1 || mar(*CDP-)	
			; CDP-> cr_h
			
			; real part = AC0(br*cr) + 	AC3(bi*ci)
			AC0 = AC0 + AC3 
			
			; AC0 ->rtemp
			; AC2 ->itemp
			||
			AC1 = dbl(*AR0) 		; AC1 -> ar 	
			AC3 = dbl(*AR0(T0))		; AC3 -> ai
			||
			AC1 = AC1>> #1			; ar = ar >> 1
			AC3 = AC3>> #1 			; ai = ai >> 1
			||
			dbl(*AR0) = AC1			; temp storage of ar
			dbl(*AR0(T0)) = AC3		; and ai
			||
			AC1 = dbl(*AR0) + AC0  ; ar + rtemp
			
			
			
			AC3 = dbl(*AR0) - AC0  ; ar - rtemp
			||
			dbl(*(AR0+T0)) = AC1       ; zptr1+0 = ar + rtemp
			dbl(*(AR1+T0))= AC3		  ; zptr2+0 = ar - rtemp
			; AR0 -> zptr1+2 (img part)
			; AR1 -> zptr2+2 (img part)
			||
			AC1 = dbl(*AR0) + AC2  ; ai + itemp
			AC3 = dbl(*AR0) - AC2  ; ai - itemp
			||
			dbl(*(AR1+T0)) = AC1	  ; zptr2+2 = ai + itemp
			dbl(*(AR0+T0)) = AC3	  ; zptr1+2 = ai - itemp
			
			; AR0 -> real part of next complex no. in zptr1
			; AR1 -> real part of next complex no. in zptr2
			; CDP -> cr_h
			
					
		}
	
		XCDP = popboth()
		@#03 = pop() ||mmap() 			; restore the ST1 register
		return 
	.endif ;__OPTIMISE_SECOND_PASS__
		
		
					
	
	.endif	;__OPTIMISE_FIRST_PASS__