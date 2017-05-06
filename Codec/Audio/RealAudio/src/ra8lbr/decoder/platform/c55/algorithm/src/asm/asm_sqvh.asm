; ***** BEGIN LICENSE BLOCK *****   
; Source last modified: $Id: asm_sqvh.asm,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
; 	File 		:	asm_sqvh.asm 
;   Description : C55X assembly routines for functions present in sqvh.c WITHOUT DMA support
;
;				:RA_TNI_DecodeVectors
;
;	NOTE : Implementation of merged version of DecodeVectors()
;------------------------------------------------------------------------------------------------------------

    .if !CONST_TABLE_DMA		; DMA disabled
    
	.mmregs

	.if	DECODEVECTORS_ASM
	
	.include coder.h55  	
	
	.def _RA_TNI_DecodeVectors

;	.ref _RA_TNI_DecodeNextSymbolWithTable
;short RA_TNI_DecodeNextSymbolWithTable( HuffmanTable table, int n, USHORT *val, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr)
;s	.ref _RA_TNI_ScalarDequant
;void RA_TNI_ScalarDequant(short cat, short r, short *rms_index, short *k, short *s, long *mlt, RA_TNI_Obj *bitstrmPtr)
	

FIXTABSIZE .set 0x03
	
NBINS 	   .set 20	; no. of MLT coeffs per region

MLTBITS	   .set 22	; bits per MLT coeff.



	.eval NBINS + NBINS + 21 , LOCAL_SIZE		; size of local stack	
	
	.eval ((1<<7)|(1<<5)|(1<<3)|(1<<2)|(1<<1)|(1<<0)), FEEDBACK
	.eval (MLTBITS - 2 - 31) , UNDERCLIP
	.eval (MLTBITS - 2), OVERCLIP	



	.cpl_on	
	; notify the assembler for compiler mode on
	
	
;	.ref _RA_TNI_kmax_tab		; const char RA_TNI_kmax_tab[7] = { 14, 10, 7, 5, 4, 3, 2 };
;
;	.ref _RA_TNI_iradix_tab 	; const short RA_TNI_iradix_tab[7] = { 2341, 3277, 4682, 6554, 8192, 10923, 16384 };
;
;	.ref _RA_TNI_vd_tab			; const char RA_TNI_vd_tab[7]  =	{	2,	2,	2,	4,	4,	5,	5	};
;	.ref _RA_TNI_vpr_tab		; const char RA_TNI_vpr_tab[7] =	{	10,	10,	10,	5,	5,	4,	4	};

	.ref _RA_TNI_sqvh_tab			; const HuffmanTable RA_TNI_sqvh_tab[7] 
	.ref _RA_TNI_sqvh_bitcount_tab	; const int RA_TNI_sqvh_bitcount_tab[7] 
	
	.ref _RA_TNI_kmax_iradix_tab	 ; const short	RA_TNI_kmax_iradix_tab[14] = {	/*kmax*/	/*iradix*/
	.ref _RA_TNI_vpr_vd_tab			 ; const char RA_TNI_vpr_vd_tab[14] = {	/* vpr */	/*vd*/

	.ref _RA_TNI_fqct_man			;const long* RA_TNI_fqct_man[7]		defined in "tables.c"
	.ref _RA_TNI_fqct_exp			;const char* RA_TNI_fqct_exp[7] 	defined in "tables.c"
	
	.ref _RA_TNI_fixrootpow2man_new ; const long RA_TNI_fixrootpow2man_new[2] 	defined in "tables.c"
	
	.ref _RA_TNI_fixdither_tab		; const long RA_TNI_fixdither_tab[8] ;

;------------------------------------------------------------------------------------------------
; _RA_TNI_DecodeVectors : ( C-callable)
; 
; /* To decode the SQVH coded MLT coefficients from the bitstream	 
;  * Involves unpacking the SQVH coded vectors from the stream, followed by
;  * scalar dequantization to recover the MLT coefficients 
;  */
;	
;	Input : XAR0 -> catbuf (USHORT*)
;		  : XAR1 -> rms_index (short*)
;		  : XAR2 -> mlt (long*)
;		  : XAR3 -> pkbit (short*)
;		  : XAR4 -> pktotal (short*)
;		  ; SP(01,02) -> bitstrmPtr(short*)	
;
;   Output : void
;
; 	Registers touched : XAR0, XAR1, XAR2,XAR3,XAR4,XAR5,XAR6,XAR7,XCDP, T0, T1, T2,T3
;							AC0, AC1, AC2,AC3, BRC0, BRC1
;



_RA_TNI_DecodeVectors:
	; SP -> odd
	;----------------------------------------------------------------------
	; prolog code
	;
	push(T2,T3)
	pshboth(XAR5)		; save for restore on exit
	pshboth(XAR6)
	pshboth(XAR7)
	pshboth(XCDP)
	||
	bit(ST2, #15) = #0
	; clear the ARMS bit to use DSP Mode addressing
	.ARMS_off
	; SP -> odd
;	||
;	mar(T1 = #(RA_TNI_Obj.nframebits))
	; T1 = offset of (RA_TNI_Obj::nframebits)
	
	XAR5 = dbl(*SP(#07) )
	; XAR5 = bitstrmPtr
		
	SP = SP - LOCAL_SIZE ;
	; allocate space for locals
	; SP -> even
		
	;-----------------------------------------------------
	; current configuration
	;
	; XAR0 -> catbuf
	; XAR1 -> rms_index
	; XAR2 -> mlt
	; XAR3 -> pkbit	
	; XAR4 -> pktotal
	; XAR5 -> bitstrmPtr
	; XAR6 -> ******
	; XAR7 -> ****
	; XCDP -> ***
	; T0 -> *****
	; T1 -> ******	/*offset of n framebits*/
	; T2 -> *****
	; T3 -> *****
	;
	; 
	; to assign
	;
	; -------------------------------------------
	; |	SP  (even)	 	 | k[NBINS]	(short*)    |
	; |	SP + NBINS 		 | s[NBINS]	(short*)	|
	; |	SP + 2*NBINS + 1 | 	cregions			|
	; |	SP + 2*NBINS + 2 | mlt					|
	; |	SP + 2*NBINS + 3 |						|
	; |	SP + 2*NBINS + 4 | 						|
	; |	SP + 2*NBINS  	 |						|
	;
	; AC0 ->
	; AC1 -> 
	; AC2 ->
	; AC3 -> 
	;
	; XAR2 -> pkbit
	; XAR3 -> pktotal
	; XAR5 -> bitstrmPtr /*+ pkptr*/
	; XAR6 -> kptr
	; XAR7 -> sptr
	; XCDP -> mlt
	;
	; T0 -> 
	; T1 ->
	; T2 -> 
	; T3 -> cregions
	;-----------------------------------------------------

	T3 = *AR5(#(RA_TNI_Obj.nregions) ) 
	; T3 = bitstrmPtr->nregions
	
	T3 = T3 + *AR5( #(RA_TNI_Obj.cplstart) )
	; T3(cregions) = bitstrmPtr->nregions + bitstrmPtr->cplstart

	*SP(#(2*NBINS+1)) = T3
	; save cregions on stack
	

	XCDP = XAR2
	; XCDP -> mlt
	XAR2 = XAR3
	; XAR2 = pkbit
	XAR3 = XAR4
	; XAR3 = pktotal
	
	dbl(*SP(#(2*NBINS + 6))) = XAR1
	; store rmsindex on stack
	

RA_TNI_DecodeVectors_L1:		; outermost for loop
	; for( r = cregions ; r > 0 ; r--) {
	
		T0 = *AR0+
		; T0 (cat)= *catbuf++ ; for next iteration
		||
		AC1 = #0
	
		T1 = T0 - #7
		; T1 = cat - 7
	
		dbl(*SP(#(2*NBINS + 4))) = XAR0
		; store catbuf + r on stack
		
		
		*SP(#(2*NBINS+12)) = AC1
		; outOfbits = 0
		||
		mar(T2 = T0)
		; T2 = cat
	
		
		if( T1 >= 0 ) goto RA_TNI_DecodeVectors_L2	; if( cat >= 7 ) goto RA_TNI_DecodeVectors_L2
		
		; if( cat < 7 ) {
			;-----------------------------------------------------
			; current configuration
			;
			; XAR0 -> catbuf+r
			; XAR1 -> rms_index
			; XAR2 -> pkbit
			; XAR3 -> pktotal
			; XAR4 -> ****
			; XAR5 -> bitstrmPtr 
			; XAR6 -> ***
			; XAR7 -> ***
			; XCDP -> mlt
			; T0 -> cat
			; T1 -> ****
			; T2 -> cat
			; T3 -> r ( cregions to 0 )
			;
			; -------------------------------------------
			; |	SP  (even)	 	 | k[NBINS]	(short*)    |
			; |	SP + NBINS 		 | s[NBINS]	(short*)	|
			; |	SP + 2*NBINS + 1 | cregions				|
			; |	SP + 2*NBINS + 2 | 						|
			; |	SP + 2*NBINS + 3 | 						|
			; |	SP + 2*NBINS + 4 | 						|
			; |	SP + 2*NBINS + 6 |	rmsindex			|
			;
			; 
			; to assign
			; |	SP + 2*NBINS + 3 | vindex				|
			; |	SP + 2*NBINS + 4 | catbuf + r			|
			; |	SP + 2*NBINS + 6 | rms_index			|
			; |	SP + 2*NBINS + 8 | vd					|
			; |	SP + 2*NBINS + 9 | kindex				|
			; |	SP + 2*NBINS + 10| radix				|
			; |	SP + 2*NBINS + 11| iradix				|
			; |	SP + 2*NBINS + 12| outofBits			|
			; |	SP + 2*NBINS + 13| n	(vpr)			| 
			;
			; AC0 ->
			; AC1 -> 
			; AC2 ->
			; AC3 -> 
			; 
			; XAR0 -> RA_TNI_sqvh_tab[cat]
			; XAR1 -> vindex
			; XAR4 -> bitstrmPtr
			; T0 -> 
			; T1 ->
			;-----------------------------------------------------
			
			XAR7 = XSP
			
			XAR6 = XSP
			; XAR6 = k[]	

			mar( AR7 + #NBINS) 
			; XAR7 = s[] { k + NBINS }
		
			XAR4 = _RA_TNI_vpr_vd_tab
			; pointer to RA_TNI_vpr_vd_tab
						
		;	dbl(*SP(#(2*NBINS + 6))) = XAR1
		;	; store rmsindex on stack
		;	||		NOTE : This parallelism leads to problems while exec. (NOT single stepping!!)
			T0 = T0 << #1
			;T0 = cat<<1	 for indexing into the combined tables
			
			AC0 = dbl(*AR4(T0))
			; AC0  = RA_TNI_vpr_vd_tab[cat]

			XAR1 = _RA_TNI_kmax_iradix_tab	; This instr is one of the victims of the above shit!!
					
			*SP(#(2*NBINS+8)) = AC0
			; save vd on stack (lower part of accumulator)
			||
			AC0 = AC0 - #1
			; calculate kindex in AC0_L

						
			AC1 = dbl(*AR1(T0))
			; AC1= RA_TNI_kmax_iradix_tab[cat]
			||
			T1 = HI(AC0)
			; T1 = vpr
			
			T1 = T1 - #1
			; T1 = vpr - 1
			||
			*SP(#(2*NBINS+9)) = AC0
			; save kindex on stack (AC0_L)
			
			
									
			BRC0 = T1
			; BRC0 = RA_TNI_vpr_tab[cat] - 1
			; load trip count
			;*SP( #(2*NBINS + 13) ) = HI(AC0)		; NOTE to convert this to a block repeat 
			; store trip count (vpr) on stack
			dbl(*SP(#(2*NBINS+10))) = AC1
			; save radix, iradix on stack
			||
			blockrepeat{
			; for( n = vpr; n > 0 ; n--) {
			
				XAR1 = _RA_TNI_sqvh_bitcount_tab
				
				T1 = T2
				; T1 = cat
				||
				mar(T0 = T2)
				; T0 = cat
				
				XAR0 = _RA_TNI_sqvh_tab
				
				XAR4 = XAR5
				; XAR4 -> bitstrmPtr
				
				T0 = T0 << #1
				; for dword addressing in _RA_TNI_sqvh_tab
								
				XAR0 = dbl(*AR0(T0) )
				; XAR0 = RA_TNI_sqvh_tab[cat]
			
				T0 = *AR1(T1)
				; T0 = RA_TNI_sqvh_bitcount_tab[cat]
									
				XAR1 = XSP
				
				AR1 = AR1 + #(2*NBINS+3)
				; XAR1 -> vindex	
				
				;-------------------------------------		
				; current configuration
				;
				; XAR0 -> RA_TNI_sqvh_tab[cat]
				; XAR1 -> vindex
				; XAR2 -> pkbit
				; XAR3 -> pktotal
				; XAR4 -> bitstrmPtr 
				; XAR5 -> bitstrmPtr 
				; XAR6 -> kptr
				; XAR7 -> sptr
				; XCDP -> mlt
				; T0 -> RA_TNI_sqvh_bitcount_tab[cat]
				; T1 -> ****
				; T2 -> cat
				; T3 -> r ( cregions to 0 )
				;;
				
	;			call _RA_TNI_DecodeNextSymbolWithTable
				; DecodeNextSymbol
	
				
					
				;***********************************************************************************
				; Inlining of RA_TNI_DecodeNextSymbolWithTable
				;***********************************************************************************
							
					; SP -> even
					;----------------------------------------------------------------------
					; prolog code
					push(T2, T3)
					; save for later restore 
					|| ; test this out
					BRC1 = 0xFFF		
					; while(1)
					
					pshboth(XAR5)
					||; verify this
					T2 = T0
					; save 'n' in T2
					
					pshboth(XAR6)
					pshboth(XAR7)
					; SP -> odd 
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
						if( T0 == 0 ) goto RA_TNI_DecodeVectors_DecodeNextSymbolWithTable_EXIT	
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
						
						if( AC1 > 0 ) execute (AD_Unit) 		; if( *pktotal + bitcount > nframebits )
							||	T0 = #0 							;		return 0
						if( AC1 > 0 ) goto 	RA_TNI_DecodeVectors_DecodeNextSymbolWithTable_EXIT
						; return 
						
						if( AC2 >= 0 )execute (AD_Unit)			 ; if(  *pkbit + bitcount >= 32 ){
							mar(AR5 = AR6)						 ; 	XAR5 = bitstrmPtr->pkptr+2
						||	T2 = AC2							 ;  T2 = *pkbit + bitcount  - 32
																 ; }
						AR6 = AR5
						; AR6 = bitstrmPtr->pkptr (+2)				
						||
						dbl(*AR4) = XAR5
						; bitstrmPtr->pkptr = bitstrmPtr->pkptr (+2)				
						
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
						if( T1 > 0 ) execute (AD_Unit)											; if( bitcount > 0 ) {
						 	 *AR1 = AC0															;   *val = symbol
							||	T0 = AR7														; 	nbits = nbits  
						if( T1 == 0 ) execute (AD_Unit)											;} if( bitcount == 0 ){
						||   T0 = #-1															;  nbits = -1
						if( T1 >= 0 ) goto RA_TNI_DecodeVectors_DecodeNextSymbolWithTable_EXIT	; } return nbits		
				;		||
						; else if (bitcount < 0 ) {    
				;		AC0 = AC0 <<< #1 
						; AC0 = symbol << 1 : for dword addressing
						
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
				
				
				
RA_TNI_DecodeVectors_DecodeNextSymbolWithTable_EXIT:
					
					XAR7 = popboth()
					XAR6 = popboth()
					XAR5 = popboth()
					T2, T3 = pop()
					; restore 
				
				
				;***********************************************************************************
				; End of  RA_TNI_DecodeNextSymbolWithTable
				;***********************************************************************************
			
																																																										
																																																																
				;-------------------------------------		
				; current configuration
				;
				; XAR0 -> ****
				; XAR1 -> vindex
				; XAR2 -> pkbit
				; XAR3 -> pktotal
				; XAR4 -> *****
				; XAR5 -> bitstrmPtr 
				; XAR6 -> kptr
				; XAR7 -> sptr
				; XCDP -> mlt
				; T0 -> nbits
				; T1 -> ****
				; T2 -> cat
				; T3 -> r ( cregions to 0 )
				;;
				; To assign
				; 
				; T1 -> outofbits
				; XAR0 -> catbuf + r
				; XAR1 -> rms_index
			
				T1 = *SP(#(2*NBINS+12))
				; T1 = outOfBits
				||
				if( T0 == #0 ) execute(AD_Unit)				;if( DecodeNextSymbol() == 0 ) {
					*AR1 = T0								; 		vindex = 0
					|| T1 = #1								;  		T1 (outOfBits) = #1
															; }
				
				*SP(#(2*NBINS+12)) = T1
				; outOfbits = T1 
			
				AR6 = AR6 + *SP(#(2*NBINS+9))
				; kptr = kptr + kindex
				||
				AC0 = #0
				
				
				T1 = *SP(#(2*NBINS+8))
				; T1 = vd
				
				AC1 = *AR1
				; AC1 = vindex
				||
				T1 = T1 - #1
				; T1 = vd -1
				
				BRC1 = T1
				; load trip count into BRC0 = vd - 1
							
				T0 = AC1
				; T0 = vindex
				||
				localrepeat{	; for( i = vd; i > 0  ; i-- ) {
					AC2 = AC1 << #16 
					; AC2_H = AC1_L (vindex)
					
					AC0 = AC2 * *SP(#(2*NBINS+11))
					; AC0 = vindex * iradix
					
					AC0 = AC0 << #-15 ;+ 16 ) try to do this
					; AC0(udiv) = (vindex * iradix) >> 15 
					AC2 = AC0 << #16 
					; AC2_H = (vindex * iradix) >> 15 
					
					AC1 = AC1 - ( AC2 * *SP(#(2*NBINS+10)))
					; AC1 = vindex - ( udiv * radix)
					
					*AR6 = AC1 
					; *kptr = vindex - ( udiv * radix)
					||
					AC1 = AC0 
					; vindex = udiv
					
					if( T0 == 0 ) execute(D_Unit)		;if( !T0 : original vindex)
					||	*AR6- = T0						;  *kptr-- = 0
					; NOTE : AR6 decrements in any case
					
				}
			
				mar(*AR6+)
				; kptr++ : come back into the range under consideration
				||
				AR4 = AR5
				; NOTE :We can do this because an earlier assignment aligns the pages
				; XAR4 = bitstrmPtr
				
				AC0 =*AR4( #(RA_TNI_Obj.nframebits) )
				; AC0 = bitstrmPtr->nframebits
				
				mar(AR4 + #(RA_TNI_Obj.pkptr))
				; XAR4 = bitstrmPtr + pkptr
				||
				BRC1 = T1
				; load trip count into BRC0 = vd - 1
				
				AC1 = *AR6+
				; AC1 = *kptr++
				||	
				blockrepeat{ 	; for( i = vd; i > 0  ; i-- ) {
				
					*AR7 = #0
					; *sptr = 0
					||
					T1 = *AR3 
					; T1 = *pktotal
				
					T1 = T1 + #1
					; T1 = *pktotal + 1
					||
					XAR0 = dbl(*AR4)
					; XAR0 = bitstrmPtr->pkptr
				
					if( AC1 == #0 ) goto RA_TNI_DecodeVectors_Unpackbit_L4
					; if ( *kptr == 0 ) goto next iteration
					
					*AR3 = T1 ;						
					;*pktotal = *pktotal + 1
					||
					T0 = *AR2
					; T0 = *pkbit
					

					T1 = T1 - AC0
					; T1 = (*pktotal + 1) - nframebits
					||
					AC2 = dbl(*AR0 )
					; AC2 = *(bitstrmPtr->pkptr)	
					
					
					if( T1 > 0 ) execute(AD_Unit)		; if ((*pktotal + 1) > nframebits )
					|| 	*SP(#(2*NBINS+12)) = #1
					; outOfBits = 1

					AC2 = AC2 <<< T0
					; AC2 = *(bitstrmPtr->pkptr) << *pkbit
			;		||
			;		if( T1 > #0 ) goto RA_TNI_DecodeVectors_Unpackbit_L4
					
					if( T1 > #0 ) execute (AD_Unit)		; if ( (*pktotal + 1) > nframebits ) *sptr = 0 
					||		AC2 = #0

										
					AC2 = AC2 <<< #-31
					; AC2 = (*(bitstrmPtr->pkptr) << *pkbit) >> 31
					||
					mar(T0 + #1)
					; T0 = *pkbit + 1		
				
					*AR2 = T0
					; *pkbit =	*pkbit + 1		
					||
					AC3 = T0
					; AC3 = *pkbit + 1		
					
					AC3 = AC3 <<< #-4
					; AC3 = (*pkbit >> 5) << 1 for dword addressing
					||
					*AR7 = AC2
					; *sptr = bit
					
					
					bit( AC3, @0 ) = #0
					; AC2 =(*pkbit >> 5 ) <<1  : for dword addressing
				
					*AR2 = *AR2 & #0x1F
					; *pkbit = *pkbit & 0x1F
					
					AR0 = AR0 + AC3 
					; XAR0 = bitstrmPtr->pkptr + (*pkbit >> 5)		
									
					dbl(*AR4) = XAR0
					; *(bitstrmPtr+pkptr) = bitstrmPtr->pkptr + (*pkbit >> 5)
									
RA_TNI_DecodeVectors_Unpackbit_L4:				
					AC1 = *AR6+
					; AC1 = *kptr++
					||
					mar(*AR7+)
					; sptr++
				}
					
				mar(AR6-#1)
				; to undo the last AR6
			
			} ; blockrepeat BRC0 end
			; end of iterating thru each vector in a region
			
		
			;-----------------------------------------------------
			; current configuration
			;
			; XAR0 -> ****
			; XAR1 -> ****
			; XAR2 -> pkbit
			; XAR3 -> pktotal
			; XAR4 -> *****
			; XAR5 -> bitstrmPtr 
			; XAR6 -> ***
			; XAR7 -> ****
			; XCDP -> mlt
			; T0 -> ***
			; T1 -> ****
			; T2 -> cat
			; T3 -> r ( cregions to 0 )
			;
			; -------------------------------------------
			; |	SP  (even)	 	 | k[NBINS]	(short*)    |
			; |	SP + NBINS 		 | s[NBINS]	(short*)	|
			; |	SP + 2*NBINS + 1 | cregions				|
			; |	SP + 2*NBINS + 2 | 						|
		 	; |	SP + 2*NBINS + 3 | vindex				|	
			; |	SP + 2*NBINS + 4 | catbuf + r			|
			; |	SP + 2*NBINS + 6 | rms_index + r		|
			; |	SP + 2*NBINS + 8 | vd					|
			; |	SP + 2*NBINS + 9 | kindex				|
			; |	SP + 2*NBINS + 10| radix				|
			; |	SP + 2*NBINS + 11| iradix				|
			; |	SP + 2*NBINS + 12| outofBits			|
			; |	SP + 2*NBINS + 13| n (vpr)				| 
			; to assign
			;
			; AC0 ->
			; AC1 -> 
			; AC2 ->
			; AC3 -> 
			;
			;
			; T0 ->
			; T1 -> 
			;-----------------------------------------------------
			
			
			;----now to check for outOfBits--------------
			T0 = *SP(#(2*NBINS+12))
			; T0 -> outOfBits
			||
			T1 = T3
			; T1 -> cregions - r (iteration count)
		
			XAR0 = dbl(*SP(#(2*NBINS+4)) )
			; XAR0 = catbuf + r 
			||
			T1 = T1 - #1
			; T1 = cregions - r - 1 
			
		
			CSR = T1
			; load trip count in CSR = cregions - r - 1 
			||
			if( T0 == 0 ) goto RA_TNI_DecodeVectors_ScalarDequant
			; if( outOfBits == 0 ) next iteration
			
			;if( outOfBits != 0 ) {
		
				AR0 = AR0 - #1 ;
				; to catbuf for current region
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																				
											
				T2 = #7	
				; cat = 7
				||
				repeat(CSR)			; do( cregions - r times )
					*AR0+ = T2		;  catbuf[r...cregions] = 7
			
			
			;}
					
			
		; } end of (cat < 7 )	

RA_TNI_DecodeVectors_L2:	

		; if ( cat == 7 ) {		
			XAR7 = XSP
			
			mar( AR7 + #NBINS) 
			; XAR7 = s[] { k + NBINS }
		
			XAR6 = XSP
			; XAR6 = k[]	
			;-----------------------------------------------------
			; current configuration
			;
			; XAR0 -> catbuf + r
			; XAR1 -> ****
			; XAR2 -> pkbit
			; XAR3 -> pktotal
			; XAR4 -> *****
			; XAR5 -> bitstrmPtr 
			; XAR6 -> kptr
			; XAR7 -> sptr
			; XCDP -> mlt
			; T0 -> ****
			; T1 -> ****
			; T2 -> cat
			; T3 -> r ( cregions to 0 )
			;
			; -------------------------------------------
			; |	SP  (even)	 	 | k[NBINS]	(short*)    |
			; |	SP + NBINS 		 | s[NBINS]	(short*)	|
			; |	SP + 2*NBINS + 1 | cregions				|
			; |	SP + 2*NBINS + 2 | 						|
			; |	SP + 2*NBINS + 3 |						|
			; |	SP + 2*NBINS + 4 | 						|
			; |	SP + 2*NBINS  	 |						|
			;
			; 
			; to assign
			;
			; AC0 ->
			; AC1 -> 
			; AC2 ->
			; AC3 -> 
			;
			; XAR5 -> 
			; XAR6 -> 
			; XAR7 -> 
			; XCDP ->
			;
			; T1 ->
			; T2 -> 
			;-----------------------------------------------------
			T1 = #0
			||
			repeat( #(NBINS-1) )	; do NBINS times	{
				*AR6+ = T1			;		*kptr++ = 0
				||					;		*sptr++ = 0
				*AR7+ = T1			; }
					
		; }  end of if(cat==7) 
		
			


RA_TNI_DecodeVectors_ScalarDequant:
	
		;***********************************************************************************
		; Inlining of ScalarDequant
		;***********************************************************************************

		;-----------------------------------------------------
		; current configuration
		;
		; XAR0 -> ****
		; XAR1 -> ****
		; XAR2 -> pkbit
		; XAR3 -> pktotal
		; XAR4 -> *****
		; XAR5 -> bitstrmPtr 
		; XAR6 -> ***
		; XAR7 -> ***
		; XCDP -> mlt
		; T0 -> ****
		; T1 -> ****
		; T2 -> cat
		; T3 -> r ( cregions to 0 )
		;
		; -------------------------------------------
		; |	SP  (even)	 	 | k[NBINS]	(short*)    |
		; |	SP + NBINS 		 | s[NBINS]	(short*)	|
		; |	SP + 2*NBINS + 1 | cregions				|
		; |	SP + 2*NBINS + 2 | 						|
	 	; |	SP + 2*NBINS + 3 | *****				|	
		; |	SP + 2*NBINS + 4 | catbuf + r			|
		; |	SP + 2*NBINS + 6 | rms_index + r		|
		; |	SP + 2*NBINS + 8 | ***					|
		; |	SP + 2*NBINS + 9 | *****				|
		; |	SP + 2*NBINS + 10| *****				|
		; |	SP + 2*NBINS + 11| *****				|
		; |	SP + 2*NBINS + 12| *********			|
		; |	SP + 2*NBINS + 13| *****				| 
		; 
		; to assign
		; |	SP + 2*NBINS + 10| XAR2					|
		; |	SP + 2*NBINS + 12| XAR3					| 
		; |	SP + 2*NBINS + 14| XAR5					|
		; |	SP + 2*NBINS + 16| deqnt				| 
		; | SP + 2*NBINS + 18| 						| 
		
	
	
		; AC0 ->
		; AC1 -> 
		; AC2 ->
		; AC3 -> man
		;
		; XAR0 -> expindex
		; XAR1 -> manindex
		; XAR2 -> fixdithertab 
		; XAR3 -> deqnt
		; XAR4 -> RA_TNI_fixrootpow2man_new[(rmsindex_r[r] & 0x01)] , i.e pts to man 
		; XAR5 ->  bitstrmPtr->_lfsr
		;
		; XAR6 -> kptr
		; XAR7 -> sptr
		;
		; T0 ->
		; T1 -> exp
		;-----------------------------------------------------

		XAR7 = XSP
		XAR6 = XSP
		; XAR6 = k[]	
		mar( AR7 + #NBINS) 
		; XAR7 = s[] 
		||
		XAR4 = dbl(*SP(#(2*NBINS+6)) )
		; XAR4 -> rms_index + r
		
		T1 = *AR4+;
		; T1 = rms_index[r]
		; XAR4 -> rms_index + r + 1
		||
		T0 = T2
		; T0 = cat
		
		dbl(*SP(#(2*NBINS+6)) ) = XAR4
		; store "rms_index + r + 1" on stack
;		||
		T0 = T0 << #1
		; T0 = cat << 1 ; for dword addressing
		
		XAR0 = #_RA_TNI_fqct_exp
		
		XAR1 = #_RA_TNI_fqct_man
		
		XAR0 = dbl(*AR0(T0) )
		; XAR0(expindex) = RA_TNI_fqct_exp[cat]
		||
		AC0 = T1
		; AC0 = rms_index[r]

		dbl(*SP(#(2*NBINS+10)) ) = XAR2
		; save XAR2 on stack
		dbl(*SP(#(2*NBINS+12)) ) = XAR3
		; save XAR3 on stack
		dbl(*SP(#(2*NBINS+14)) ) = XAR5
		; save XAR5 on stack)
		
		XAR1 = dbl(*AR1(T0) )
		; XAR1(manindex) = RA_TNI_fqct_man[cat]
		||
		T1 = T1 & 0x01 
		; T1 = (rmsindex_r[r] & 0x01) 
		
		XAR4 = _RA_TNI_fixrootpow2man_new
		
		AC0 = AC0 + #64
		; AC0 = rms_index[r] + 64
		||
		T1 = T1 << #1
		; T1 = (rmsindex_r[r] & 0x01) << 1 : for dword addressing

		AC0 = AC0 >> #1
		; AC0 = (rms_index[r] + 64 )/ 2
		||
		AR4 = AR4 + T1
		; XAR4 ->	&RA_TNI_fixrootpow2man_new[(rmsindex_r[r] & 0x01)]

		XAR3 = XSP
		||
		BRC0 = #(NBINS-1)
		;BRC0 ->trip count for loop = NBINS - 1 

		XAR2 = _RA_TNI_fixdither_tab
	
		T1 = AC0 - #31
		; T1(exp)  = (rms_index[r] + 64 )/ 2 - 31
		
		AR3 = AR3 + #(2*NBINS+16)
		; XAR3 -> deqnt
		
		AR5 = AR5 + #( RA_TNI_Obj._lfsr) 
		; AR5 -> bitstrmObj->lfsr

		T0 = *AR6
		; T0 = *kptr
		||			
		blockrepeat{
		; for( i = 0 ; i < NBINS ; i++ ) {
			;-----------------------------------------------------
			; current configuration
			;
			; XAR0 -> expindex
			; XAR1 -> manindex
			; XAR2 -> RA_TNI_fixdither_tab
			; XAR3 -> deqnt
			; XAR4 -> man
			; XAR5 -> bitstrmObj->_lfsr
			; XAR6 -> kptr
			; XAR7 -> sptr
			; XCDP -> mlt
			; T0 -> *kptr , to assign exp2
			; T1 -> exp
			; T2 -> cat
			; T3 -> r ( cregions to 0 )	
			
			; AC0 -> 
			; AC1 -> 
			; AC2 -> 
			; AC3 -> 
			;---------------------------------------------------
			
			if( T0 != 0 ) goto RA_TNI_DecodeVectors_ScalarDequant_L5 ;if( *kptr != 0 ) 
			||
			AC2 = *AR7
			; AC2 = *sptr
							
			; if( *kptr == 0 ) {
				AC1 = dbl(*AR5)
				; AC1 = bitstrmObj->_lfsr
				||
				T0 = T2
				; T0 -> cat
								
				AC3 = AC1 << #-31
				; AC3(sign) = bitstrmObj->_lfsr >> 31
				||
				AC0 = dbl(*AR5) 
				; AC0 = bitstrmObj->_lfsr
				
				AC1 = AC3 & #FEEDBACK
				; AC1 = sign & FEEDBACK
				
				AC1 = AC1 ^ (AC0 <<< #1) 
				;AC1 = (sign & FEEDBACK) ^ (bitstrmObj->_lfsr <<< 1)
				||
				T0 = T0 << #1
				; T0 = cat << 1 for dword addressing
				
				dbl(*AR5) = AC1
				; bitstrmPtr->_lfsr = (sign & FEEDBACK) ^ (bitstrmObj->_lfsr <<< 1)
				||
				AC1 = dbl(*AR2(T0) )
				; AC1 = RA_TNI_fixdither_tab[cat]								
				
				AC1 = AC1 ^ AC3 
				; AC1 = RA_TNI_fixdither_tab[cat] ^ sign
				||
				T0 = T1
				; T0(exp2) = exp
				
				AC1 = AC1 - AC3
				; AC1 = (fixdequant ^ sign) - sign
				
				dbl(*AR3) = AC1
				; fixdqnt = (fixdequant ^ sign) - sign
				||
					
			; }
			goto RA_TNI_DecodeVectors_ScalarDequant_L6
						
RA_TNI_DecodeVectors_ScalarDequant_L5:
			; if( *kptr != 0 ) {
			
				AC3 = -AC2
				;AC3 = -*sptr
			
				AC1 = T1 + *AR0(T0)
				; AC1 = exp + expindex[*kptr]
				||
				T0 = T0 << #1
				; T0 = kptr << 1 : for dword addressing
			
				AC2 = dbl(*AR1(T0))
				; AC2 = manindex[*kptr]
				||
				T0 = AC1
				; T0(exp2) = exp + expindex[*kptr]
			
				AC2 = AC2 ^ AC3 
				; AC2 = AC2 ^ sign
				
				AC2 = AC2 - AC3
				; AC1 = (fixdequant ^ sign) - sign
				
				dbl(*AR3) = AC2
				; fixdqnt = (fixdequant ^ sign) - sign
							
			; }

RA_TNI_DecodeVectors_ScalarDequant_L6:
			; T0 -> exp2
			; XAR3 -> fixdqnt
			; XAR4 -> man
			; XCDP -> mlt
			AC2 = UNDERCLIP
			||
			mar(*AR3+)
			; AR3 -> fixdqnt_l
		
			AC1 = OVERCLIP
		
			mar(*AR4+)
			; AR4 -> man_l
			||	
			TC1 = T0 < AC2
			; TC1 = ( exp2 < UNDERCLIP ) ? 1 : 0

			AC0 = uns(*AR3-) * uns(*AR4)
			; AC0 = fixdeqnt_l * man_l
			
			if( TC1 ) execute(AD_Unit)			; if( exp2 < UNDERCLIP )
			||	T0 = AC2						; 	T0(exp2) = UNDERCLIP
				
			TC1 = T0 > AC1
			; TC1 = ( exp2 > OVERCLIP ) ? 1 : 0
			||
			mar(*AR7+)
			; sptr++

			
			AC0 = (AC0 >> #16) + ( (*AR3+) * uns(*AR4-)	)
			; AC0 = (fixdeqnt_h * man_l)+ (fixdeqnt_l * man_l >> 16)		
			
			if( TC1 ) execute(AD_Unit)			; if( exp2 > OVERCLIP )
			||	T0 = AC1						; 	T0(exp2) = OVERCLIP
				
			AC0 = AC0  + ( uns(*AR3-) * (*AR4)	)
			; AC0 = (fixdeqnt_l * man_h) +  (fixdeqnt_h * man_l) + (fixdeqnt_l * man_l >> 16)		
			
			T0 = T0 - #(MLTBITS-2)
			; T0 = exp2 - (MLTBITS-2)	; for right shifting
			
			AC0 = (AC0 >> #16) + ( (*AR3) * (*AR4)	)
			; AC0 = (fixdeqnt_h * man_h) + ((fixdeqnt_l * man_h) +  (fixdeqnt_h * man_l) + (fixdeqnt_l * man_l >> 16) ) >> 16
			
			AC0 = AC0 << T0
			; AC0 = (fixdeqnt * man) >> ((MLTBITS-2)- exp2)
			||
			T0 = *+AR6
			; T0 = *kptr
;		
;			mar(*AR6+)
			;kptr++
			
			dbl(*CDP+) = AC0
			; *mlt++ = (fixdeqnt * man) >> ((MLTBITS-2)- exp2)
			
;			T0 = *AR6
			; T0 = *kptr
;			||
;			mar(*AR7+)
			; sptr++
			
		} ; end of for loop
		XAR2 = dbl(*SP(#(2*NBINS+10)) ) 
		; get XAR2 from stack
		XAR3 = dbl(*SP(#(2*NBINS+12)) ) 
		; get XAR3 from stack
		XAR5 =  dbl(*SP(#(2*NBINS+14)) )
		; get XAR5 from stack)
		; XAR2 -> pkbit
		; XAR3 -> pktotal
		; XAR5 -> bitstrmPtr
	
		
		
		;***********************************************************************************
		; End of ScalarDequant
		;***********************************************************************************



RA_TNI_DecodeVectors_End_L1:
	
		XAR0 = dbl(*SP(#(2*NBINS+4)) )
		; XAR0 = catbuf + r 
		;-----------------------------------------------------
		; current configuration
		;
		; XAR0 -> catbuf + r
		; XAR1 -> ****
		; XAR2 -> pkbit
		; XAR3 -> pktotal
		; XAR4 -> *****
		; XAR5 -> bitstrmPtr 
		; XAR6 -> ***
		; XAR7 -> ****
		; XCDP -> mlt
		; T0 -> ****
		; T1 -> ****
		; T2 -> cat
		; T3 -> r ( cregions to 0 )
;		||			
		T3 = T3 - #1 
		; r--
		if( T3 > 0 ) goto RA_TNI_DecodeVectors_L1 
	; repeat loop
	; }

	;------------------------------------------------------------------------
	; to set non-coded regions to zero 
	AC0 = *SP(#(2*NBINS+1))	* #(NBINS)
	; AC0 = cregions * NBINS
	
	T0 = *AR5(#(RA_TNI_Obj.nsamples)) - AC0
	; T0 = bitstrmPtr->nsamples - cregions * NBINS
	
	mar(T0 - #1)
	; reduce the count
	
	if( T0 < 0 ) goto RA_TNI_DecodeVectors_EXIT	; if count is -ve go ahead
	||	
	CSR = T0
	; load CSR with trip count =  bitstrmPtr->nsamples - cregions * NBINS - 1
	
	AC1 = #0 
	||
	repeat(CSR)		; do (bitstrmPtr->nsamples - cregions * NBINS )times
		dbl(*CDP+) = AC1
		
	;----------------------------------------------------------------------
	; epilog code
	;
RA_TNI_DecodeVectors_EXIT:	

	; SP -> even
	SP = SP + LOCAL_SIZE 
	; SP -> odd
	
		.ARMS_on
	bit(ST2, #15) = #1
	; set the ARMS bit to enable control mode
	||
	XCDP = popboth() 
	XAR7 = popboth() 	
	XAR6 = popboth() 	
	XAR5 = popboth() 	
	T2,T3 = pop()
	; restore regs. and exit
	return 



;------------------------------------------------------------------------------------------------
; _RA_TNI_DecodeVectors : END
;------------------------------------------------------------------------------------------------

	.endif	; DECODEVECTORS_ASM             
	
	.endif	; CONST_TABLE_DMA		 DMA disabled