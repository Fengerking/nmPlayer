; ***** BEGIN LICENSE BLOCK *****   
; Source last modified: $Id: hufftables.asm,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
; 	File 		:	hufftables.asm 
;   Description : C55X assembly routines for functions present in huffmantables.c
;					: RA_TNI_DecodeNextSymbolWithTable
;	
;------------------------------------------------------------------------------------------------------------
;/* Functions to peek the bits in a bitstream, and then to commit the read operation*/


	.if HUFFTABLE_ASM			; to enable assembly version of RA_TNI_DecodeNextSymbolWithTable
	.include "coder.h55"
	.mmregs
	.def _RA_TNI_DecodeNextSymbolWithTable
;	.ref _FIXTABSIZE			;const int FIXTABSIZE = 0x03 
								;	// fixed table size for non-level 1 tables
FIXTABSIZE .set 0x03
;---------------------------------------------------------------------------------------------------------


;----------------------------------------------------------------------------------
; _RA_TNI_DecodeNextSymbolWithTable : C callable function
; 
; Decodes the next code from the stream, using huffman lookup tables.
; Returns -1 for error in decoding process.
; 
; NOTE : use a non-bitpacked version of huffman trees.			
;
; Input : XAR0-> table (HUFFMANTABLENODE*)
;		  XAR1-> val (USHORT*)
;		  XAR2-> pkbit (short*)
;		  XAR3-> pktotal (short*)
;		  XAR4-> bitstrmPtr (RA_TNI_Obj*)
;
;		  T0 ->  n (int)
;
; Return : T0 -> nbits
;


_RA_TNI_DecodeNextSymbolWithTable:
    .noremark 5573
    .noremark 5673
    .noremark 5505
	; SP -> odd
	;----------------------------------------------------------------------
	; prolog code
	push(T2, T3)
	; save for later restore 
	|| ; test this out
	BRC0 = 0xFFF		
	; while(1)
	
	pshboth(XAR5)
	||; verify this
	T2 = T0
	; save 'n' in T2
	
	pshboth(XAR6)
	pshboth(XAR7)
;	pshboth(XCDP)
	; save for later restore 
	; allocate space for locals on stack
;	SP = SP - #1
	; SP -> bitfield
	; SP -> even
	
	;-----------------------------------------------------
	; current assignments
	;
	;	XAR0-> table (HUFFMANTABLENODE*)
	;   XAR1-> val (USHORT*)
	;   XAR2-> pkbit (short*)
	;   XAR3-> pktotal (short*)
	;   XAR4-> bitstrmPtr (RA_TNI_Obj*)
	;
	;   T0 ->  n (int)
	;	T2 -> n
	;
	;
	; to assign
	;	T3 = nframebits
	;   XAR5 -> bitstrmPtr + pkptr
	; 	XAR5 -> bitstrmPtr->pkptr
	;	XAR6 -> bitstrmPtr->pkptr
	;   AR7 -> 	nbits (return value)
	; 	XCDP -> 
	;
	;-----------------------------------------------------
	
	
	
	T3 = *AR4( #(RA_TNI_Obj.nframebits) )
	; T3 = nframebits
	
	mar( AR4 + #(RA_TNI_Obj.pkptr) ) 
	; AR4 = bitstrmPtr + pkptr
	||
	T1 = *AR2
	; T1 = *pkbit

	XAR5 = dbl(*AR4)
	; XAR5 = bitstrmPtr->pkptr

	AR7 = #0
	; nbits = 0 
	
	XAR6 = XAR5
	; XAR6 = bitstrmPtr->pkptr 
	||	
	; do {
	blockrepeat{
	
		;***********************************************************************
		;RA_TNI_Peekbits( )
		;**********************************************************************
	
		;-----------------------------------------------------
		; current assignments
		;
		; XAR0 -> 	table
		; XAR1 -> val
		; XAR2 -> pkbit
		; XAR3 -> pktotal
		; XAR4 -> bitstrmPtr + pkptr
		; XAR5 -> bitstrmPtr->pkptr
		; XAR6 -> bitstrmPtr->pkptr
		;  
		; AR7 -> nbits
		; T0 -> ****
		; T1 -> *pkbit
		; T2 -> nbits
		; T3 -> bitstrmpPtr->nframebits
		;
		; to assign
		;
		; AC0 ->(*pktotal + nbits) - bitstrmPtr->nframebits
		; AC1 ->
		; AC2 -> *(bitstrmPtr->pkptr)
		; AC3 -> temp
		;
		; XAR6 -> bitstrmPtr->pkptr + 2
		;
		; T0 -> retbits (actual bits read)
		;-----------------------------------------------------
		
		
		AC0 = *AR3 + T2
		; AC0 = *pktotal + nbits
		||
		T0 = T2
		; T0 = nbits
		
		AC0 = AC0 - T3
		; AC0 = (*pktotal + nbits) - nframebits
		||
		AC2 = dbl(*AR5)
		; AC2 = *(bitstrmpPtr->pkptr)
		nop
		nop;added to avoid silicon exception causing remark 5505
		if( AC0 > 0 ) execute(AD_Unit)		; if( *pktotal + nbits > nframebits )
		||	T0 = T3 - *AR3					; 	T0(retbits) = nframebits - *pktotal
			
			
		AC3 = AC2 <<< T1			; NOTE : <<< is important . << will not do!!
		; AC3(temp) = *(bitstrmPtr->pkptr) <<< pkbit
		||
		T1 = T1 + T0
		; T1 = *pkbit + retbits
		
		mar( T1 - #32 )
		; T1 = (*pkbits+retbits) - 32
		||							; Parallelism verified at 12:30 on 8/12/01
		AR6 = AR6 + #2
		; AR6 = bitstrmPtr->pkptr + #2

		AC1 = T1
		; AC1 = (*pkbits+retbits)-32
		||
		AC2 = dbl(*AR6)
		; AC2 = *(bitstrmPtr->pkptr + #2)

		T1 = T1 - T0
		; T1 = (*pkbits + retbits - 32) - retbits	; shift factor for the second dword
		; 	=> T1 = (*pkbits - 32 ) 
		||
		AC0 = T0 - #32
		; AC0 = retbits - #32
		
		AC2 = AC2 <<< T1
		; AC2 = *(bitstrmPtr->pkptr + #2) >> ( 32 - *pkbits ) 
		||
		T1 = AC0
		; T1 = retbits - #32
		
		if( AC1 > 0 ) execute(AD_Unit)			; if ( (*pkbits+retbits) > 32 ) 
		||	AC3 = AC3 | AC2						;  AC3(temp) = temp | *(bitstrmPtr->pkptr+#2)>>(32-*pkbits ) 
			
		AC3 = AC3 <<< T1
		; AC3 = temp >>> (32 - retbits)
				
		
		;***********************************************************************
		; End of RA_TNI_Peekbits( )
		;**********************************************************************
			
		;-----------------------------------------------------
		; current assignments
		;
		; XAR0 -> table
		; XAR1 -> val
		; XAR2 -> pkbit
		; XAR3 -> pktotal
		; XAR4 -> bitstrmPtr + pkptr
		; XAR5 -> bitstrmPtr->pkptr
		; XAR6 -> bitstrmPtr->pkptr + 2 
		;
		; AR7 -> nbits
		; T0 -> retbits
		; T1 -> *****
		; T2 -> nbits
		; T3 -> bitstrmpPtr->nframebits
		;
		; AC0 -> *****
		; AC1 -> *****
		; AC2 -> *****
		; AC3 = bitfield (required bits)
		||
		T1 = *AR3 + T2
		; T1 = *pktotal  + nbits
		if( T0 <= 0 ) execute ( AD_Unit )		 ; if (retbits <= 0  ) { ;  // discard all read bits, and commit operation 
			*AR3 = T1	 						 ;	*pktotal += nbits 
			|| T0 = #0							 ; 	return 0 
												 ; }
		if( T0 == 0 ) goto RA_TNI_DecodeNextSymbolWithTable_EXIT	
		||
		T2 = T2 - T0			
		; T2 = n - retbits
		
		if( T2 != 0 ) execute (AD_Unit)			 ; if( n != retbits )
		||	AC3 = AC3 <<< T2					 ;    bitfield = bitfield<<(n - retbits )
		
		T0 = AC3 
		; T0 = bitfield (offset into table)
		||
		AC1 = #0
		; clear AC1 (flag)
		
;		T0 = T0 <<< #1
		; T0 = bitfield << #1 : for dword addressing
;		||
;		T2 = #-16
		T2 = #-5	 		
		;-----------------------------------------------------
		; current assignments
		;
		; XAR0 -> table
		; XAR1 -> val
		; XAR2 -> pkbit
		; XAR3 -> pktotal
		; XAR4 -> bitstrmPtr + pkptr
		; XAR5 -> bitstrmPtr->pkptr
		; XAR6 -> bitstrmPtr->pkptr + 2 
		;
		; AR7 -> nbits
		; T0 -> bitfield (offset into table
		; T1 -> *****
		; T2 -> -16
		; T3 -> bitstrmpPtr->nframebits
		;
		; AC0 -> *****
		; AC1 -> 0
		; AC2 -> *****
		; AC3 -> *****
		;
		; to assign
		;
		; AC0 -> table[bitfield] (2 words) , & symbol
		; AC1 -> ****
		; AC2 -> ***
		; AC3 -> 
		;
		; 
		;
		; T0 -> abs(bitcount)
		; T1 -> bitcount
		;-----------------------------------------------------
		||
;		AC0 = dbl(*AR0(T0) )
		AC0 = *AR0(T0) 
		; AC0 = table[bitfield] 


;		T1 = AC0 & #0xFFFF
		AC1 = AC0 << #27
		AC1 = AC1 << #-27
		;AC1 = lower 5 bits of AC0 (sign extended )
		T1 = AC1 ;
		; T1 = signed(bitcount) (lower 5 bits)

		AC0 = AC0 <<<T2
		; AC2 = table[bitfield].symbol
		||
		T0 = |T1|
		; T0 = abs(bitcount) 
		
		
		;***********************************************************************
		;RA_TNI_Removebits( ) : Remove 'bitcount' bits from stream
		;**********************************************************************
	
		;-----------------------------------------------------
		; current assignments
		;
		; XAR0 -> table
		; XAR1 -> val
		; XAR2 -> pkbit
		; XAR3 -> pktotal
		; XAR4 -> bitstrmPtr + pkptr
		; XAR5 -> bitstrmPtr->pkptr
		; XAR6 -> bitstrmPtr->pkptr + 2
		;  
		; AR7 -> nbits
		; T0 -> bitcount , abs
		; T1 -> bitcount , signed
		; T2 -> ****
		; T3 -> bitstrmpPtr->nframebits
		;
		; AC0 ->  symbol
		;
		; to assign
		; 
		; AC1 ->  *pktotal + bitcount - nframebits
		; AC2 -> *pkbit + bitcount - 32
		; AC3 ->
		;
		; T2 -> *pkbit + bitcount
		; 
		;-----------------------------------------------------
		
		AC1 = *AR3 + T0
		; AC1 =   *pktotal + bitcount
		||
		AR7 = AR7 + T0
		; nbits = nbits + bitcount
		
		T2 = *AR2 + T0
		; T2 = *pkbit + bitcount
		
		*AR3 = AC1
		;*pktotal = *pktotal + bitcount
		||
		AC1 = AC1 - T3
		; AC1 =  *pktotal + bitcount - nframebits
		
		AC2 = T2 - #32
		; AC2 = *pkbit + bitcount - 32
		
		; This set of conditions should never happen
		; If the frame gets exhaused before reading 'bitcount' bits
		; return 0 to the calling function
		; However this condition has already been tested in Peekbits part
		.noremark 5503
		if( AC1 > 0 ) execute (AD_Unit) 		; if( *pktotal + bitcount > nframebits )
			||	T0 = #0 							;		return 0
		if( AC1 > 0 ) goto 	RA_TNI_DecodeNextSymbolWithTable_EXIT
		; return 
		.remark 5503
		if( AC2 >= 0 )execute (AD_Unit)			 ; if(  *pkbit + bitcount >= 32 ){
			mar(AR5 = AR6)						 ; 	XAR5 = bitstrmPtr->pkptr+2
		||	T2 = AC2							 ;  T2 = *pkbit + bitcount  - 32
												 ; }
		AR6 = AR5
		; AR6 = bitstrmPtr->pkptr (+2)				
		||
		dbl(*AR4) = XAR5
		nop
		nop;added to avoid silicon exception causing remark 5505
		; bitstrmPtr->pkptr = bitstrmPtr->pkptr (+2)				
		.noremark 5503
		*AR2 = T2 
		; *pkbit = *pkbit + bitcount (-32) 
				
		;***********************************************************************
		;End of RA_TNI_Removebits( ) 
		;**********************************************************************
		;-----------------------------------------------------
		; current assignments
		;
		; XAR0 -> table
		; XAR1 -> val
		; XAR2 -> pkbit
		; XAR3 -> pktotal
		; XAR4 -> bitstrmPtr + pkptr
		; XAR5 -> bitstrmPtr->pkptr
		; XAR6 -> bitstrmPtr->pkptr
		;  
		; AR7 -> nbits
		; T0 -> bitcount , abs
		; T1 -> bitcount , signed
		; T2 -> ****
		; T3 -> bitstrmpPtr->nframebits
		;
		; AC0 ->  symbol
		;
		; to assign
		; 
		;
		; T0 -> return value (offset)
		; T1 -> *pkbit
		; T2 ->  FIXTABSIZE (n)
		; 
		;-----------------------------------------------------
		||
		if( T1 > 0 ) execute (AD_Unit)							; if( bitcount > 0 ) {
		 	 *AR1 = AC0											;   *val = symbol
			||	T0 = AR7										; 	nbits = nbits  
		if( T1 == 0 ) execute (AD_Unit)							;} if( bitcount == 0 ){
		||   T0 = #-1											;      nbits = -1
		if( T1 >= 0 ) goto RA_TNI_DecodeNextSymbolWithTable_EXIT	; } return nbits		
;		||
		; else if (bitcount < 0 ) {    
;		AC0 = AC0 <<< #1 
		; AC0 = symbol << 1 : for dword addressing
		.remark 5503
		T2 = FIXTABSIZE
		; T2(n) = FIXTABSIZE for next pass
		||
		T1 = *AR2
		; T1 = *pkbit
		
		AR0 = AR0 + AC0
		; AR0 = table + (symbol << 1)
		
		;-----------------------------------------------------
		; current assignments
		;
		; XAR0 -> table (with new offset)
		; XAR1 -> val
		; XAR2 -> pkbit
		; XAR3 -> pktotal
		; XAR4 -> bitstrmPtr + pkptr
		; XAR5 -> bitstrmPtr->pkptr
		; XAR6 -> bitstrmPtr->pkptr
		;  
		; AR7 -> nbits
		; T0 -> *****
		; T1 -> *pkbit
		; T2 -> FIXTABSIZE
		; T3 -> bitstrmpPtr->nframebits
		;
		;-----------------------------------------------------
	
	
	}; whille(1)



RA_TNI_DecodeNextSymbolWithTable_EXIT:
	
	; SP -> even
	;----------------------------------------------------------------------
	; epilog code
	
	; deallocate space for locals on stack
;	SP = SP + #1
	
;	XCDP = popboth()
	XAR7 = popboth()
	XAR6 = popboth()
	XAR5 = popboth()
	T2, T3 = pop()
	; restore 
    .remark 5573
    .remark 5673
    .remark 5505	
	return 

	
; _RA_TNI_DecodeNextSymbolWithTable
;----------------------------------------------------------------------------------------------

	.endif ; HUFFTABLE_ASM
