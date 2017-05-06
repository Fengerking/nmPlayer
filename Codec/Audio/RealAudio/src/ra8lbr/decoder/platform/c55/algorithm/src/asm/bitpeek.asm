; ***** BEGIN LICENSE BLOCK *****   
; Source last modified: $Id: bitpeek.asm,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
; 	File 		:	Bitpeek.asm 
;   Description : C55X assembly routines for functions present in huffmantables.c
;					: RA_TNI_Peekbits()
;					: RA_TNI_Removebits()
;			
;	
;------------------------------------------------------------------------------------------------------------
;/* Functions to peek the bits in a bitstream, and then to commit the read operation*/


	.if PEEKBIT_ASM			; to enable assembly version of RA_TNI_Unpackbits and RA_TNI_Unpackbit
	.include "coder.h55"
	.mmregs

	.def _RA_TNI_Peekbits
	.def _RA_TNI_Removebits
;---------------------------------------------------------------------------------------------------------


;----------------------------------------------------------------------------------
; _RA_TNI_Peekbits : C callable function
; 
;			
;/*
; * peeks 'nbits' bits from stream.
; * Returns nbits if successful, or max possible
; *
; */	
;
; Input : T0 -> nbits
; 		  XAR0-> data (USHORT*)
;		  T1 -> pkbit (short)
;		  AR1-> pktotal (short)
;		  XAR2-> bitstrmPtr (RA_TNI_Obj*)
;
; Return : T0 -> nbits
;
; Registers touched : AC0, AC1, AC2, AC3, T0, T1, XAR0, XAR1, XAR2, XAR4





_RA_TNI_Peekbits:

	.noremark 5573
	.noremark 5549
	.noremark 5505
	;-----------------------------------------------------
	; current assignments
	;
	; XAR0 -> 	*data
	; AR1 -> pktotal
	; XAR2 -> bitstrmPtr
	;
	; T0 -> nbits
	; T1 -> pkbit
	
	;
	; to assign
	;
	; AC0 ->(pktotal + nbits) - bitstrmPtr->nframebits
	; AC1 -> bitstrmPtr->nframebits
	; AC2 -> *(bitstrmPtr->pkptr)
	; AC3 -> temp
	;
	; XAR4 -> bitstrmPtr->pkptr
	;
	;-----------------------------------------------------
	
	AC0 = AR1 
	; AC0 = pktotal 

	AC1 = *AR2(#(RA_TNI_Obj.nframebits))
	;AC1 = bitstrmPtr->nframebits

	AC0 = AC0 + T0
	; AC0 = pktotal + nbits
	||
	mar( AR2 + #(RA_TNI_Obj.pkptr) )
	; AR2 = bitstrmPtr + RA_TNI_Obj.pkptr

	XAR4 =dbl(*AR2)
	; XAR4 = bitstrmPtr->pkptr
	||
	AC0 = AC0 - AC1
	; AC0 = pktotal + nbits - bitstrmPtr->nframebits
	
	AC2 = dbl(*AR4)
	; AC2 -> *(bitstrmPtr->pkptr) 
	|| AC1 = AC1 - AR1					;		AC1 = bitstrmPtr->framebits - pktotal
	nop
	nop;added two nops to avoid silicon exception causing remark 5505
	if( AC0 > 0 ) execute(AD_Unit)		; if( pktotal + nbits > bitstrmPtr->nframebits)
	|| T0 = AC1							; 	T0(nbits) = bitstrmPtr->framebits - pktotal
	

	
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																												
	AC3 = AC2 <<< T1
	; AC3 (temp) = (*(bitstrmPtr->pkptr) ) << pkbit
	||
	T1 = T1 + T0 
	; pkbit = pkbit + nbits

	mar(T1 - #32)
	; T1 = pkbit - 32
	||
	AR4 =  AR4 + #2
	;AR4 = bitstrmPtr->pkptr +  #2

	AC2 = dbl(*AR4)
	; AC2 -> *(bitstrmPtr->pkptr + #2)
	||
	AC1 = T1 
	; AC1 = pkbit - 32
	
	T1 = T1 - T0
	; T1 = pkbit - nbits
	||
	AC0 =  T0 - #32
	; AC0 = nbits - 32
	
	AC2 = AC2 <<< T1
	; AC2 = *(bitstrmPtr->pkptr)>> (nbits - pkbits)
	||
	T1 = AC0 
	; T1 = nbits - 32
	
	if( AC1 > 0 ) execute(AD_Unit)			; if ( pkbit - 32 > 0 )
	||	AC3 = AC3 | AC2						; 	AC3 (temp) = temp | *(bitstrmPtr->pkptr)>> (nbits - pkbits)
	
		
	AC3 = AC3 <<< T1
	; AC3 = temp >>> ( 32 - nbits )
	
	*AR0 = AC3
	; *data = USHORT( temp >> (32-nbits) )
	||								
	return 
	;return nbits
	
; _RA_TNI_Peekbits
;----------------------------------------------------------------------------------------------




;----------------------------------------------------------------------------------
; _RA_TNI_Removebits : C callable function
; 
;			
;/*
; * Remove bits from stream.
; * Returns nbits if successful, or zero if out-of-bits.
; *
; */	
;
; Input : T0 -> nbits
;		  XAR0-> pkbit (short*)
;		  XAR1-> pktotal (short*)
;		  XAR2-> bitstrmPtr (RA_TNI_Obj*)
;
; Return : T0 -> nbits
;
; Registers touched : AC0, AC1, , T0, T1, XAR0, XAR1, XAR2, 



_RA_TNI_Removebits:


	;-----------------------------------------------------
	; current assignments
	;
	;	T0 -> nbits
	;	XAR0-> pkbit (short*)
	;	XAR1-> pktotal (short*)
	;	XAR2-> bitstrmPtr (RA_TNI_Obj*)
	;
	
	;
	; to assign
	;
	; AC0 ->(pktotal + nbits) - bitstrmPtr->nframebits
	; AC1 -> bitstrmPtr->nframebits
	; AC2 -> *(bitstrmPtr->pkptr)
	; AC3 -> temp
	;
	; XAR4 -> bitstrmPtr->pkptr
	;
	;-----------------------------------------------------
	

	AC1 = *AR2(#(RA_TNI_Obj.nframebits))
	;AC1 = bitstrmPtr->nframebits
	
	T0 = T0 + *AR1  
	; T0 = *pktotal + bits
	||
	mar(T1 = T0)
	; T1 = nbits
	
	*AR1 = T0
	; *pktotal = *pktotal + bits
	||
	mar( AR2 + #(RA_TNI_Obj.pkptr) )
	; AR2 = bitstrmPtr + RA_TNI_Obj.pkptr
	
	
	AC1 = *AR1 - AC1 
	; AC1 = *pktotal - bitstrmPtr->nframebits
	||
	AC0 = *AR0
	; AC0 = *pkbit

	T0 = #0
	; for return 0
	||
	if( AC1 > 0 ) return 
	; if( *pktotal > bitstrmPtr->nframebits )

	T1 = T1 + AC0
	;T1 = *pbbit + nbits
	||
	mar(T0 = T1)
	; T0 = nbits
	
	AC2 = T1 - #32
	; AC2 = (*pkbit+nbits) - 32
	
	AC1 = dbl(*AR2) 
	; AC1 = bitstrmPtr->pkptr	
	||
	if( AC2 >= 0 ) execute(AD_Unit)			; if( *pkbit + nbits >= 32 ) {
		AC1 = AC1 + #2						;		AC1 = bitstrmPtr->pktpr + #2
		||									;		T1 = *(pkbits + nbits - 32 )
		T1 = AC2							; }
	
	dbl (*AR2 ) = AC1 		
	; bitstrmPtr->pkptr = bitstrmPtr->pkptr (+2)
	
	.remark 5573
	.remark 5549
	.remark 5505
	*AR0 = T1
	; *pkbit = *pkbit + nbits (-32)
	||
	return 


; _RA_TNI_Peekbits
;----------------------------------------------------------------------------------------------

	

	.endif ;PEEKBIT_ASM