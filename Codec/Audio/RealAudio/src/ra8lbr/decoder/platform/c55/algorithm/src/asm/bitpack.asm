; ***** BEGIN LICENSE BLOCK *****   
; Source last modified: $Id: bitpack.asm,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
; 	File 		:	Bitpack.asm 
;   Description : C55X assembly routines for functions present in Bitpack.c
;	
;------------------------------------------------------------------------------------------------------------
;/*
; * Gecko2 stereo audio codec.
; * Developed by Ken Cooke (kenc@real.com)
; * August 2000
; *
; * Last modified by:	Firdaus Janoos (Texas Instruments, India)
; *						12/09/01
; *
; * Bitpacking functions in assembly.
; */

	.if UNPACKBIT_ASM			; to enable assembly version of RA_TNI_Unpackbits and RA_TNI_Unpackbit
	.include "coder.h55"
	.mmregs
	.def _RA_TNI_Unpackbits
	.def _RA_TNI_DecodeBytes		;	NOT IMPLEMENTED

;----------------------------------------------------------------------------------
; _RA_TNI_Unpackbits : C callable function
; 
;			
;/*
; * Unpack bits from stream.
; * Returns nbits if successful, or zero if out-of-bits.
; * NOTES:
; * out-of-bits not needed here?
; *
; */	
;
; Input : T0 -> nbits
; 		  XAR0-> data (USHORT*)
;		  XAR1-> pkbit (short*)
;		  XAR2-> pktotal (short*)
;		  XAR3-> bitstrmPtr (RA_TNI_Obj*)
;
; Return : T0 -> nbits
;
;




_RA_TNI_Unpackbits:

	;-----------------------------------------------------
	;
	; to assign
	;
	; AC0 -> *pktotal + nbits
	; AC1 -> (*pktotal + nbits) - bitstrmPtr->nframebits
	; AC2 -> *(bitstrmPtr->pkptr)
	; AC3 -> temp
	;
	; XAR4 -> bitstrmPtr->pkptr
	;
	; T0 -> return value, *pkbit
	; T1 -> nbits
	;-----------------------------------------------------
	.noremark 5673
	.noremark 5573
	.noremark 5571
	AC0 = *AR2 + T0
	; AC0 = *pktotal + nbits
	||
	T1 = T0
	; T1 -> nbits

	AC1 = AC0 - *AR3(#(RA_TNI_Obj.nframebits))
	;AC1 = AC0 - bitstrmPtr->nframebits


	mar( AR3 + #(RA_TNI_Obj.pkptr) )
	; AR3 = bitstrmPtr + RA_TNI_Obj.pkptr

	XAR4 =dbl(*AR3)
	; XAR3 = bitstrmPtr->pkptr
	
	*AR2 = AC0
	; *pktotal = *pktotal + nbits
	||
	T0 = #0
	; T0 -> *pkbit
	
	if( AC1 > 0 ) return		; if( AC0 > bitstrmPtr->nframebits)
		 						;		return 0 
	
	T0 = *AR1
	; T0 -> *pkbit
	
	AC2 = dbl(*AR4)
	; AC2 -> *(bitstrmPtr->pkptr) 
	||
	AR2 = T1
	; AR2 = nbits
	
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																											
	AC3 = AC2 << T0
	; AC3 (temp) = (*(bitstrmPtr->pkptr) ) << *pkbit
	||
	T0 = T0 + T1 
	*AR1 = T0
	; *pkbit = *pkbit + nbits

	mar(T0 - #32)
	; T0 = *pkbit - 32
	||
	AR4 =  AR4 + #2
	;AR4 = bitstrmPtr->pkptr +  #2
	
	T1 = T1 - T0 
	; T1 = (nbits - *pkbits)
	||
	if( T0 < #0 ) goto EXIT_LABEL   ; if *pkbit < 32 
	
	;if( *pkbit >= 32) {
		AC2 = dbl(*AR4)
		; AC2 -> bitstrmPtr->pkptr
		dbl(*AR3) = XAR4 
		; bitstrmPtr->pkptr = bitstrmPtr->pkptr + #2
		||
		T1 = -T1
		; T1 = -(nbits - *pkbits ) for right shift

		AC2 = AC2<<<T1
		; AC2 = *(bitstrmPtr->pkptr)>> (nbits - *pkbits)
		||
		*AR1 = T0
		; *pkbit = *pkbit - 32 
		nop;added to supress silicon exception causing remark 5505
		.noremark 5505
		.noremark 5503
		if( T0 != #0 ) execute (AD_Unit) ; to elimate CPU_30
		; if ( *pkbit)
			AC3 = AC3 | AC2
			; temp = temp | (*(bitstrmPtr->pkptr)>> (nbits - *pkbits) )
		.remark 5503
	;}
		
EXIT_LABEL:
	; case : *pkbit < 32
	

	T1 = AR2 - #32
	; T1 = -( 32 - nbits ) for right shifting
	
	T0 = AR2
	; T0 = nbits
	||
	AC3 = AC3 <<< T1
	; AC3 = temp >>> ( 32 - nbits )
	
	*AR0 = AC3
	; *data = USHORT( temp >> (32-nbits) )
	||								
	return 
	;return nbits
	
; _RA_TNI_Unpackbits
;----------------------------------------------------------------------------------------------

;	.endif 		; UNPACKBIT_ASM


	.if !HUFFTABLE_ASM		; last place this is needed
;----------------------------------------------------------------------------------
; _RA_TNI_Unpackbit : C callable function
; 
;			
;/*
; * Unpack bits from stream.
; * Returns nbits if successful, or zero if out-of-bits.
; * NOTES:
; * out-of-bits not needed here?
; *
; */	
;
; Input : XAR0-> data (USHORT*)
;		  XAR1-> pkbit (short*)
;		  XAR2-> pktotal (short*)
;		  XAR3-> bitstrmPtr (RA_TNI_Obj*)
;
; Return : T0 -> nbits
;
;

     	.def _RA_TNI_Unpackbit

_RA_TNI_Unpackbit:

	;-----------------------------------------------------
	;
	; to assign
	;
	; AC0 -> *pktotal - bitstrmPtr->nframebits
	; AC1 -> *(bitstrmPtr->pkptr)
	;
	; XAR4 -> bitstrmPtr->pkptr
	;
	; T0 -> return value, *pkbit
	; T1 -> nbits
	;-----------------------------------------------------	
	
	AC0 = *AR3(#(RA_TNI_Obj.nframebits) )
	; AC0 = bitstrmPtr->nframebits
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																									
	*AR2 = *AR2 + #1
	; ++(*pktotal)
				
	mar(T0 = #0)
	; return 0
	||
	AC0 = *AR2 - AC0
	; AC0 = *pktotal - bitstrmPtr->nframebits
	
	mar( AR3 + #(RA_TNI_Obj.pkptr) )
	; AR3 = bitstrmPtr + RA_TNI_Obj.pkptr
	||
	T1 = *AR1
	; T1 = *pkbit

	if(AC0 > 0 )		 return 
	; if (*pktotal > bitstrmPtr->nframebits ) 
	;		return 0

			
	XAR4 =dbl(*AR3)
	; XAR4 = bitstrmPtr->pkptr
	||
	mar(T1 - #31)
	; T1 = *pkbit - 31
	
	AC1 = dbl(*AR4)
	; AC1 = *(bitstrmPtr->pkptr)
	||
	T0 = #1
	; return 1
	
	*AR1 = *AR1 + #1
	; ++(*pkbit)
		
	AC2 = *AR1
	; AC2 = *pkbit
	||
	AC1 = AC1 <<< T1 
	;  AC1 = *(bitstrmPtr->pkptr) <<< (*pkbit - 31)
	
	
	AC2 = AC2 << #-4
	bit( AC2, @0 ) = #0
	; AC2 =(*pkbit >> 5 ) <<1  : for dword addressing
	
	AC1 = AC1 & 0x01
	;  AC1 = *(bitstrmPtr->pkptr) <<< (*pkbit)>>>31
	
	*AR0 = AC1
	; *data = AC1
	||
	AR4 = AR4 + AC2
	; AR4 = bitstrmPtr->pkptr + (*pkbit >> 5)
	
	*AR1 = *AR1 & 0x1F
	; *pkbit &= 0x1F
	
	dbl(*AR3 ) = XAR4
	; bitstrmPtr->pkptr = XAR4
	||
	return 
	; return 1 
		
; _RA_TNI_Unpackbit
;----------------------------------------------------------------------------------------------
	.endif 		; HUFFTABLE_ASM

	.endif 		; UNPACKBIT_ASM
	
	




;----------------------------------------------------------------------------------
; _RA_TNI_DecodeBytes : C callable function
; 
;
;/*
; * Data comes in a byte at a time (always big-endian) and is reorganized here
; *   into ints with whatever byte order the machine uses.
; * Also has simple scrambling scheme of XOR with byte masks
; *
; * Fixed-point changes:
; *  - none
; */
;
; Input : XAR0-> codebuf (USHORT*)
;		  XAR1-> pkbuf (ULONG*)
;		  T0 -> nbits (short)
;
; Return : T0 -> voids
;
;

	
DEC_BYTES_OLD .set   0x00
	
	.if DEC_BYTES_OLD 		

	.global _RA_TNI_pkmask			;const long RA_TNI_pkmask[2] = { 0x0000ffff, 0xffff0000 };
	.global _RA_TNI_pkshift		;const short RA_TNI_pkshift[2] = { 16, 0 };
	.global _RA_TNI_pkinc			;const short RA_TNI_pkinc[2] = { 0, 1 };
	.global _RA_TNI_pkkey			;const USHORT RA_TNI_pkkey[2] = { 0x37c5, 0x11f2 };
	.global _RA_TNI_pkkey4			;const USHORT RA_TNI_pkkey4[4] = { 0x3700, 0xc500, 0x1100, 0xf200 };

	; old version of RA_TNI_DecodeBytes
_RA_TNI_DecodeBytes:

	;-----------------------------------------------------
	;	To Assign :
	;
	;	XAR0 -> codebuf
	;	XAR1 -> pkbuf
	;   XAR3 -> RA_TNI_pkkey
	;   XAR4 -> RA_TNI_pkkey4
	;
	;	T0 ->  idx
	;	T1 ->  nbytes
	;   AR2 -> *RA_TNI_pkinc[idx]  { 0 or 2 }
	;   AC0 -> curSh
	; 	AC1 -> *pkbuf
	;   AC2 -> *RA_TNI_pkmask[idx]
	
	
	XAR3 = _RA_TNI_pkkey
	XAR4 = _RA_TNI_pkkey4
	
	AC0 = T0 
	; AC0 = nbits
	AC0 = AC0 << #-3
	; AC0 = nbits>>3
	T1 = AC0
	;T1 (nbytes) = AC0
	||
	AC0 = AC0 >> #1
	; AC0(nwords) = (nbits >>3 )>>1
	
	T0 = AC0 - #1
	; trip count = nwords - 1
		
	BRC0 = T0
	; load trip count
	
		
	T0 = #0
	||
	localrepeat{		;do (nwords) times
	
		AC0 = uns(*AR0+)
		; AC0 = *codebuf++
		||
		T0 = T0 & 0x01
		
		AC0 = AC0 ^ *AR3(T0)
		; AC0 = (curSh  ^ RA_TNI_pkkey[idx])  << RA_TNI_pkshift[1]
		||
		AR2 = #0
		; AR2 = RA_TNI_pkinc[2]
		
		AC2 = dbl(* (_RA_TNI_pkmask) )		;	
		; AC2 = RA_TNI_pkmask[0]

		AC1 = dbl(*AR1)
		; AC1 = *pkbuf
		||		
		if( T0 != #0 )	execute(AD_Unit)		; if( idx == 1 )
			AC2 = AC2 << #16				;   AC2 = RA_TNI_pkmask[1]
			||
			AR2 = #2						;   AR2 = RA_TNI_pkinc[1]
	
		AC1 = AC1 & AC2 
		; AC1 = *pkbuf & RA_TNI_pkmask[idx]
		||
		if( T0 == #0 )	execute (D_Unit )   ; if ( idx == 0 )
			AC0 =  AC0 << #16				;  		AC0 = (curSh  ^ RA_TNI_pkkey[idx])  << RA_TNI_pkshift[0]
			
		T0 = T0 + #1
		; idx++
		||	
		AC1 = AC1 | AC0	
		; AC1 = AC1 | (curSh  ^ RA_TNI_pkkey[idx])  << RA_TNI_pkshift[idx]
			
		dbl(*AR1) = AC1
		
		AR1 = AR1 + AR2
		; pkbuf += RA_TNI_pkinc[idx] 

	}
	
	AC0 = *AR0+
	;AC0 = *codebuf++
	||
	T0 = T1
	; T0 = nbytes
	
	T0 = T0 & 0x03 
	; T0 = nbytes & 0x03
	
	
	T1 = T1 & 0x01
	; T1 = nbytes & 0x01
	||
	mar( T0 - #1)
	; T0 (idx) = (nbytes & 0x03) - 1
	
	AC0 = AC0 ^ *AR4(T0)
	; AC0 = *codebuf++ ^ RA_TNI_pkkey4[idx]
	
	AC0 = AC0 << #16
	; AC0 = (*codebuf++ ^ RA_TNI_pkkey4[idx]) << 16
	||
	if( T1 != 0 )  execute(AD_Unit)                              ;if( nbytes & 0x01 )
		dbl(*AR1) = AC0							 ; 	*pkbuf = (*codebuf++ ^ RA_TNI_pkkey4[idx]) << 16
	
	
	return	
	
	
	.else 		;DECODE_BYTES_OLD	

	.global _RA_TNI_pkkey4			;const USHORT RA_TNI_pkkey4[4] = { 0x3700, 0xc500, 0x1100, 0xf200 };
	
	; new version of decode bytes
_RA_TNI_DecodeBytes:

	;-----------------------------------------------------
	;	To Assign :
	;
	;	XAR0 -> codebuf
	;	XAR1 -> pkbuf
	;   XAR4 -> RA_TNI_pkkey4
	;
	;	T0 ->  idx
	;	T1 ->  nbytes
	;   AC0 -> curSh
	; 	AC1 -> *pkbuf
	
	

	XAR4 = _RA_TNI_pkkey4
	
	AC0 = T0 
	; AC0 = nbits
	|| ;[fj] 09/02/2003 00:03: added to fix byte load bug
	bit(ST1, #ST1_SXMD) = #0
	; clear the SXMD bit
	
	AC0 = AC0 << #-3
	; AC0 = nbits>>3
	T1 = AC0
	;T1 (nbytes) = AC0
	||
	AC0 = AC0<<C #-2
	; AC0(nwords/2) = (nbits >>3 )>>2
	; move nwords[0] into CARRY
	
	T0 = AC0 - #1
	; trip count = nwords - 1
	
	TC1 = bit( @06, #11 ) || mmap()
	; TC1 = CARRY ( bit #11 of ST0 )
			
	BRC0 = T0
	; load trip count
	
	T0 = T1
	; T0 = nbytes
	||
	localrepeat{		;do (nwords/2) times
	
		*AR0+ = *AR0+ ^ 0x37c5
		; *codebuf++ = *codebuf ^ RA_TNI_pkkey[0]
			
		*AR0+ = *AR0+ ^ 0x11f2
		; *codebuf++ = *codebuf ^ RA_TNI_pkkey[1]
	
		;[fj] 09/02/2003 02:49 : Deleted lines here !!
	}
	
	.noremark 5503
	if ( TC1 ) execute(AD_Unit)				; if (nwords & 0x01) 
	||	*AR0+ = *AR0+ ^ 0x37C5				; *codebuf++ = *codebuf ^ RA_TNI_pkkey[0]
	
		
	;[fj] 09/02/2003 02:49 
	; removed post-inc
	AC0 = *AR0
	;AC0 = *codebuf
	||
	T0 = T0 & 0x03 
	; T0 = nbytes & 0x03
	
	T1 = T1 & 0x01
	; T1 = nbytes & 0x01
	||
	mar( T0 - #1)
	; T0 (idx) = (nbytes & 0x03) - 1
	
	; [fj] : modification on  09/02/2003 02:49
	AC0 = AC0 ^ *AR4(T0)
	
	AC1 = low_byte( *AR0 ) 
	; AC0 = (*codebuf ^ pkkey4[idx])
	; AC1 = (*codebuf | 0x00FF )
	nop;added to avoid silicon exception causing remark 5505
	AC0 = AC0 | AC1 
	; AC0 = (*codebuf ^ pkkey4[idx]) | (*codebuf & 0x00FF )
	||
	if( T1 != 0 )  execute(AD_Unit)                              ;if( nbytes & 0x01 )
		*AR0 = AC0							 ; 	codebuf = (*codebuf ^ pkkey4[idx]) | (*codebuf & 0x00FF )

	.remark 5503
	.remark 5505
	.remark 5673
	.remark 5573
	.remark 5571
	
	bit(ST1, #ST1_SXMD) = #1
	; set the SXMD bit
	|| ;[fj] 09/02/2003 02:49: added to fix byte load bug
	return

	
	
	.endif 		;DECODE_BYTES_OLD	


; _RA_TNI_DecodeBytes
;----------------------------------------------------------------------------------------------


;	.endif 		; UNPACKBIT_ASM
