; ***** BEGIN LICENSE BLOCK *****   
; Source last modified: $Id: gainctrl.asm,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
; 	File 		:	assembly_Gainctrl.asm 
;   Description : C55X assembly routines for functions present in Gainctrl.c
;
;				: Interpolate
;				: RA_TNI_MAKESHORT
;				: RA_TNI_GainCompensate
;				: RA_TNI_DecodeGainInfo
;				: RA_TNI_GainWindow
;
; NOTE : Define [GAINWINDOW_ASM .set 0x01] to enable assembly version of _RA_TNI_GainWindow
;
;------------------------------------------------------------------------------------------------------------
;/*
; * Gecko2 stereo audio codec.
; * Developed by Ken Cooke (kenc@real.com)
; * August 2000
; *
; * Last modified by:	Firdaus Janoos (firdaus@ti.com)
; *						03/10/2001
; *
; * Gain Control functions.
; */


	.if INTERP_ASM			; define this symbol to enable asm version of Interpolate(); RA_TNI_MAKESHORT()
	.include "coder.h55"		
	;.def _Interpolate 		; defined just before Interpolate() (conditional definition)		
;	.def _RA_TNI_MAKESHORT	; defined just before MAKESHORT() (conditional definition)		
	.def _RA_TNI_GainCompensate
	.def _RA_TNI_DecodeGainInfo 

	;.def _RA_TNI_GainWindow		; called internally from _RA_TNI_GainCompensate


AC0H	.set  0x09			; mmreg AC0
	
	.global _RA_TNI_fixroot2ntab	;const long RA_TNI_fixroot2ntab[3][2*GAINDIF + 1] 	/* 3 rows, for N = 256, 512, 1024, respectively */

;#define GAINMAX		4
;#define GAINMIN		(-7)	/* clamps to GAINMIN<=gain<=GAINMAX */
;#define GAINDIF		(GAINMAX - GAINMIN)
	
;GAINMAX		.set 	4
;GAINMIN		.set	-7
		.eval GAINMAX - GAINMIN, GAINDIF
		.eval GAINMIN * 2 , GAINMIN2	
		.eval GAINDIF*2	, GAINDIF2
;		.eval ( 2*GAINDIF + 1)*2,  GAINDIF3
	
;----------------------------------------------------------------------------------
; _Interpolate : C callable function
; 
;		
;	
; * Interpolates part of gain control window.
; * log interp done by successive multiplication.
; *
; * gain ranges from -7 to 4 so window ranges from 2^-7 to 2^4
; * if gain0 != gain1, we let fixwnd start at 2^gain0 and increase it logarithmically
; *   to a final value of gain1
; *
; * This is the last step before converting to short, so fractional roundoff error is
; *	not a big issue
; * Gain window represented as 16.16 signed to accomodate gains of [2*GAINMIN to 2*GAINMAX]
; * Interpolation factors stored as 2.30 signed
; *
;
; Input :    XAR0 -> inptr (long*)
;		:	 T0   -> gain0 (short)
;		:    T1   -> gain1 (short)
;		:	 AR1  -> rootindex (short)
;		:    AR2  -> npsamps (short)
;
; Output : void
;
; Registers Touched : XAR0, XAR1, XAR2, XAR3, XAR4, XCDP, XSP, BRC0, T0, T1, AC0,AC1, AC2, AC3
;
; Globals : _RA_TNI_fixroot2ntab
	
	.cpl_on
	
	.if !GAINWINDOW_ASM				; if assembly version of RA_TNI_GainWindow is not used
	
	.def _Interpolate
_Interpolate:
	;-----------------------------------------------------
	;
	; to assign
	;
	;  SP -> fixwnd		;fixwnd can be retained in AC0
	;
	;  XCDP -> fixwnd
	;  XAR4 -> fixfactor
	;
	;  T1 = T1 - T0  
	;-----------------------------------------------------
    .noremark 5673	
	push( @#03 ) ||mmap()	
	; save ST1	
	
;	push ( @#0x4B) || mmap()
	; save ST2
	
	pshboth(XCDP)
	; save for later restore
		
	; allocate space for fixwnd on stack
	SP = SP - #3			; NOTE : aligment of SP
	; SP -> fixwnd
	
	
	
	T1 = T1 - T0
	; T1 -> gain1 - gain0
	||
	AC0 = 0x100
		
	; fixwnd = 0x100 << (gain0- 2*GAINMIN)
	T0 = T0 - GAINMIN2
	;(gain0- 2*GAINMIN)
	
	mar( AR2 - #1)
	; npsamps - 1
	||
	mar( *AR0+ )
	; AR0 -> inptr_l
	
	bit(ST1, #10) = #1
	; set M40 bit
	; to allow 40 bit arithmetic
	||	
	AC0 = AC0 <<< T0 
	;uns(0x100 << (gain0- 2*GAINMIN))
	; AC0 -> fixwnd
	
	XCDP = XSP
	; XCDP -> fixwnd

	
	dbl(*SP) = AC0 
	; store fixwnd on stack
	; XAR3 -> fixwnd
	||
	bit(ST2, #15) = #0
	; clear the ARMS bit to use DSP Mode addressing
	.ARMS_off
	
	
	BRC0 = AR2
	; BRC0 = npsamps - 1
	||
	if( T1 != 0 ) goto ELSE_BLOCK	; if ( gain0 != gain1 )
	; gain0 == gain1

	XAR3 = XCDP				 
	mar( *AR3+)
	; AR3 -> fixwnd_l
	||
	; do npsamps times
	localrepeat{
		; to do 
		; RA_TNI_MULSHIFT0( inptr, fixwnd)
		; AC2 -> upper 32-bits of product
		T0 = #3
		; for incrementing inptr
		||
		; AR0 -> inptr_L
		; AR3 -> fixwnd _l
		AC2 = uns( *AR0- ) * uns( *AR3 )	; inptr_l * fixwnd_l
		; AR0 -> inptr_h
		; AR3 -> fixwnd_l
		; AC2 = (inptrl*fixwnd_l)>>16  +  xh*yl + xl * yh 
		AC2 = (AC2 >> #16) + ( (*AR0+) * uns(*AR3-) )	; inptr_h*fixwnd_l
		; AR0 -> inptr_l
		; AR1 -> fixwnd_h
		AC2 = AC2 + ( uns(*AR0-) * (*AR3) ) 	;inptr_l* fixwnd_h
		; AR0 -> inptr_h
		; AR3 -> fixwnd_h
		AC2= (AC2>>#16 ) + ( (*AR0) * (*AR3+) )   ; inptr_h*fixwnd_h
		; AR0 = inptr_h
		; AR3 -> fixwnd_l
		
		dbl(*(AR0+T0) ) = AC2
		; inptr++ = inptr*fixwnd
		; AR0 -> (inptr+2)_l
		
		
		
	}
		
	goto END_INTERPOLATE
	; return if u please
				
ELSE_BLOCK:			; gain0 != gain1
	
	; fixfactor = *( (RA_TNI_fixroot2ntab + rootindex *( 2*GAINDIF +1)*2 + (gain1 - gain0 + GAINDIF)*2 )
	; fixfactor = *( RA_TNI_fixroot2ntab + 2*GAINDIF*(2*rootindex +1) + 2*(gain1 - gain0 + rootindex) )
	XAR4 = _RA_TNI_fixroot2ntab
		
	AR1 = AR1 << #1 
	; AR1 -> 2 * rootindex : for dword addressing
	||
	mar( T1 + T1 )
	; T1 = (gain1 - gain0 )*2 : for dword addressing
	
	mar( T1 + AR1 )
	; T1 -> (gain1 - gain0 + rootindex)*2
	||
	mar( AR1 + #1)
	; AR1 = 2*rootindex + 1
	T0 = AR1
	; T0 = 2*rootindex + 1
	||
	AC1 = T1
	; AC1 -> (gain1 - gain0 + rootindex)*2 
	
	AC1 = AC1 + (T0 * GAINDIF2)
	; AC1 -> (gain1 - gain0 + rootindex)*2 + 2*GAINDIF*(2*rootindex+1)
	
		
	AR4 = AR4 + AC1
	; XAR4 = RA_TNI_fixroot2ntab + 2*GAINDIF*(2*rootindex +1) + 2*(gain1 - gain0 + rootindex) 
	||
	mar( *CDP+)
	; CDP -> fixwnd_l
	
	
	mar(*AR4+)
	; AR4 -> fixfactor_l
	||
	; do npsamps times
	localrepeat{
			
		; AR0 -> inptr_l
		; CDP -> fixwnd_l
		; AR4 -> fixfactor_l
		
		; to calculate
		;   (inptr * fixwnd) || (fixfactor*fixwnd)
		; AC2 = inptr*fixwnd
		; AC1 = upper 32 bits of fixfactor*fixwnd
		; AC3 = lower 32 bits of fixfactor*fixwnd
		;	
			
		T0 = #3
		; for incrementing inptr
		||
		; AR0 -> inptr_L
		; AR3 -> fixwnd _l
		AC2 = uns( *AR0- ) * uns(coef( *CDP) ),	; inptr_l * fixwnd_l
			AC3 = uns( *AR4- ) * uns (coef(*CDP)) 		; fixfactor_l * fixwnd_l
		; AR0 -> inptr_h
		; CDP -> fixwnd_l
		; AR4 -> fixfactor_h
		
		T1 = -1
		; T1 = 0xFFFF
		||
		AC2 = (AC2 >> #16) + ( (*AR0+) * uns(coef(*CDP-) )),	; inptr_h*fixwnd_l
			AC3 = (AC3 >> #16) + ( (*AR4+) * uns(coef(*CDP-) ))	; fixfactor_h*fixwnd_l

		; AR0 -> inptr_l
		; CDP -> fixwnd_h
		; AR4 -> fixfactor_l
		AC2 = AC2 + ( uns(*AR0-) * coef(*CDP) ), 	;inptr_l* fixwnd_h
			AC3 = AC3 + ( uns(*AR4-) * coef(*CDP) ) 	;fixfactor_l* fixwnd_h
		; AR0 -> inptr_h
		; CDP -> fixwnd_h
		; AR4 -> fixfactor_h
		
		AC1 = AC3 
						
		AC2= (AC2>>#16 ) + ( (*AR0) * coef(*CDP) ),   ; inptr_h*fixwnd_h
			AC1= (AC1>>#16 ) + ( (*AR4+) * coef(*CDP) )   ; fixfactor_h*fixwnd_h
		
		; AR0 = inptr_h
		; CDP -> fixwnd_h
		; AR4 -> fixfactor_l
			
		dbl(*(AR0 + T0)) = AC2 
		; inptr = inptr * fixwnd
		||
		; AR0 -> (inptr+2)_l
		AC3 = AC3 & T1
		; isolate bits 31-16 of product fixfactor* fixwnd
		AC1 = AC1<<#2
		; shifting of (AC1:AC3) << 2
		AC1 = AC1 | (AC3 <<< #-14 )
		
		dbl(*CDP) = AC1
		; fixwnd = (fixwnd*fixfactor)<<2
		
		mar(*CDP+)
		; CDP -> fixwnd_l
		
	}
	

	;-----------------------------------------------------
	; cleanup code
END_INTERPOLATE:	
	SP = SP + #3
	XCDP = popboth()
	; save for later restore
;	@#0x4B = pop()||mmap()
	; restore ST2
			
	@#03 = pop()||mmap()
	; restore ST1
		
	
	.ARMS_on
	.remark 5673
	bit(ST2, #15) = #1
	; set the ARMS bit to enable control mode
	
	return 
	



	.endif 	; .if !GAINWINDOW_ASM	
	
; _Interpolate 
;----------------------------------------------------------------------------------------------
	


;#define POSTBITS			19		/* never set less than 17, or risk rampant saturation before gain control */
;#define NINTBITS			10		/* used in gain control, shouldn't adjust unless changing gain control tables */
;#define PCMFBITS			(32 - POSTBITS - NINTBITS)	/* fraction bits */
;#define RNDMASK				(1 << (PCMFBITS - 1))
;#define POSCLIP				((((unsigned long)RNDMASK << 16) - 1) - (((unsigned long)RNDMASK << 1) - 1))
;#define NEGCLIP				(((long)~POSCLIP) - (((long)RNDMASK << 1) - 1))
;#define RNDFIXTOS(x)		(short)((long)(x + RNDMASK) >> PCMFBITS)

POSTBITS 	.set 	19
NINTBITS	.set 	10

		.eval 32 - POSTBITS - NINTBITS , PCMFBITS
		.eval 1 << (PCMFBITS - 1) ,	RNDMASK
		.eval ( (RNDMASK<<16) - 1) - ( ( RNDMASK << 1) - 1), POSCLIP
		.eval ( (~POSCLIP) -( ( RNDMASK << 1 ) - 1 ) ), NEGCLIP
		.eval ( POSCLIP >> PCMFBITS ) , POSRET
		.eval ( NEGCLIP >> PCMFBITS ) , NEGRET
	
	

	.if !INTERP_ASM
;----------------------------------------------------------------------------------
; RA_TNI_MAKESHORT : C callable function
; 
;
; Input :    AC0 -> x (long)
;			 AC1 -> y (long)
;
; Output : short (T0) 
;
      
      .def _RA_TNI_MAKESHORT
				
_RA_TNI_MAKESHORT:
	.noremark 5673
	XAR0 = #POSCLIP	
	AC2 = XAR0
	; AC2 = POSCLIP
	
	AC1 = AC1 + AC0 
	; (AC1) z = x + y 
	AC0 = AC1 
	; AC0 -> z
	AC3 = AC0 
	; AC3 = z 
	
	AC1 = AC1 - AC2
	; AC1 = z - POSCLIP
			
	AC2 = #-4 << #16				; NEGCLIP -> FFFC0000
	
	AC0 = AC0 - AC2		
	; AC0 = z - NEGCLIP

	AC3 = AC3 + RNDMASK
	; z + RNDMASK
	AC3 = AC3 << -PCMFBITS
	; (z + RNDMASK) >> PCMFBITS
	
	T0 = LO(AC3)
	||
	if( AC1 > 0 ) execute (D_Unit)
		T0 = POSRET ; if z > POSCLIP  return POSRET
		
	if( AC0 < 0 ) execute ( D_Unit)	; if z < NEGCLIP return NEGRET
		T0 = NEGRET
	;-----------------------------------------------------
	; cleanup code
	.remark 5673
	return 
		
; _RA_TNI_MAKESHORT
;----------------------------------------------------------------------------------------------
	.endif  ;!INTERP_ASM



;----------------------------------------------------------------------------------
; RA_TNI_GainCompensate : C callable function
; 
;/*
; * Compensate the signal using the gain control function.
; * Includes the overlap-add stage of inverse transform.
; *
; * NOTE: operates in-place, with result in first half of input.
; *
; * gainc0 has the gain info for the first half of the input buffer
; * gainc1 has the gain info for the second half of the input buffer
; *
; */
;
; Input :  XAR0 -> input (long*)
;		   XAR1 -> gainc0 (GAINC*)
;		   XAR2 -> gainc1 (GAINC*)
;		   XAR3 -> overlap ( long*)  
;		   XAR4 -> outbuf (short*)
;		   T0 -> nsamps
;		   T1 -> pcmInterleaveFact
;
;
; Output : void
;


	.global _RA_TNI_GainWindow				;void RA_TNI_GainWindow(long *input, GAINC *gainc0, GAINC *gainc1, short nsamps);
	.eval -NINTBITS, RNINTBITS		; for right shifting by NINTBITS
	
;	.include coder.h55
	
	
_RA_TNI_GainCompensate:
	.noremark 5673
	
	; SP -> odd	
	;------------------------------------------------
	; prolog code
	;
	
	pshboth(XAR5) 		; 	push for later restore
	pshboth(XAR6)		; 
	; SP -> odd
	
	;------------------------------------------------
	; to assign
	;
	; XAR0 -> inPtr1
	; XAR1 -> gainc0 
	; XAR2 -> gainc1 
	; XAR3 -> overlap 
	; XAR4 -> outbuf 
	; XAR5 -> inPtr2
	; T0 -> nsamps
	; T1 -> pcmInterleaveFact

	
	XAR5 = XAR0

	AC0 = *AR1					; (#(GAINC.nats) )
	; AC0 = gainc0->nats
	||
	T0 = T0 << #1
	; nsamps << #1 for dword addressing 
	
	AC0 = AC0 | *AR2			; (#(GAINC.nats) )
	; gainc0->nats | gainc1->nats
	||
	AR5 = AR5 + T0
	; AR5 = input + (nsamps<<1)
	
	T0 = T0>>#1
	; T0 -> nsamps
	||
	if( AC0 == 0 ) goto ELSE_LOOP ; gainc0->nats == 0 && gainc1->nats == 0

	;case : gainc0->nats || gainc1->nats 
	pshboth(XAR0)
	pshboth(XAR1)
	pshboth(XAR2)
	pshboth(XAR3)
	pshboth(XAR4)
	push(T0, T1)
	; SP -> even
	
	; save *input, gainc0, gainc1,overlap, outbuf, nsamps and pcmInterleaveFact* for call to RA_TNI_GainWindow
	call _RA_TNI_GainWindow		;RA_TNI_GainWindow(long *input, GAINC *gainc0, GAINC *gainc1, short nsamps);		
	; XAR0 -> input
	; XAR1 -> gainc0
	; XAR2 -> gainc1 
	; T0 -> nsamps
	T0, T1 =  pop()
	XAR4 = popboth()
	XAR3 = popboth()
	XAR2 = popboth()
	XAR1 = popboth()
	XAR0 = popboth()
	; restore *input, gainc0, gainc1,overlap, outbuf, nsamps and pcmInterleaveFact* after call to RA_TNI_GainWindow
	||
	T0 = T0 - #1
	; nsamps - 1
	
	BRC0 = T0 
	; tripcount in BRC0 = nsamps-1

	T0 = T1 ;
	;T0  = pcmInterleaveFact 		: word addressing NOT dword addessing
	||	
	localrepeat { 	;do nsamps times
		;---------------------------------------------------------
		; RA_TNI_MAKESHORT
		XAR6 = #POSCLIP	
		AC2 = XAR6
		; AC2 = POSCLIP
	
		AC0 = dbl( *AR0+ )
		; AC0 = *inPtr1++
				
		AC1 = dbl( *AR3 ) + AC0 
		; (AC1) z =  *overlap + *inPtr1 
			
		AC0 = AC1 
		; AC0 -> z
		||
		AC3 = dbl(*AR5+)
		; AC3 = *inPtr2++
		
		dbl(*AR3+) = AC3
		; *overlap++ = AC3
		||
		AC1 = AC1 - AC2
		; AC1 = z - POSCLIP
		
		AC3 = AC0 
		; AC3 = z 
				
		AC2 = #-4 << #16				; NEGCLIP -> FFFC0000
		
		AC0 = AC0 - AC2		
		; AC0 = z - NEGCLIP
	
		AC3 = AC3 + RNDMASK
		; z + RNDMASK
		AC3 = AC3 << -PCMFBITS
		; (z + RNDMASK) >> PCMFBITS
		
		*AR4 = AC3
		; *outbuf = ( z + RNDMASK )>>  PCMFBITS
		||
		if( AC1 > 0 ) execute (AD_Unit)     ; if z > POSCLIP  ;to eliminate CPU_30
			*AR4 = POSRET			        ;	    *outbuf =  POSRET 
						
		
		if( AC0 < 0 ) execute ( D_Unit)	; if z < NEGCLIP 
		||	*(AR4+T0) = NEGRET					;		*outbut =  NEGRET  ;******
		;---------------------------------------------------------
		; end of RA_TNI_MAKESHORT
	
	;	AR4 = AR4 + T1 		; this can be done as mar( AR4 + T0 ) || with the prev cond instr
		; 
	
	}	
	
	goto END_GC			; can be replace with epilog code
	; end of function	


ELSE_LOOP:		
	;case : !(gainc0->nats || gainc1->nats )

	;T1  = pcmInterleaveFact 		: word addressing NOT dword addessing

	T0 = T0 - #1
	; nsamps - 1
	
	BRC0 = T0 
	; tripcount in BRC0 = nsamps-1

	T0 = T1
	
	T1= RNINTBITS 	; for right shifting
	;T0 = RNINTBITS 	; for right shifting
	||
	localrepeat { 	;do nsamps times
		;---------------------------------------------------------
		; RA_TNI_MAKESHORT
		
		XAR6 = #POSCLIP	
		AC2 = XAR6
		; AC2 = POSCLIP
		
		AC0 = dbl( *AR0+ )
		; AC0 = *inPtr1++
		AC0 = AC0 << T1
		; AC0 = *inPtr1++ >> NINTBITS
						
		AC1 = dbl( *AR3 ) + AC0 
		; (AC1) z =  *overlap + *inPtr1 >> NINTBITS
			
		AC0 = AC1 
		; AC0 -> z
		||
		AC3 = dbl(*AR5+)
		; AC3 = *inPtr2++
		AC3 = AC3 << T1
		; AC3 = (*inPtr2++) >> NINTBITS
		dbl(*AR3+) = AC3
		; *overlap++ = AC3
		||
		AC1 = AC1 - AC2
		; AC1 = z - POSCLIP
		
		AC3 = AC0 
		; AC3 = z 
				
		AC2 = #-4 << #16				; NEGCLIP -> FFFC0000
		
		AC0 = AC0 - AC2		
		; AC0 = z - NEGCLIP
	
		AC3 = AC3 + RNDMASK
		; z + RNDMASK
		AC3 = AC3 << -PCMFBITS
		; (z + RNDMASK) >> PCMFBITS
		
		*AR4 = AC3
		; *outbuf = ( z + RNDMASK )>>  PCMFBITS
		||
		if( AC1 > 0 ) execute (AD_Unit)     ; if z > POSCLIP   ; to eliminate CPU_30
			*AR4 = POSRET			        ;	    *outbuf =  POSRET 
						
			
		if( AC0 < 0 ) execute (D_Unit)	; if z < NEGCLIP 
		||	*(AR4+T0) = NEGRET					;		*outbut =  NEGRET  ;**************
		;---------------------------------------------------------
		; end of RA_TNI_MAKESHORT
	
	;AR4 = AR4 + T1 		; this can be done as mar( AR4 + T0 ) || with the prev cond instr
		; 
	
	}	



END_GC:
	;------------------------------------------------
	; epilog code
	;
	.remark 5673
	XAR6 = popboth() 		; 	restore on exit
	XAR5 = popboth() 		; 	restore on exit
	return 
	
;-----------------------------------------------------------------------------------------------------
; end of RA_TNI_GainCompensate
	
;	.endif    		; INTERP_ASM
	
	
	
	
;----------------------------------------------------------------------------------
; RA_TNI_DecodeGainInfo : C callable function
; 
; Input :  XAR0 -> gainc (GAINC*)
;		   XAR1 -> pkbit (short*)
;		   XAR2 -> pktotal ( short*)
;		   XAR3 -> bitstrmPtr (RA_TNI_Obj)
;
;
; Return : T0 -> nbits
;	

	.eval LOCBITS - 32 , LOC32 
	.eval GAINBITS - 32 , GAIN32 

_RA_TNI_DecodeGainInfo:
	;------------------------------------------------
	; prolog code
	.noremark 5573
	.noremark 5571
	.noremark 5673
	.noremark 5505
	pshboth(XAR5)
	pshboth(XAR6)
	;||
	XAR5 = XAR0
	; XAR5  = gainc
	XAR6 = XAR0
	; XAR6 = gainc
	
	;------------------------------------------------
	; to assign
	; 
	; XAR0 -> gainc (GAINC*)
	; XAR1 -> pkbit (short*)
	; XAR2 -> pktotal ( sh
	; XAR3 -> bitstrmPtr (RA_TNI_Obj)
	; XAR4 -> bitstrmPtr->pkptr
	; XAR5 -> gainc->loc
	; XAR6 -> gainc->gain
	;
	; T0 -> nbits
	; T1 -> code, and other stuff
	;
	; AC0 -> *bitstrmPtr->nframebits
	; AC1 -> *(bitstrmPtr->pkptr) and other stuff
	; AC2 -> *pktotal - (*bitstrmPtr->nframebits), and other stuff
	; AC3
	; 

	;------------------------------------------------
	; function code
	
	AC0 = *AR3( #(RA_TNI_Obj.nframebits) )
	; AC0 = *bitstrmPtr->nframebits
	T0 = #0
	; nbits = 0
	||
	mar( AR3 + #(RA_TNI_Obj.pkptr) )
	; AC3 = bitstrmPtr + RA_TNI_Obj.pkptr

	
DO_CODE_LABEL:			;do {
	
	;-----------------------------------
	; RA_TNI_Unpackbit( code, pkbit, pktotal, bitstrmptr)
	
	*AR2 = *AR2 + #1
	; ++(*pktotal)
	
	;	AC2	= *AR2 - AC0 		; the frame boundary will never be reached
	; AC2 = *pktotal - (*bitstrmPtr->nframebits)
	
	;	if( AC2 > 0 )			; frame boundary will never be reached
		
	T1 = *AR1
	; T1 = *pkbit
	
	XAR4 = dbl(*AR3)
	; XAR4 = bitstrmPtr->pkptr
	||
	mar(T1 - #31)
	; T1 = *pkbit - #31
	
	AC1 = dbl(*AR4)
	; AC1 = *(bitstrmPtr->pkptr)
	||
	mar(T0 + #1)
	; nbits = nbits + #1
	
	*AR1 = *AR1 + #1
	; ++(*pkbit)
		
	AC2 = *AR1
	; AC1 = *pkbit
	||
	AC1 = AC1 <<< T1
	; AC1 = *(bitstrmPtr->pkptr) <<< (*pkbit - #31)
	
	AC2 = AC2 << #-4
	; AC2 = (*pkbit >> ( 5 - 1 ) 		
	||
	T1 = AC1 
	; T1 = AC1 = *(bitstrmPtr->pkptr) <<< (*pkbit - #31)
	
	T1 = T1 & 0x01
	; T1 = *(bitstrmPtr->pkptr <<< *pkbit ) >>> #31
	; T1 -> code 
	||
	bit( AC2, @0 ) = #0
	; ; AC2 = (*pkbit >>  5) << 1  		; for dword addressing
		
	AR4 = AR4 + AC2
	; AR4 = bitstrmPtr->pkptr + (*pkbit >>  5) << 1 

	*AR1 = *AR1 & 0x1F
	; *pkbit &= 0x1F
	
	dbl(*AR3) = XAR4
	; bitstrmPtr->pkptr = bitstrmPtr->pkptr + (*pkbit >>  5) << 1 
	;--------------End of RA_TNI_Unpackbit -----------------------------
	; T1 -> code
	; T0 -> nbits
	||
	if( T1 != 0 ) goto DO_CODE_LABEL
	
	; } while (code)
	;--------------------------------------------------------------------------
	
	
	
	T1 = T0 - #1
	; T1 = nbits - 1

	
	*AR0( #(GAINC.nats) )= T1
	; gainc->nats = T1

	if( T1 == 0 ) goto END_DECGAININFO
	; if( nbits-1 == 0 ) return nbits
	||
	T1 = T1 - #1
	; T1 = (nbits-1) - 1  
	
	BRC0 = T1
	; trip count = nbits - 2
	||
	mar( AR6 + #(GAINC.gain) )
	; XAR6 -> gainc->gain + 0
	
	mar( AR5 + #(GAINC.loc) )
	; XAR5 -> gainc->loc + 0	
	||
	blockrepeat{	; do ( nbits - 1 ) times
	;---------------------------------------
	; current assignments
	;
	; XAR0 -> gainc (GAINC*)
	; XAR1 -> pkbit (short*)
	; XAR2 -> pktotal ( sh
	; XAR3 -> bitstrmPtr + RA_TNI_Obj.pkptr
	; XAR4 -> bitstrmPtr->pkptr
	; XAR5 -> gainc->loc
	; XAR6 -> gainc->gain
	;
	; T0 -> nbits
	; AC0 -> *bitstrmPtr->nframebits
	
	; to assign
	; AC2 = *bitstrmPtr->pkptr
	; AC3 = temp
	;-------------------------------------------------------
	; nbits = nbits + RA_TNI_Unpackbits(LOCBITS, &gainc->loc[i], pkbit, pktotal, bitstrmptr)
	
	
	
	*AR2 = *AR2 + #LOCBITS
	; AC1 = *pktotal + LOCBITS
	
;	AC1 = *AR2 - AC0
	; AC1 = *pktotal - *bitstrmPtr->nframebits
	
	XAR4 = dbl(*AR3)
	; XAR4 -> bitstrmPtr->pkptr
;	||
;	if( AC1 > 0 ) goto UNPACKBIT_LABEL
	;if(*pktotal > *bitstrmPtr->nframebits) return 0

	T1 = *AR1
	; T1 = *pkbit
	||
	mar(T0 + #LOCBITS)
	; nbits = nbits + LOCBITS
	
	AC2 = dbl(*AR4)
	; AC2 = *(bitstrmptr->pkptr) 
	
	AC3 = AC2 << T1
	; AC3 = *(bitstrmptr->pkptr) << *pkbit
	||
	T1 =  T1 + #LOCBITS 
	; T1 = *pkbit + LOCBITs
				
	AC1 = T1
	; AC1 = *pkbit + LOCBITS
	||
	T1 = T1 - #32
	; T1 = *pkbit - 32
	
	mar( AR4 + #2)
	; XAR4 = bitstrmPtr->pkptr + 2
	|| 
	if( T1 < 0 ) goto UNPACKBITS_ELSE_LABEL 		; if ( *pkbit < 32 )
	
	;if (*pkbit >= 32 ) {
		AC2 = dbl(*AR4)
		; AC2 = *(bitstrmPtr->pkptr) 
		||
		AC1 = T1
		; AC1 = *pkbit - #32
		
		dbl(*AR3) = XAR4
		; bitstrmptr->pkptr = bitstrmPtr->pkptr + 2
		||
		T1 = T1 - LOCBITS  
		; T1 =  *pkbit - LOCBITS 		; for right shifting 
		nop
		nop;added to avoid silicon exception causing remark 5505
		AC2 = AC2 <<< T1
		; AC2 = *(bitstrmPtr->pkptr) >>> (LOCBITS-*pkbit) 
		||
		if( AC1 == 0 ) execute (AD_Unit)		; to eliminate CPU_30
		;	if( *pkbit )
			AC3 = AC3 | AC2
			; AC3 = temp | *(bitstrmPtr->pkptr) >>> (LOCBITS-*pkbit) 
	;}

		
UNPACKBITS_ELSE_LABEL:
	
	
	*AR1 = AC1
	; *pkbit = (*pkbit + LOCBITS) or (*pkbit + LOCBITS - 32 )
	||
	AC3 = AC3 <<< #LOC32
	; AC3 = temp >>> (32 - LOCBITS)
	
	*AR5+ = AC3
	; gainc->loc[i] = AC3
	; AR5 = gainc->loc + (i++)
	
	
	;---------------------------------------
	; current assignments
	;
	; XAR0 -> gainc (GAINC*)
	; XAR1 -> pkbit (short*)
	; XAR2 -> pktotal ( sh
	; XAR3 -> bitstrmPtr + RA_TNI_Obj.pkptr
	; XAR4 -> bitstrmPtr->pkptr
	; XAR5 -> gainc->loc++ 
	; XAR6 -> gainc->gain
	;
	; T0 -> nbits
	; AC0 -> *bitstrmPtr->nframebits
	
	; to assign
	; AC2 = *bitstrmPtr->pkptr
	; AC3 = temp	
	;-------------------------------------------------------
	; nbits = nbits + RA_TNI_Unpackbit( &code, pkbit, pktotal, bitstrmptr)
UNPACKBIT_LABEL:
	
	
	
	*AR2 = *AR2 + #1
	; ++(*pktotal)
	
;	AC2	= *AR2 - AC0 		
	; AC2 = *pktotal - (*bitstrmPtr->nframebits)
	
;	if( AC2 > 0 ) goto IF_CODE_LABEL
	; if( *pktotal > *bitstrmPtr->nframebits) 
	; 		return 0			
		
		
	T1 = *AR1
	; T1 = *pkbit
	
	XAR4 = dbl(*AR3)
	; XAR4 = bitstrmPtr->pkptr
	||
	mar(T1 - #31)
	; T1 = *pkbit - #31
	
	AC1 = dbl(*AR4)
	; AC1 = *(bitstrmPtr->pkptr)
	||
	mar(T0 + #1)
	; nbits = nbits + #1
	
	*AR1 = *AR1 + #1
	; ++(*pkbit)
		
	AC2 = *AR1
	; AC1 = *pkbit
	||
	AC1 = AC1 <<< T1
	; AC1 = *(bitstrmPtr->pkptr) <<< (*pkbit - #31)
	
	AC2 = AC2 << #-4
	; AC2 = (*pkbit >> ( 5 - 1 ) 		
	||
	T1 = AC1 
	; T1 = AC1 = *(bitstrmPtr->pkptr) <<< (*pkbit - #31)
	
	T1 = T1 & 0x01
	; T1 = *(bitstrmPtr->pkptr <<< *pkbit ) >>> #31
	; T1 -> code 
	||
	bit( AC2, @0 ) = #0
	; ; AC2 = (*pkbit >>  5) << 1  		; for dword addressing
		
	AR4 = AR4 + AC2
	; AR4 = bitstrmPtr->pkptr + (*pkbit >>  5) << 1 

	*AR1 = *AR1 & 0x1F
	; *pkbit &= 0x1F
	
	dbl(*AR3) = XAR4
	; bitstrmPtr->pkptr = bitstrmPtr->pkptr + (*pkbit >>  5) << 1 
	;--------------End of RA_TNI_Unpackbit -----------------------------
	; T1 -> code
	; T0 -> nbits
		
IF_CODE_LABEL:
	
	*AR6 = #-1
	; gainc->gain[i] = -1
	; AR6 = gainc->gain + i
	if( T1 == 0 )  goto REPEAT_LABEL		
	; if (!code ) continue		
				
ELSE_CODE_LABEL:	
	; else {
	
		;---------------------------------------
		; current assignments
		;
		; XAR0 -> gainc (GAINC*)
		; XAR1 -> pkbit (short*)
		; XAR2 -> pktotal ( short*)
		; XAR3 -> bitstrmPtr + RA_TNI_Obj.pkptr
		; XAR4 -> bitstrmPtr->pkptr
		; XAR5 -> gainc->loc
		; XAR6 -> gainc->gain
		;
		; T0 -> nbits
		; AC0 -> *bitstrmPtr->nframebits
		
		; to assign
		; AC2 = *bitstrmPtr->pkptr
		; AC3 = temp
		;-------------------------------------------------------
		; nbits = nbits + RA_TNI_Unpackbits(GAINBITS, &gainc->gain[i], pkbit, pktotal, bitstrmptr)
		
		
		
		*AR2 = *AR2 + #GAINBITS
		; AC1 = *pktotal + GAINBITS
		
;		AC1 = *AR2 - AC0
		; AC1 = *pktotal - *bitstrmPtr->nframebits
		
		XAR4 = dbl(*AR3)
		; XAR4 -> bitstrmPtr->pkptr
;		||
;		if( AC1 > 0 ) goto REPEAT_LABEL
		;if(*pktotal > *bitstrmPtr->nframebits) return 0
	
		T1 = *AR1
		; T1 = *pkbit
		||
		mar(T0 + #GAINBITS)
		; nbits = nbits + GAINBITS
		
		AC2 = dbl(*AR4)
		; AC2 = *(bitstrmptr->pkptr) 
		
		AC3 = AC2 << T1
		; AC3 = *(bitstrmptr->pkptr) << *pkbit
		||
		T1 =  T1 + #GAINBITS 
		; T1 = *pkbit + GAINBITs
					
		AC1 = T1
		; AC1 = *pkbit + GAINBITS
		||
		T1 = T1 - #32
		; T1 = *pkbit - 32
		
		mar( AR4 + #2)
		; XAR4 = bitstrmPtr->pkptr + 2
		|| 
		if( T1 < 0 ) goto UNPACKBITS_ELSE_LABEL2 		; if ( *pkbit < 32 )
		
		;if (*pkbit >= 32 ) {
			AC2 = dbl(*AR4)
			; AC2 = *(bitstrmPtr->pkptr) 
			||
			AC1 = T1
			; AC1 = *pkbit - #32
			
			dbl(*AR3) = XAR4
			; bitstrmptr->pkptr = bitstrmPtr->pkptr + 2
			||
			T1 = T1 - GAINBITS  
			; T1 =  *pkbit - GAINBITS 		; for right shifting 
			nop
			nop;added to avoid silicon exception causing remark 5505
			AC2 = AC2 <<< T1
			; AC2 = *(bitstrmPtr->pkptr) >>> (GAINBITS-*pkbit) 
			||
			if( AC1 != 0 ) execute (AD_Unit)		; to eliminate CPU_30
			;	if( *pkbit )
				AC3 = AC3 | AC2
				; AC3 = temp | *(bitstrmPtr->pkptr) >>> (GAINBITS-*pkbit) 
		;}

		
UNPACKBITS_ELSE_LABEL2:
	
	
		*AR1 = AC1
		; *pkbit = (*pkbit + GAINBITS) or (*pkbit + GAINBITS - 32 )
		||
		AC3 = AC3 <<< #GAIN32
		; AC3 = temp >>> (32 - GAINBITS)
		
		AC3 = AC3 + GAINMIN
		*AR6 = AC3
		; gainc->gain[i] = CODE2GAIN(AC3)
			
	;}

REPEAT_LABEL
	mar( AR6 + #1 )
	; AR6 = gainc->gain + (i++)
	}	


	;------------------------------------------------
	; epilog code
END_DECGAININFO:
	XAR6 = popboth()
	XAR5 = popboth()
	
	.remark 5573
	.remark 5571
	.remark 5673
	.remark 5505
	return 
	

;-----------------------------------------------------------------------------------------------------
; end of RA_TNI_DecodeGainInfo


	.endif    		; INTERP_ASM
	



;----------------------------------------------------------------------------------
; _RA_TNI_GainWindow : C callable function
; 
;	 
;/*
; * Multiply input by gain control window.
; * gainc0 is for first half, gainc1 is for second half.
; *
; */
;	Input : XAR0 -> input (long*)
;			XAR1 -> gainc0 (GAINC*)
;			XAR2 -> gainc1 (GAINC*)
;			T0 -> nsamps
;
;	Registers Touched : XAR0, XAR1, XAR2, XAR3,XAR4,XAR5,XAR6, XAR7 T0, T1,T2, T2, AC0, AC1, AC2, AC3
; 	
;	Returns : void
;
;	Calls : Interpolate()		/* This function has now been inlined */
;
;   Globals : none
;

	
	.if GAINWINDOW_ASM			; enable assembly version of RA_TNI_GainWindow

	.eval NPARTS + 1, EXGAIN_SIZE
	.eval NPARTS - 1, INITi

_RA_TNI_GainWindow:	
	; SP -> odd
	;------------------------------------------------
	; prolog code
	.noremark 5573
	push(@#0x03) || mmap()
	; save ST1
	
	push(T2,T3)
	; save for later restore
	||
	bit(ST2, #15) = #0
	; clear the ARMS bit to use DSP Mode addressing
	.ARMS_off
	
	pshboth(XAR5)
	||
	AR4 = T0
	; AR4 = nsamps
	
	pshboth(XAR6)
	||
	AR4 = AR4 << #1
	; AR4 = nsamps << 1 : for dword addressing
	

	pshboth(XAR7)
	pshboth(XCDP)
		
	bit(ST1, #10) = #1
	; set M40 bit
	; to allow 40 bit arithmetic
	;	||		// NOTE : Does not work
	
	SP = SP - #EXGAIN_SIZE
	; allocate space for exgain[NPARTS + 1] on stack
	; SP -> odd
	; NOTE Alignment of SP
	;*****************************************************************
	; Second part : operations using gainc1
	;*****************************************************************

	;--------------------------------------------------
	; to assign
	; T0 -> temp
	; T2 -> npsamps
	; T3 -> rootindex
	; AR4 -> nsamps
	; XAR5 -> gainc1->loc
	; XAR6 -> gainc1->gain
	; XAR7 -> exgain
	; XSP -> exgain
	;
	; AC1 -> nats
	
	XAR7 = XSP
	
	AC0 = T0
	; AC0 = nsamps
	||
	*SP(#NPARTS) = #0
	; exgain[NPARTS] = 0
	
	AC0 = AC0 << #-3
	; AC0 = nsamps >> 3
	||
	AC1 = *AR2
	; AC1 (nats) = gainc1->nats
	
	T2 = AC0
	; T2(npsamps) = nsamps >> 3
	||
	AC0 = AC0 << #-6
	; AC0 = (nsamps >> 3) >> 6
	

	T3 = AC0
	; T3 = rootindex 	
	||
	if( AC1 == 0 ) goto GAINC1_ELSE_LABEL    ;if (nats == 0 ) 
	
	; if(nats) {
	
		;--------------------------------------------------
		; current  assignments
		;
		; T0 -> *****
		; T1 -> *****
		; T2 -> npsamps
		; T3 -> rootindex
		; AR4 -> nsamps << 1
		;
		; XAR0 -> input (long*)
		; XAR1 -> gainc0 
		; XAR2 -> gainc1 
		; XAR5 -> ******
		; XAR6 -> ******
		; XAR7 -> exgain
		; XSP -> exgain
		;
		; AC1 -> ****
		;
		;--------------------------------------------------
	
		T1 = INITi 
		; T1(i) = NPARTS - 1
				
		XAR5 = XAR2
		; XAR5 -> gainc1
		XAR6 = XAR2 
		; XAR6 -> gainc1
		||
		AC0 = #0
	
		mar(AR7 + NPARTS )
		; AR7 -> exgain[NPARTS] 
	
		BRC0 = T1
		; trip count = NPARTS - 1
		||
		mar( AR5 + #(GAINC.loc))
		; XAR5 -> gainc1->loc
		
		mar( AR6 + #(GAINC.gain))
		; XAR6 -> gainc1->gain
		||		
		localrepeat{ 	; do NPARTS times
			
			T0 = AC1 - #1
			; T0 = nats - 1
			AC2 =  *AR5(T0)
			; AC2 =  gainc1->loc[nats-1]
			||			
			AC3 = *AR7- 
			; AC3 = exgain[i+1]
			; AR7 -> exgain[i]
			
			TC1 = ( AC1 != AC0 )
			; set TC1  if ( nats != 0 )	
			||
			TC2 = ( T1 == AC2)
			; set TC2 		if( i == gainc1->loc[nats-1] )
			
			T1 = T1 - #1
			; T1 = i - 1 (for next loop)
			||
			*AR7 = AC3
			; exgain[i] = exgain[i+1]
			.noremark 5590
			.noremark 5503
			if( TC1 & TC2  ) execute (AD_Unit)			; if( nats && ( i == gainc1->loc[nats-1] ) )
			 	*AR7 = *AR6(T0)							;			exgain[i] = gainc1->gain[nats-1]
			 	||										;			
			 	AC1 = AC1 - #1							;			AC1(nats) = nats - 1
			.remark 5590 				 				 	
			.remark 5503
		}			
	
	
		;-------------------------------------------------
		; to assign (for call to Interpolate() )
		;
		; XAR0 -> inptr
		;
		; T0 -> exgain[i] (gain0)
		; T1 -> exgain[i+1] (gain1)
		; AR1 -> rootindex
		; AR2 -> npsamps
		;
		; XAR5 ->input
		; XAR6 -> gainc0
		; XAR7 -> exgain
		;
		;--------------------------------------------------
	
	
		XAR5 = XAR0
		; XAR5 -> input
	
		XAR6 = XAR1	
		; XAR6 -> gainc0
		
		AR0 = AR0 + AR4 
		; AR0(inptr) = input + (nsamps<<1)
		||
		T0 = INITi
		; T0 = trip count (NPARTS-1)
		
		
		BRC0 = T0
		; trip count (NPARTS-1) in T0		
		
		; SP -> odd		
		; allocate space for fixwnd on stack
		SP = SP - #3			; NOTE : aligment of SP
		; SP -> fixwnd
		; SP => even
		||
		; for( i = 0 ; i < NPARTS ; i++)	{
		blockrepeat{
			T0 = *AR7+
			; T0 = exgain[i] 
			; AR7 -> exgain[i+1]
			||
			AR1 = T3
			; AR1 = rootindex
			
			T1 = *AR7
			; T1 = exgain[i+1]
			||
			AR2 = T2 
			; AR2 = npsamps
			
			
			;*****************************************************************
			; Inling of Interpolate function
			; Interpolate( inptr, exgain[i], exgain[i+1], rootindex, npsamps );
			; SP -> even
			;call _Interpolate		
			; AR0 = inptr + (npsamps<<1) + 1
			; subtract 1 to set AR0 = inptr + (npsamps<<1)
			

			;-----------------------------------------------------
			;
			; to assign
			;
			;  SP -> fixwnd		;fixwnd can be retained in AC0
			;
			;  XCDP -> fixwnd
			;  XAR4 -> fixfactor
			;
			;  T1 = T1 - T0  
			;-----------------------------------------------------
			
		
  			; SP -> odd		
			; allocate space for fixwnd on stack
		;	SP = SP - #3			; NOTE : aligment of SP
			; SP -> fixwnd
			; SP => even
		
			
			T1 = T1 - T0
			; T1 -> gain1 - gain0
			||
			AC0 = 0x100
				
			; fixwnd = 0x100 << (gain0- 2*GAINMIN)
			T0 = T0 - GAINMIN2
			;(gain0- 2*GAINMIN)
			
			mar( AR2 - #1)
			; npsamps - 1
			||
			mar( *AR0+ )
			; AR0 -> inptr_l
			
			AC0 = AC0 <<< T0 
			;uns(0x100 << (gain0- 2*GAINMIN))
			; AC0 -> fixwnd
			
			XCDP = XSP
			; XCDP -> fixwnd
		
			
			dbl(*SP) = AC0 
			; store fixwnd on stack
			; XAR3 -> fixwnd
		
			BRC1 = AR2
			; BRC1 = npsamps - 1
			||
			if( T1 != 0 ) goto GAINC1_IF_ELSE_BLOCK	; if ( gain0 != gain1 )
			; gain0 == gain1
		
			XAR3 = XCDP				 
			mar( *AR3+)
			; AR3 -> fixwnd_l
			||
			; do npsamps times
			localrepeat{
				; to do 
				; RA_TNI_MULSHIFT0( inptr, fixwnd)
				; AC2 -> upper 32-bits of product
				T0 = #3
				; for incrementing inptr
				||
				; AR0 -> inptr_L
				; AR3 -> fixwnd _l
				AC2 = uns( *AR0- ) * uns( *AR3 )	; inptr_l * fixwnd_l
				; AR0 -> inptr_h
				; AR3 -> fixwnd_l
				; AC2 = (inptrl*fixwnd_l)>>16  +  xh*yl + xl * yh 
				AC2 = (AC2 >> #16) + ( (*AR0+) * uns(*AR3-) )	; inptr_h*fixwnd_l
				; AR0 -> inptr_l
				; AR1 -> fixwnd_h
				AC2 = AC2 + ( uns(*AR0-) * (*AR3) ) 	;inptr_l* fixwnd_h
				; AR0 -> inptr_h
				; AR3 -> fixwnd_h
				AC2= (AC2>>#16 ) + ( (*AR0) * (*AR3+) )   ; inptr_h*fixwnd_h
				; AR0 = inptr_h
				; AR3 -> fixwnd_l
				
				dbl(*(AR0+T0) ) = AC2
				; inptr++ = inptr*fixwnd
				; AR0 -> (inptr+2)_l
				
				
				
			}
				
			goto GAINC1_IF_END_INTERPOLATE
			; return if u please
						
GAINC1_IF_ELSE_BLOCK:			; gain0 != gain1
			
			; fixfactor = *( (RA_TNI_fixroot2ntab + rootindex *( 2*GAINDIF +1)*2 + (gain1 - gain0 + GAINDIF)*2 )
			; fixfactor = *( RA_TNI_fixroot2ntab + 2*GAINDIF*(2*rootindex +1) + 2*(gain1 - gain0 + rootindex) )
			XAR4 = _RA_TNI_fixroot2ntab
				
			AR1 = AR1 << #1 
			; AR1 -> 2 * rootindex : for dword addressing
			||
			mar( T1 + T1 )
			; T1 = (gain1 - gain0 )*2 : for dword addressing
			
			mar( T1 + AR1 )
			; T1 -> (gain1 - gain0 + rootindex)*2
			||
			mar( AR1 + #1)
			; AR1 = 2*rootindex + 1
			T0 = AR1
			; T0 = 2*rootindex + 1
			||
			AC1 = T1
			; AC1 -> (gain1 - gain0 + rootindex)*2 
			
			AC1 = AC1 + (T0 * GAINDIF2)
			; AC1 -> (gain1 - gain0 + rootindex)*2 + 2*GAINDIF*(2*rootindex+1)
			
				
			AR4 = AR4 + AC1
			; XAR4 = RA_TNI_fixroot2ntab + 2*GAINDIF*(2*rootindex +1) + 2*(gain1 - gain0 + rootindex) 
			||
			mar( *CDP+)
			; CDP -> fixwnd_l
			
			
			mar(*AR4+)
			; AR4 -> fixfactor_l
			||
			; do npsamps times
			localrepeat{
					
				; AR0 -> inptr_l
				; CDP -> fixwnd_l
				; AR4 -> fixfactor_l
				
				; to calculate
				;   (inptr * fixwnd) || (fixfactor*fixwnd)
				; AC2 = inptr*fixwnd
				; AC1 = upper 32 bits of fixfactor*fixwnd
				; AC3 = lower 32 bits of fixfactor*fixwnd
				;	
					
				T0 = #3
				; for incrementing inptr
				||
				; AR0 -> inptr_L
				; AR3 -> fixwnd _l
				AC2 = uns( *AR0- ) * uns(coef( *CDP) ),	; inptr_l * fixwnd_l
					AC3 = uns( *AR4- ) * uns (coef(*CDP)) 		; fixfactor_l * fixwnd_l
				; AR0 -> inptr_h
				; CDP -> fixwnd_l
				; AR4 -> fixfactor_h
				
				T1 = -1
				; T1 = 0xFFFF
				||
				AC2 = (AC2 >> #16) + ( (*AR0+) * uns(coef(*CDP-) )),	; inptr_h*fixwnd_l
					AC3 = (AC3 >> #16) + ( (*AR4+) * uns(coef(*CDP-) ))	; fixfactor_h*fixwnd_l
		
				; AR0 -> inptr_l
				; CDP -> fixwnd_h
				; AR4 -> fixfactor_l
				AC2 = AC2 + ( uns(*AR0-) * coef(*CDP) ), 	;inptr_l* fixwnd_h
					AC3 = AC3 + ( uns(*AR4-) * coef(*CDP) ) 	;fixfactor_l* fixwnd_h
				; AR0 -> inptr_h
				; CDP -> fixwnd_h
				; AR4 -> fixfactor_h
				
				AC1 = AC3 
								
				AC2= (AC2>>#16 ) + ( (*AR0) * coef(*CDP) ),   ; inptr_h*fixwnd_h
					AC1= (AC1>>#16 ) + ( (*AR4+) * coef(*CDP) )   ; fixfactor_h*fixwnd_h
				
				; AR0 = inptr_h
				; CDP -> fixwnd_h
				; AR4 -> fixfactor_l
					
				dbl(*(AR0 + T0)) = AC2 
				; inptr = inptr * fixwnd
				||
				; AR0 -> (inptr+2)_l
				AC3 = AC3 & T1
				; isolate bits 31-16 of product fixfactor* fixwnd
				AC1 = AC1<<#2
				; shifting of (AC1:AC3) << 2
				AC1 = AC1 | (AC3 <<< #-14 )
				
				dbl(*CDP) = AC1
				; fixwnd = (fixwnd*fixfactor)<<2
				
				mar(*CDP+)
				; CDP -> fixwnd_l
				
			}
			
		
			;-----------------------------------------------------
			; cleanup code
GAINC1_IF_END_INTERPOLATE:	
		;	SP = SP + #3
		;	XCDP = popboth()
			; save for later restore
			
		
			; End of Interpolate	
			;*****************************************************************
			;----------------------------------------------------
			; Current reg. assignment
			; 
			; T0 -> *****
			; T1 -> *****
			; T2 -> npsamps
			; T3 -> rootindex
			;
			; XAR0 -> inptr + npsamps<<1 + 1
			; XAR1 -> ******
			; XAR2 -> ******
			; XAR3 -> ******
			; XAR4 -> ******
			; XAR5 -> input
			; XAR6 -> gainc0
			; XAR7 -> exgain[NPARTS]
			; XSP -> exgain
			;
			;--------------------------------------------------



			

			mar( AR0 - #1  )	
		}	; end of  for( i = 0 ; i < NPARTS ; i++)	
		
		SP = SP + #3
		; restore SP (deallocate fixwnd)
		
		AR0 = *SP
		; AR0 = exgain[0] 
		
		;--------------------------------------------------
		; current  assignments
		;
		; T0 -> *****
		; T1 -> *****
		; T2 -> npsamps
		; T3 -> rootindex
		;
		; XAR0 -> offset
		; XAR1 -> ******
		; XAR2 -> ******
		; XAR3 -> ******
		; XAR4 -> ******
		; XAR5 -> input
		; XAR6 -> gainc0
		; XAR7 -> exgain[NPARTS]
		; XSP -> exgain
		;
		;--------------------------------------------------

	; }
	goto GAIN0_PROCESS_LABEL
	
GAINC1_ELSE_LABEL:

	; else {
	
		;--------------------------------------------------
		; current  assignments
		;
		; T0 -> *****
		; T1 -> *****
		; T2 -> npsamps
		; T3 -> rootindex
		; AR4 -> nsamps << 1
		;
		; XAR0 -> input (long*)
		; XAR1 -> gainc0 
		; XAR2 -> gainc1 
		; XAR5 -> ******
		; XAR6 -> ******
		; XAR7 -> exgain
		; XSP -> exgain
		;
		;
		;--------------------------------------------------
		
		
		;--------------------------------------------------
		;
		; to assign (for call to Interpolate() )
		;
		; XAR0 -> inptr
		;
		; T0 -> 0 (gain0)
		; T1 -> 0 (gain1)
		; AR1 -> rootindex
		; AR2 -> npsamps
		;
		; XAR5 ->input
		; XAR6 -> gainc0
		; XAR7 -> exgain
		;
		;--------------------------------------------------
		XAR5 = XAR0
		; XAR5 -> input
	
		XAR6 = XAR1	
		; XAR6 -> gainc0
		
		AR0 = AR0 + AR4 
		; AR0(inptr) = input + (nsamps<<1)
		||
		T0 = INITi
		
		BRC0 = T0 
		; BRC0 = trip count (NPARTS-1)

		; allocate space for fixwnd on stack
		SP = SP - #3			; NOTE : aligment of SP
		; SP -> fixwnd
		; SP => even
		||
		blockrepeat {
		; for( i = NPARTS ; i > 0 ; i--)	{
		
			T0 = #0
			; T0 = 0
			; AR7 -> exgain[i+1]
			||
			AR1 = T3
			; AR1 = rootindex
			
			T1 = #0
			||
			AR2 = T2 
			; AR2 = npsamps
			
			
			
			;*****************************************************************
			; Inling of Interpolate function
			; Interpolate( inptr, 0, 0, rootindex, npsamps );
			; SP -> even
			;call _Interpolate		
			; AR0 = inptr + (npsamps<<1) + 1
			; subtract 1 to set AR0 = inptr + (npsamps<<1)
			

			;-----------------------------------------------------
			;
			; to assign
			;
			;  SP -> fixwnd		;fixwnd can be retained in AC0
			;
			;  XCDP -> fixwnd
			;  XAR4 -> fixfactor
			;
			;  T1 = T1 - T0  
			;-----------------------------------------------------
			
		
		;	pshboth(XCDP)
			; save for later restore
				
			; allocate space for fixwnd on stack
		;	SP = SP - #3			; NOTE : aligment of SP
			; SP -> fixwnd
			; SP => even
			
			
			T1 = T1 - T0
			; T1 -> gain1 - gain0
			||
			AC0 = 0x100
				
			; fixwnd = 0x100 << (gain0- 2*GAINMIN)
			T0 = T0 - GAINMIN2
			;(gain0- 2*GAINMIN)
			
			mar( AR2 - #1)
			; npsamps - 1
			||
			mar( *AR0+ )
			; AR0 -> inptr_l
			
			;bit(ST1, #10) = #1
			; set M40 bit
			; to allow 40 bit arithmetic
			;||	
			AC0 = AC0 <<< T0 
			;uns(0x100 << (gain0- 2*GAINMIN))
			; AC0 -> fixwnd
			
			XCDP = XSP
			; XCDP -> fixwnd
		
			
			dbl(*SP) = AC0 
			; store fixwnd on stack
			; XAR3 -> fixwnd
		
			BRC1 = AR2
			; BRC1 = npsamps - 1
			||
			if( T1 != 0 ) goto GAINC1_ELSE_ELSE_BLOCK	; if ( gain0 != gain1 )
			; gain0 == gain1
		
			XAR3 = XCDP				 
			mar( *AR3+)
			; AR3 -> fixwnd_l
			||
			; do npsamps times
			localrepeat{
				; to do 
				; RA_TNI_MULSHIFT0( inptr, fixwnd)
				; AC2 -> upper 32-bits of product
				T0 = #3
				; for incrementing inptr
				||
				; AR0 -> inptr_L
				; AR3 -> fixwnd _l
				AC2 = uns( *AR0- ) * uns( *AR3 )	; inptr_l * fixwnd_l
				; AR0 -> inptr_h
				; AR3 -> fixwnd_l
				; AC2 = (inptrl*fixwnd_l)>>16  +  xh*yl + xl * yh 
				AC2 = (AC2 >> #16) + ( (*AR0+) * uns(*AR3-) )	; inptr_h*fixwnd_l
				; AR0 -> inptr_l
				; AR1 -> fixwnd_h
				AC2 = AC2 + ( uns(*AR0-) * (*AR3) ) 	;inptr_l* fixwnd_h
				; AR0 -> inptr_h
				; AR3 -> fixwnd_h
				AC2= (AC2>>#16 ) + ( (*AR0) * (*AR3+) )   ; inptr_h*fixwnd_h
				; AR0 = inptr_h
				; AR3 -> fixwnd_l
				
				dbl(*(AR0+T0) ) = AC2
				; inptr++ = inptr*fixwnd
				; AR0 -> (inptr+2)_l
				
				
				
			}
				
			goto GAINC1_ELSE_END_INTERPOLATE
			; return if u please
						
GAINC1_ELSE_ELSE_BLOCK:			; gain0 != gain1
			
			; fixfactor = *( (RA_TNI_fixroot2ntab + rootindex *( 2*GAINDIF +1)*2 + (gain1 - gain0 + GAINDIF)*2 )
			; fixfactor = *( RA_TNI_fixroot2ntab + 2*GAINDIF*(2*rootindex +1) + 2*(gain1 - gain0 + rootindex) )
			XAR4 = _RA_TNI_fixroot2ntab
				
			AR1 = AR1 << #1 
			; AR1 -> 2 * rootindex : for dword addressing
			||
			mar( T1 + T1 )
			; T1 = (gain1 - gain0 )*2 : for dword addressing
			
			mar( T1 + AR1 )
			; T1 -> (gain1 - gain0 + rootindex)*2
			||
			mar( AR1 + #1)
			; AR1 = 2*rootindex + 1
			T0 = AR1
			; T0 = 2*rootindex + 1
			||
			AC1 = T1
			; AC1 -> (gain1 - gain0 + rootindex)*2 
			
			AC1 = AC1 + (T0 * GAINDIF2)
			; AC1 -> (gain1 - gain0 + rootindex)*2 + 2*GAINDIF*(2*rootindex+1)
			
				
			AR4 = AR4 + AC1
			; XAR4 = RA_TNI_fixroot2ntab + 2*GAINDIF*(2*rootindex +1) + 2*(gain1 - gain0 + rootindex) 
			||
			mar( *CDP+)
			; CDP -> fixwnd_l
			
			
			mar(*AR4+)
			; AR4 -> fixfactor_l
			||
			; do npsamps times
			localrepeat{
					
				; AR0 -> inptr_l
				; CDP -> fixwnd_l
				; AR4 -> fixfactor_l
				
				; to calculate
				;   (inptr * fixwnd) || (fixfactor*fixwnd)
				; AC2 = inptr*fixwnd
				; AC1 = upper 32 bits of fixfactor*fixwnd
				; AC3 = lower 32 bits of fixfactor*fixwnd
				;	
					
				T0 = #3
				; for incrementing inptr
				||
				; AR0 -> inptr_L
				; AR3 -> fixwnd _l
				AC2 = uns( *AR0- ) * uns(coef( *CDP) ),	; inptr_l * fixwnd_l
					AC3 = uns( *AR4- ) * uns (coef(*CDP)) 		; fixfactor_l * fixwnd_l
				; AR0 -> inptr_h
				; CDP -> fixwnd_l
				; AR4 -> fixfactor_h
				
				T1 = -1
				; T1 = 0xFFFF
				||
				AC2 = (AC2 >> #16) + ( (*AR0+) * uns(coef(*CDP-) )),	; inptr_h*fixwnd_l
					AC3 = (AC3 >> #16) + ( (*AR4+) * uns(coef(*CDP-) ))	; fixfactor_h*fixwnd_l
		
				; AR0 -> inptr_l
				; CDP -> fixwnd_h
				; AR4 -> fixfactor_l
				AC2 = AC2 + ( uns(*AR0-) * coef(*CDP) ), 	;inptr_l* fixwnd_h
					AC3 = AC3 + ( uns(*AR4-) * coef(*CDP) ) 	;fixfactor_l* fixwnd_h
				; AR0 -> inptr_h
				; CDP -> fixwnd_h
				; AR4 -> fixfactor_h
				
				AC1 = AC3 
								
				AC2= (AC2>>#16 ) + ( (*AR0) * coef(*CDP) ),   ; inptr_h*fixwnd_h
					AC1= (AC1>>#16 ) + ( (*AR4+) * coef(*CDP) )   ; fixfactor_h*fixwnd_h
				
				; AR0 = inptr_h
				; CDP -> fixwnd_h
				; AR4 -> fixfactor_l
					
				dbl(*(AR0 + T0)) = AC2 
				; inptr = inptr * fixwnd
				||
				; AR0 -> (inptr+2)_l
				AC3 = AC3 & T1
				; isolate bits 31-16 of product fixfactor* fixwnd
				AC1 = AC1<<#2
				; shifting of (AC1:AC3) << 2
				AC1 = AC1 | (AC3 <<< #-14 )
				
				dbl(*CDP) = AC1
				; fixwnd = (fixwnd*fixfactor)<<2
				
				mar(*CDP+)
				; CDP -> fixwnd_l
				
			}
			
		
			;-----------------------------------------------------
			; cleanup code
GAINC1_ELSE_END_INTERPOLATE:	
		;	SP = SP + #3
		;	XCDP = popboth()
			; save for later restore
			
		
			; End of Interpolate	
			;*****************************************************************
		
			
			
			mar( AR0 - #1 )	
			; AR0 = inptr + npsamps<<1 (for dword addressing) 
			
		}		; end of for( i = NPARTS ; i > 0 ; i--)
		
		SP = SP + #3
		; deallocate fixwnd from stack
		
		AR0 = 0
		; offset = 0
		;--------------------------------------------------
		; current  assignments
		;
		; T0 -> *****
		; T1 -> *****
		; T2 -> npsamps
		; T3 -> rootindex
		;
		; XAR0 -> offset
		; XAR1 -> ******
		; XAR2 -> ******
		; XAR3 -> ******
		; XAR4 -> ******
		; XAR5 -> input
		; XAR6 -> gainc0
		; XAR7 -> exgain
		; XSP -> exgain
		;
		;
		;--------------------------------------------------

	
	;}
	
	;*****************************************************************
	;End of  Second part : operations using gainc1
	;*****************************************************************

	;-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
GAIN0_PROCESS_LABEL:
	;*****************************************************************
	;First part : operations using gainc0
	;*****************************************************************

	;--------------------------------------------------
	; current  assignments
	;
	; T0 -> *****
	; T1 -> *****
	; T2 -> npsamps
	; T3 -> rootindex
	;
	; XAR0 -> offset
	; XAR1 -> ******
	; XAR2 -> ******
	; XAR3 -> ******
	; XAR4 -> ******
	; XAR5 -> input
	; XAR6 -> gainc0
	; XAR7 -> ******
	; XSP -> exgain
	;
	;
	;--------------------------------------------------


	;--------------------------------------------------
	; to assign
	; T0 -> temp
	;
	; XAR3 -> gainc0->loc
	; XAR4 -> gainc0->gain
	; XAR7 -> exgain
	; XSP -> exgain
	;
	; AC1 -> nats
	

	AC1 = *AR6
	; AC1 (nats) = gainc0->nats
	
	XAR7 = XSP
	; XAR7 -> exgain
	*SP(#NPARTS) = #0
	; exgain[NPARTS] = 0

	if( AC1 == 0 ) goto GAINC0_ELSE_LABEL    ;if (nats == 0 ) 
	
	; if(nats) {
	
		;--------------------------------------------------
		; current  assignments
		;
		; T0 -> *****
		; T1 -> *****
		; T2 -> npsamps
		; T3 -> rootindex
		;
		;
		; XAR5 -> input
		; XAR6 -> gainc0
		; XAR7 -> exgain
		; XSP -> exgain
		;
		; AC1 -> nats
		;
		;--------------------------------------------------
	
		T1 = INITi 
		; T1(i) = NPARTS - 1
				
		XAR3 = XAR6
		; XAR5 -> gainc0
		XAR4 = XAR6 
		; XAR6 -> gainc0
		||
		AC0 = #0
			
		mar(AR7 + NPARTS )
		; AR7 -> exgain[NPARTS] 
	
		BRC0 = T1
		; trip count = NPARTS - 1
		||
		mar( AR3 + #(GAINC.loc))
		; XAR3 -> gainc0->loc
		
		mar( AR4 + #(GAINC.gain))
		; XAR4 -> gainc0->gain
		||		
		localrepeat{ 	; do NPARTS times
			
			T0 = AC1 - #1
			; T0 = nats - 1
			AC2 =  *AR3(T0)
			; AC2 =  gainc0->loc[nats-1]
			||			
			AC3 = *AR7- 
			; AC3 = exgain[i+1]
			; AR7 -> exgain[i]
			
			TC1 = ( AC1 != AC0 )
			; set TC1  if ( nats != 0 )	
			||
			TC2 = ( T1 == AC2)
			; set TC2 		if( i == gainc1->loc[nats-1] )
			
			T1 = T1 - #1
			; T1 = i - 1 (for next loop)
			||
			*AR7 = AC3
			; exgain[i] = exgain[i+1]
			
			.noremark 5590, 5503
			if( TC1 & TC2  ) execute (AD_Unit)			; if( nats && ( i == gainc1->loc[nats-1] ) )
			 	*AR7 = *AR4(T0)							;			exgain[i] = gainc1->gain[nats-1]
			 	||										;			
			 	AC1 = AC1 - #1							;			AC1(nats) = nats - 1
			.remark 5590	
			.remark 5503
			 				 				 	
		}			
	
	
		;-------------------------------------------------
		; to assign (for call to Interpolate() )
		;
		; XAR0 -> inptr
		;
		; T0 -> exgain[i] (gain0)
		; T1 -> exgain[i+1] (gain1)
		; AR1 -> rootindex
		; AR2 -> npsamps
		;
		; XAR5 ->input
		; XAR6 -> offset
		;
		;--------------------------------------------------
	
	
		AR6 = AR0
		; AR6 = offset
		||
		T0 = INITi
		; Trip count (NPARTS-1) in T0
		
		XAR0 = XAR5
		; XAR0(inptr) -> input
				
		;*(temp_count) = T0 
		; load temporary count		
		BRC0 = T0
		; trip count in BRC0	
	
		; allocate space for fixwnd on stack
		SP = SP - #3			; NOTE : aligment of SP
		; SP -> fixwnd
		; SP => even
		||
		blockrepeat{
		; for( i = 0 ; i < NPARTS ; i++)	{
		
			T0 = (*AR7+) + AR6
			; T0 = exgain[i] + offset
			; AR7 -> exgain[i+1]
			
			
			AR1 = T3
			; AR1 = rootindex
			
			T1 = *AR7 + AR6
			; T1 = exgain[i+1] + offset
			
			AR2 = T2 
			; AR2 = npsamps
			
			;*****************************************************************
			; Inling of Interpolate function
			; Interpolate( inptr, exgain[i], exgain[i+1], rootindex, npsamps );
			; SP -> even
			;call _Interpolate		
			; AR0 = inptr + (npsamps<<1) + 1
			; subtract 1 to set AR0 = inptr + (npsamps<<1)
			

			;-----------------------------------------------------
			;
			; to assign
			;
			;  SP -> fixwnd		;fixwnd can be retained in AC0
			;
			;  XCDP -> fixwnd
			;  XAR4 -> fixfactor
			;
			;  T1 = T1 - T0  
			;-----------------------------------------------------
			
		
		;	pshboth(XCDP)
			; save for later restore
				
			; allocate space for fixwnd on stack
		;	SP = SP - #3			; NOTE : aligment of SP
			; SP -> fixwnd
			; SP => even
			
			
			T1 = T1 - T0
			; T1 -> gain1 - gain0
			||
			AC0 = 0x100
				
			; fixwnd = 0x100 << (gain0- 2*GAINMIN)
			T0 = T0 - GAINMIN2
			;(gain0- 2*GAINMIN)
			
			mar( AR2 - #1)
			; npsamps - 1
			||
			mar( *AR0+ )
			; AR0 -> inptr_l
			
			AC0 = AC0 <<< T0 
			;uns(0x100 << (gain0- 2*GAINMIN))
			; AC0 -> fixwnd
			
			XCDP = XSP
			; XCDP -> fixwnd
		
			
			dbl(*SP) = AC0 
			; store fixwnd on stack
			; XAR3 -> fixwnd
		
			BRC1 = AR2
			; BRC1 = npsamps - 1
			||
			if( T1 != 0 ) goto GAINC0_IF_ELSE_BLOCK	; if ( gain0 != gain1 )
			; gain0 == gain1
		
			XAR3 = XCDP				 
			mar( *AR3+)
			; AR3 -> fixwnd_l
			||
			; do npsamps times
			localrepeat{
				; to do 
				; RA_TNI_MULSHIFT0( inptr, fixwnd)
				; AC2 -> upper 32-bits of product
				T0 = #3
				; for incrementing inptr
				||
				; AR0 -> inptr_L
				; AR3 -> fixwnd _l
				AC2 = uns( *AR0- ) * uns( *AR3 )	; inptr_l * fixwnd_l
				; AR0 -> inptr_h
				; AR3 -> fixwnd_l
				; AC2 = (inptrl*fixwnd_l)>>16  +  xh*yl + xl * yh 
				AC2 = (AC2 >> #16) + ( (*AR0+) * uns(*AR3-) )	; inptr_h*fixwnd_l
				; AR0 -> inptr_l
				; AR1 -> fixwnd_h
				AC2 = AC2 + ( uns(*AR0-) * (*AR3) ) 	;inptr_l* fixwnd_h
				; AR0 -> inptr_h
				; AR3 -> fixwnd_h
				AC2= (AC2>>#16 ) + ( (*AR0) * (*AR3+) )   ; inptr_h*fixwnd_h
				; AR0 = inptr_h
				; AR3 -> fixwnd_l
				
				dbl(*(AR0+T0) ) = AC2
				; inptr++ = inptr*fixwnd
				; AR0 -> (inptr+2)_l
				
				
				
			}
				
			goto GAINC0_IF_END_INTERPOLATE
			; return if u please
						
GAINC0_IF_ELSE_BLOCK:			; gain0 != gain1
			
			; fixfactor = *( (RA_TNI_fixroot2ntab + rootindex *( 2*GAINDIF +1)*2 + (gain1 - gain0 + GAINDIF)*2 )
			; fixfactor = *( RA_TNI_fixroot2ntab + 2*GAINDIF*(2*rootindex +1) + 2*(gain1 - gain0 + rootindex) )
			XAR4 = _RA_TNI_fixroot2ntab
				
			AR1 = AR1 << #1 
			; AR1 -> 2 * rootindex : for dword addressing
			||
			mar( T1 + T1 )
			; T1 = (gain1 - gain0 )*2 : for dword addressing
			
			mar( T1 + AR1 )
			; T1 -> (gain1 - gain0 + rootindex)*2
			||
			mar( AR1 + #1)
			; AR1 = 2*rootindex + 1
			T0 = AR1
			; T0 = 2*rootindex + 1
			||
			AC1 = T1
			; AC1 -> (gain1 - gain0 + rootindex)*2 
			
			AC1 = AC1 + (T0 * GAINDIF2)
			; AC1 -> (gain1 - gain0 + rootindex)*2 + 2*GAINDIF*(2*rootindex+1)
			
				
			AR4 = AR4 + AC1
			; XAR4 = RA_TNI_fixroot2ntab + 2*GAINDIF*(2*rootindex +1) + 2*(gain1 - gain0 + rootindex) 
			||
			mar( *CDP+)
			; CDP -> fixwnd_l
			
			
			mar(*AR4+)
			; AR4 -> fixfactor_l
			||
			; do npsamps times
			localrepeat{
					
				; AR0 -> inptr_l
				; CDP -> fixwnd_l
				; AR4 -> fixfactor_l
				
				; to calculate
				;   (inptr * fixwnd) || (fixfactor*fixwnd)
				; AC2 = inptr*fixwnd
				; AC1 = upper 32 bits of fixfactor*fixwnd
				; AC3 = lower 32 bits of fixfactor*fixwnd
				;	
					
				T0 = #3
				; for incrementing inptr
				||
				; AR0 -> inptr_L
				; AR3 -> fixwnd _l
				AC2 = uns( *AR0- ) * uns(coef( *CDP) ),	; inptr_l * fixwnd_l
					AC3 = uns( *AR4- ) * uns (coef(*CDP)) 		; fixfactor_l * fixwnd_l
				; AR0 -> inptr_h
				; CDP -> fixwnd_l
				; AR4 -> fixfactor_h
				
				T1 = -1
				; T1 = 0xFFFF
				||
				AC2 = (AC2 >> #16) + ( (*AR0+) * uns(coef(*CDP-) )),	; inptr_h*fixwnd_l
					AC3 = (AC3 >> #16) + ( (*AR4+) * uns(coef(*CDP-) ))	; fixfactor_h*fixwnd_l
		
				; AR0 -> inptr_l
				; CDP -> fixwnd_h
				; AR4 -> fixfactor_l
				AC2 = AC2 + ( uns(*AR0-) * coef(*CDP) ), 	;inptr_l* fixwnd_h
					AC3 = AC3 + ( uns(*AR4-) * coef(*CDP) ) 	;fixfactor_l* fixwnd_h
				; AR0 -> inptr_h
				; CDP -> fixwnd_h
				; AR4 -> fixfactor_h
				
				AC1 = AC3 
								
				AC2= (AC2>>#16 ) + ( (*AR0) * coef(*CDP) ),   ; inptr_h*fixwnd_h
					AC1= (AC1>>#16 ) + ( (*AR4+) * coef(*CDP) )   ; fixfactor_h*fixwnd_h
				
				; AR0 = inptr_h
				; CDP -> fixwnd_h
				; AR4 -> fixfactor_l
					
				dbl(*(AR0 + T0)) = AC2 
				; inptr = inptr * fixwnd
				||
				; AR0 -> (inptr+2)_l
				AC3 = AC3 & T1
				; isolate bits 31-16 of product fixfactor* fixwnd
				AC1 = AC1<<#2
				; shifting of (AC1:AC3) << 2
				AC1 = AC1 | (AC3 <<< #-14 )
				
				dbl(*CDP) = AC1
				; fixwnd = (fixwnd*fixfactor)<<2
				
				mar(*CDP+)
				; CDP -> fixwnd_l
				
			}
			
		
			;-----------------------------------------------------
			; cleanup code
GAINC0_IF_END_INTERPOLATE:	
		;	SP = SP + #3
		;	XCDP = popboth()
			; save for later restore
			
		
			; End of Interpolate	
			;*****************************************************************
			

			mar( AR0 - #1 )	

		} 		; end of for( i = 0 ; i < NPARTS ; i++)	
	; }
		SP = SP + #3
		; deallocate fixwnd off stack
		goto END_GAINWINDOW
	
GAINC0_ELSE_LABEL:

	; else {
	
		;--------------------------------------------------
		; current  assignments
		;
		; T0 -> *****
		; T1 -> *****
		; T2 -> npsamps
		; T3 -> rootindex
		;
		; AR0 -> offset
		;
		; XAR5 -> input
		; XAR6 -> gainc0
		; XAR7 -> exgain
		; XSP -> exgain
		;
		;--------------------------------------------------
		
		;--------------------------------------------------
		;
		; to assign (for call to Interpolate() )
		;
		; XAR0 -> inptr
		;
		; T0 -> offset(gain0)
		; T1 -> offset (gain1)
		; AR1 -> rootindex
		; AR2 -> npsamps
		;
		; AR6 -> offset
		;--------------------------------------------------
		
		AR6 = AR0 
		; AR6 = offset
		||
		T0 = INITi
		; T0 = NPARTS -1
		
		BRC0 = T0
		; trip count of NPARTS-1 in BRC0
							
		XAR0 = XAR5
		; XAR0 (inptr) -> input
					
		; allocate space for fixwnd on stack
		SP = SP - #3			; NOTE : aligment of SP
		; SP -> fixwnd
		; SP => even
		||	
		blockrepeat{
		; for( i = NPARTS ; i > 0 ; i--)	{
			T0 = AR6
			; T0 ->offset
			
			AR1 = T3
			; AR1 = rootindex
			
			T1 = AR6
			; T1 -> offset
			
			AR2 = T2 
			; AR2 = npsamps
			
			
			;*****************************************************************
			; Inling of Interpolate function
			; Interpolate( inptr, 0, 0, rootindex, npsamps );
			; SP -> even
			;call _Interpolate		
			; AR0 = inptr + (npsamps<<1) + 1
			; subtract 1 to set AR0 = inptr + (npsamps<<1)
			

			;-----------------------------------------------------
			;
			; to assign
			;
			;  SP -> fixwnd		;fixwnd can be retained in AC0
			;
			;  XCDP -> fixwnd
			;  XAR4 -> fixfactor
			;
			;  T1 = T1 - T0  
			;-----------------------------------------------------
			
		
		;	pshboth(XCDP)
			; save for later restore
				
			; allocate space for fixwnd on stack
		;	SP = SP - #3			; NOTE : aligment of SP
			; SP -> fixwnd
			; SP => even
			
			
			T1 = T1 - T0
			; T1 -> gain1 - gain0
			||
			AC0 = 0x100
				
			; fixwnd = 0x100 << (gain0- 2*GAINMIN)
			T0 = T0 - GAINMIN2
			;(gain0- 2*GAINMIN)
			
			mar( AR2 - #1)
			; npsamps - 1
			||
			mar( *AR0+ )
			; AR0 -> inptr_l
			
			AC0 = AC0 <<< T0 
			;uns(0x100 << (gain0- 2*GAINMIN))
			; AC0 -> fixwnd
			
			XCDP = XSP
			; XCDP -> fixwnd
		
			
			dbl(*SP) = AC0 
			; store fixwnd on stack
			; XAR3 -> fixwnd
		
			BRC1 = AR2
			; BRC1 = npsamps - 1
			||
			if( T1 != 0 ) goto GAINC0_ELSE_ELSE_BLOCK	; if ( gain0 != gain1 )
			; gain0 == gain1
		
			XAR3 = XCDP				 
			mar( *AR3+)
			; AR3 -> fixwnd_l
			||
			; do npsamps times
			localrepeat{
				; to do 
				; RA_TNI_MULSHIFT0( inptr, fixwnd)
				; AC2 -> upper 32-bits of product
				T0 = #3
				; for incrementing inptr
				||
				; AR0 -> inptr_L
				; AR3 -> fixwnd _l
				AC2 = uns( *AR0- ) * uns( *AR3 )	; inptr_l * fixwnd_l
				; AR0 -> inptr_h
				; AR3 -> fixwnd_l
				; AC2 = (inptrl*fixwnd_l)>>16  +  xh*yl + xl * yh 
				AC2 = (AC2 >> #16) + ( (*AR0+) * uns(*AR3-) )	; inptr_h*fixwnd_l
				; AR0 -> inptr_l
				; AR1 -> fixwnd_h
				AC2 = AC2 + ( uns(*AR0-) * (*AR3) ) 	;inptr_l* fixwnd_h
				; AR0 -> inptr_h
				; AR3 -> fixwnd_h
				AC2= (AC2>>#16 ) + ( (*AR0) * (*AR3+) )   ; inptr_h*fixwnd_h
				; AR0 = inptr_h
				; AR3 -> fixwnd_l
				
				dbl(*(AR0+T0) ) = AC2
				; inptr++ = inptr*fixwnd
				; AR0 -> (inptr+2)_l
				
				
				
			}
				
			goto GAINC0_ELSE_END_INTERPOLATE
			; return if u please
						
GAINC0_ELSE_ELSE_BLOCK:			; gain0 != gain1
			
			; fixfactor = *( (RA_TNI_fixroot2ntab + rootindex *( 2*GAINDIF +1)*2 + (gain1 - gain0 + GAINDIF)*2 )
			; fixfactor = *( RA_TNI_fixroot2ntab + 2*GAINDIF*(2*rootindex +1) + 2*(gain1 - gain0 + rootindex) )
			XAR4 = _RA_TNI_fixroot2ntab
				
			AR1 = AR1 << #1 
			; AR1 -> 2 * rootindex : for dword addressing
			||
			mar( T1 + T1 )
			; T1 = (gain1 - gain0 )*2 : for dword addressing
			
			mar( T1 + AR1 )
			; T1 -> (gain1 - gain0 + rootindex)*2
			||
			mar( AR1 + #1)
			; AR1 = 2*rootindex + 1
			T0 = AR1
			; T0 = 2*rootindex + 1
			||
			AC1 = T1
			; AC1 -> (gain1 - gain0 + rootindex)*2 
			
			AC1 = AC1 + (T0 * GAINDIF2)
			; AC1 -> (gain1 - gain0 + rootindex)*2 + 2*GAINDIF*(2*rootindex+1)
			
				
			AR4 = AR4 + AC1
			; XAR4 = RA_TNI_fixroot2ntab + 2*GAINDIF*(2*rootindex +1) + 2*(gain1 - gain0 + rootindex) 
			||
			mar( *CDP+)
			; CDP -> fixwnd_l
			
			
			mar(*AR4+)
			; AR4 -> fixfactor_l
			||
			; do npsamps times
			localrepeat{
					
				; AR0 -> inptr_l
				; CDP -> fixwnd_l
				; AR4 -> fixfactor_l
				
				; to calculate
				;   (inptr * fixwnd) || (fixfactor*fixwnd)
				; AC2 = inptr*fixwnd
				; AC1 = upper 32 bits of fixfactor*fixwnd
				; AC3 = lower 32 bits of fixfactor*fixwnd
				;	
					
				T0 = #3
				; for incrementing inptr
				||
				; AR0 -> inptr_L
				; AR3 -> fixwnd _l
				AC2 = uns( *AR0- ) * uns(coef( *CDP) ),	; inptr_l * fixwnd_l
					AC3 = uns( *AR4- ) * uns (coef(*CDP)) 		; fixfactor_l * fixwnd_l
				; AR0 -> inptr_h
				; CDP -> fixwnd_l
				; AR4 -> fixfactor_h
				
				T1 = -1
				; T1 = 0xFFFF
				||
				AC2 = (AC2 >> #16) + ( (*AR0+) * uns(coef(*CDP-) )),	; inptr_h*fixwnd_l
					AC3 = (AC3 >> #16) + ( (*AR4+) * uns(coef(*CDP-) ))	; fixfactor_h*fixwnd_l
		
				; AR0 -> inptr_l
				; CDP -> fixwnd_h
				; AR4 -> fixfactor_l
				AC2 = AC2 + ( uns(*AR0-) * coef(*CDP) ), 	;inptr_l* fixwnd_h
					AC3 = AC3 + ( uns(*AR4-) * coef(*CDP) ) 	;fixfactor_l* fixwnd_h
				; AR0 -> inptr_h
				; CDP -> fixwnd_h
				; AR4 -> fixfactor_h
				
				AC1 = AC3 
								
				AC2= (AC2>>#16 ) + ( (*AR0) * coef(*CDP) ),   ; inptr_h*fixwnd_h
					AC1= (AC1>>#16 ) + ( (*AR4+) * coef(*CDP) )   ; fixfactor_h*fixwnd_h
				
				; AR0 = inptr_h
				; CDP -> fixwnd_h
				; AR4 -> fixfactor_l
					
				dbl(*(AR0 + T0)) = AC2 
				; inptr = inptr * fixwnd
				||
				; AR0 -> (inptr+2)_l
				AC3 = AC3 & T1
				; isolate bits 31-16 of product fixfactor* fixwnd
				AC1 = AC1<<#2
				; shifting of (AC1:AC3) << 2
				AC1 = AC1 | (AC3 <<< #-14 )
				
				dbl(*CDP) = AC1
				; fixwnd = (fixwnd*fixfactor)<<2
				
				mar(*CDP+)
				; CDP -> fixwnd_l
				
			}
			
		
			;-----------------------------------------------------
			; cleanup code
GAINC0_ELSE_END_INTERPOLATE:	
		;	SP = SP + #3
		;	XCDP = popboth()
			; save for later restore
			
		
			; End of Interpolate	
			;*****************************************************************		
			
			mar( AR0 - #1 )	
			; AR0 -> inptr + npsamps << 1
			
		 }; enf of for( i = NPARTS ; i > 0 ; i--)	
		 
		SP = SP + #3
		; deallocate fixwnd from stack	
	;}

	;*****************************************************************
	;End of First part : operations using gainc0
	;*****************************************************************

END_GAINWINDOW:




	;------------------------------------------------
	; epilog code
	
	SP = SP + #EXGAIN_SIZE
	; deallocate space for exgain[NPARTS + 1] on stack
	
	XCDP = popboth()
	XAR7 = popboth()
	XAR6 = popboth()
	XAR5 = popboth()
	
	T2, T3 = pop()
	; restore and exit
	||
	bit(ST2, #15) = #1
	; set the ARMS bit to use Control Mode addressing
	.ARMS_on
	
	@#0x03 = pop()||mmap()
	.remark 5573
	return 


;-----------------------------------------------------------------------------------------------------
; end of RA_TNI_GainWindow
	.endif    		; GAINWINDOW_ASM

