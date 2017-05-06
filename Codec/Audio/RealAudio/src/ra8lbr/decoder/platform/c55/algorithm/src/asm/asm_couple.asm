; ***** BEGIN LICENSE BLOCK *****   
; Source last modified: $Id: asm_couple.asm,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
; 	File 		:	asm_couple.asm 
;   Description : C55X assembly routines for functions present in couple.c
;
;				:RA_TNI_DecodeCoupleInfo
;
;
;------------------------------------------------------------------------------------------------------------
	.mmregs

	.if	DECODECOUPLEINFO_ASM
	
	.include coder.h55  	
	
	.def _RA_TNI_DecodeCoupleInfo
	
	.global _RA_TNI_DecodeNextSymbolWithTable	; defined in huffmantables.c

	.global _RA_TNI_cpl_table_tab		;HuffmanTable const RA_TNI_cpl_table_tab[7] 
	.global _RA_TNI_cpl_bitcount_tab	;const int RA_TNI_cpl_bitcount_tab[7] 
										;	 both defined in "cpl_tables.c"		
	.global _RA_TNI_cplband				 	;const short RA_TNI_cplband[MAXREGNS]  defined in couple.c
									
						
										
										

;------------------------------------------------------------------------------------------------
; _RA_TNI_DecodeCoupleInfo : ( C-callable)
; 
;/* decodes coupling data from bitstream
; * data was either Huffman coded or stored directly, whichever is more efficient
; * the hufmode flag (one bit) indicates which format was used
; *
; * Fixed-point changes:
; *  - none
; */
; 
;	Input : XAR0 -> cplindex (USHORT*)
;		  : XAR1 -> pkbit (short*)
;		  : XAR2 -> pktotal (short*)
;		  ; XAR3 -> bitstrmPtr(short*)	
;
;   Output : nbits (int)
;
; 	Registers touched : XAR0, XAR1, XAR2,XAR3,XAR4,XAR5,XAR6,XAR7,XCDP, T0, T1, 



_RA_TNI_DecodeCoupleInfo:
	; SP -> odd
	;----------------------------------------------------------------------
	; prolog code
	;
	push(T2,T3)
	pshboth(XAR5)		; save for restore on exit
	pshboth(XAR6)
	pshboth(XAR7)
	||
	bit(ST2, #15) = #0
	; clear the ARMS bit to use DSP Mode addressing
	.ARMS_off
	pshboth(XCDP)
	; SP -> odd
	
	;----------------------------------------------------------------------
	;  current assignments
	; 
	; XAR0 -> cplindex (short*)
	; XAR1 -> pkbit (short*)
	; XAR2 -> pktotal (short*)
	; XAR3 -> bitstrmPtr(short*)	
	;
	;  To assign 
	; 
	; XAR5 -> cplindex[b]
	; XAR6 -> pkbit
	; XAR7 -> pktotal
	; XCDP -> bitstrmPtr
	;
	; T0 -> bitstrmPtr-> cplstart, bandstart
	; T1 -> bitstrmPtr->nregions
	; T2 -> nbits
	; AC0 -> bandend - bandstart 
	.noremark 5673
	.noremark 5573	
	.noremark 5571	
	.noremark 5505	
	XAR4 = _RA_TNI_cplband
	
	T0 = *AR3( #(RA_TNI_Obj.cplstart) )
	; T0 = bitstrmPtr->cplstart
	
	T1 = *AR3( #(RA_TNI_Obj.nregions) ) 
	; T1 = bitstrmPtr->nregions
	
	
	T0 = *AR4( T0 )
	; T0(bandstart) = RA_TNI_cplband[bitstrmPtr->cplstart]
	||
	mar(T1 - #1)
	; T1 = bitstrmPtr->nregions - 1
	
	T3 = *AR4(T1) - T0
	;T3 = RA_TNI_cplband[bitstrmPtr->nregions - 1] - bandstart
	||
	T2 = #0
	; nbits = 0
	
	XAR5 = XAR0 
	; XAR5 = cplindex
	
	
	AR5 = AR5 + T0 
	; XAR5 = cplindex[bandstart]
	||
	;***********************************************************************
	;RA_TNI_Unpackbit( )
	;**********************************************************************

	;-----------------------------------------------------
	; current assignments
	;
	; XAR0 -> 	cplindex
	; XAR1 -> pkbit
	; XAR2 -> pktotal
	; XAR3 -> bitstrmPtr 
	; XAR4 -> ******
	; XAR5 -> &cplindex[bandstart]
	; XAR6 -> ******
	; XAR7 -> ****
	; XCDP -> *****
	; T0 -> *****
	; T1 -> ******
	; T2 -> 0
	; T3 -> RA_TNI_cplband[bitstrmPtr->nregions - 1] - bandstart
	;
	;
	; to assign
	;
	; AC0 ->(*pktotal ) - bitstrmPtr->nframebits
	; AC1 -> (*pkbit + 1) >> 5
	; AC3 -> data
	;
	; XAR4 -> bitstrmp + pkptr
	; XAR6 -> bitstrmPtr->pkptr
	; XAR7 -> pktotal
	; XCDP -> bitstrmptr
	;
	; T0 -> *pkbit
	; T2 = 0 or 1
	;-----------------------------------------------------
	
	*AR2 = *AR2  + #1
	; *pktotal = *pktotal + 1
	AC0 = *AR3( #(RA_TNI_Obj.nframebits) )
	; AC0 = bitstrmPtr->nframebits
	
	XAR4 = XAR3 ;
	; XAR4 = bitstrmPtr
	
	AC0 = *AR2 - AC0
	; AC0 = *pktotal - bitstrmPtr->nframebits
	||
	AR4 = AR4 + #(RA_TNI_Obj.pkptr)
	; XAR4 = bitstrmPtr + pkptr
	
	XAR6 = dbl(*AR4)
	; XAR6 = bitstrmPtr->pkptr
	||
	mar(T2 = #1)
	; nbits = 1
	
	T0 = *AR1
	; T0 = *pkbit
	||
	AC1 = *AR1
	; AC1 = *pkbit
	nop;added for silicon exception giving R5505
			
	if( AC0 > 0 ) execute(AD_Unit)				; if ( *pktotal > bitstrmPtr->nframebits )
	||	T2 = #0	
	
	AC3 = dbl(*AR6)
	; AC3 = *(bitstrm->pkptr)
	||
	AC1 = AC1 + #1
	; AC1 = *pkbit + 1
	
	AC3 = AC3 <<< T0 
	; AC3 (data) = *(bitstrm->pkptr) << *pkbit
	||
	*AR1 = AC1
	; *pkbit = *pkbit + 1
	
	AC1 = AC1 <<< #-5
	; AC2 = (*pkbit + 1) >> 5
	||
	XCDP = XAR3
	; XCDP = bitstrmPtr
	
	AC3 = AC3 <<< #-31
	; AC3 = (*(bitstrm->pkptr) << *pkbit) >> 31
	||
	XAR7 = XAR2
	; XAR7 = pktotal
	
	AR6 = AR6 + AC1
	; AR6 = bitstrmPtr->pkptr +  (*pkbit + 1) >> 5
	
	dbl(*AR4) = XAR6
	; *(bitstrmPtr+pkptr) = XAR6
	
	*AR1 = *AR1 & #0x1F
	; *pkbit &= 0x1F
	
	;***********************************************************************
	;End of RA_TNI_Unpackbit( )
	;**********************************************************************
	; AC3 = data(hufmode)
	if( AC3 == 0 ) goto RA_TNI_DecodeCoupleInfo_L1 ; if( !hufmode ) goto L1

	XAR6 = XAR1
	; XAR6 = pkbit	

	;-----------------------------------------------------
	; current assignments
	;
	; XAR0 -> 	cplindex
	; XAR1 -> pkbit
	; XAR2 -> pktotal
	; XAR3 -> bitstrmPtr 
	; XAR4 -> ******
	; XAR5 -> &cplindex[bandstart]
	; XAR6 -> pkbit
	; XAR7 -> pktotal
	; XCDP -> bitstrmPtr
	; T0 -> ******
	; T1 -> ******
	; T2 -> nbits
	; T3 -> RA_TNI_cplband[bitstrmPtr->nregions - 1] - bandstart
	;
	;
	
	
	
	; if ( hufmode ) {
RA_TNI_DecodeCoupleInfo_L2:
		; do bandend - bandstart + 1 times
	
		T0 = *CDP(#(RA_TNI_Obj.cplqbits) )  
		; T0 = bitstrmPtr->cplQbits
		
		XAR0 = #_RA_TNI_cpl_table_tab

		XAR2 = XAR6
		; XAR2 = pkbit
		
		XAR3 = XAR7
		; XAR3 = pktotal
		
		XAR1 = #_RA_TNI_cpl_bitcount_tab
	
		T1 = *AR1(T0)	
		; T1 = RA_TNI_cpl_bitcount_tab[bitstrmPtr->cplqbits] 
		||
		T0 = T0 << #1
		; for dword addressing
				
		XAR0 = dbl(*AR0(T0) )
		; XAR0 = RA_TNI_cpl_table_tab[bitstrmPtr->cplqbits] 
				
		XAR1 = XAR5
		; XAR1(*val) = &cplindex[b]
		
		AR5 = AR5 + #1
		; AR5 = &cplindex[b++]
		||
		mar(T0 = T1)	
		; T0 = RA_TNI_cpl_bitcount_tab[bitstrmPtr->cplqbits] 
		
		XAR4 = XCDP
		; XAR4 = bitstrmPtr
		
		call _RA_TNI_DecodeNextSymbolWithTable	
		||	
		T3 = T3 - #1
		; next count
		
		T2 = T2 + T0
		; T2 = nbits + return val
		||
		if( T3 >= 0) goto RA_TNI_DecodeCoupleInfo_L2
		; repeat
		
		goto RA_TNI_DecodeCoupleInfo_EXIT
	;}
	
RA_TNI_DecodeCoupleInfo_L1:	
	; if (!hufmode){
		;-----------------------------------------------------
		; current assignments
		;
		; XAR0 -> 	cplindex
		; XAR1 -> pkbit
		; XAR2 -> pktotal
		; XAR3 -> bitstrmPtr 
		; XAR4 -> ******
		; XAR5 -> &cplindex[bandstart]
		; XAR6 -> ****
		; XAR7 -> ****
		; XCDP -> *****
		; T0 -> ******
		; T1 -> ******
		; T2 -> nbits
		; T3 -> RA_TNI_cplband[bitstrmPtr->nregions - 1] - bandstart
		;
		;
		; To assign
		;   BRC0 -> trip count
		;   T3 = bitstrmPtr -> cplQbits
		;
		;   AC0 = bitstrmPtr->nframebits
		;   XAR3 -> bitstrmPtr + pkPtr
		;   XAR4 -> bitstrmPtr->pkptr
		
		
		BRC0 = T3
		; trip count BRC0 = bandend - bandstart
		
		T3 = *AR3(#(RA_TNI_Obj.cplqbits) )	
		; T3 = bitstrmPtr->cplqbits
		
		AC0 = *AR3(#(RA_TNI_Obj.nframebits) )
		; AC0 = nframebits
	
		mar(AR3 + #(RA_TNI_Obj.pkptr))
		
		T1 = T3 -#32
		; T1 = cplqbits - 32
	
		XAR4 = dbl(*AR3) 
		; XAR4 = bitstrmPtr->pkptr
		||
		blockrepeat {		; do bandend - bandstart + 1 times
		
			AC1 = *AR2 + T3
			; AC1 = *pktotal + cplqbits
			||
			T0 = *AR1
			; T0 = *pkbit
		
			AC2 = dbl(*AR4)
			; AC2 = *bitstrmPtr->pkptr
			
			*AR2 = AC1
			; *pktotal= *pktotal + cplqbits
			||
			AC1 = AC1 - AC0
			; AC1 =  *pktotal + cplqbits - nframebits
			
			AC2 = AC2 <<< T0
			; AC2(temp) = (*bitstrmPtr->pkptr)<<< *pkbit
			||
			T0 = T0 + T3
			; T0 = *pkbit + cplqbits
		
			.noremark 5503	
			if( AC1 <= 0 ) execute(AD_Unit)			; if(*pktotal + cplqbits <= nframebits)
			|| T2 = T2 + T3							; nbits = nbits + cplqbits
			.remark  5503
			
			T0 = T0 - #32
			; T0 = *pkbit  + cplqbits - 32
			||
			AC1 = T0	
			; AC1 = *pkbit  + cplqbits
			
			if( T0 < 0 ) goto RA_TNI_DecodeCoupleInfo_L3		 ; if( *pkbit  + cplqbits < 32) 
			
			;if( *pkbit  + cplqbits >= 32) 	{
				AR4 = AR4 + #2
				; AR4 = bitstrmPtr->pkptr + #2
				||
				AC1 = T0
				; AC1 = *pkbit  + cplqbits - 32

				AC3 = dbl(*AR4)
				; AC3 = *(bitstrmptr->pkptr+2)				
				||
				T0 = T0 - T3
				; T0 = (*pkbit  + cplqbits - 32) - cplqbits
				
				AC3 = AC3 <<< T0
				; AC3 = *(bitstrmptr->pkptr+2)>>>(32 - *pkbit) 
	;--------------------------------------------------------------------			
	;			||	 NOTE : This paralleism
	;				causes incorrect results while executing (not single 
	;				stepping !!) for AC3 = FF956D5C31
	;							and T0 = FFFB
	;--------------------------------------------------------------------
				dbl(*AR3) = XAR4
				; bitstrmPtr->pkptr = bitstrmPtr->pkptr + 2
				
				AC2 = AC2 | AC3
				; AC2(temp) = temp | *(bitstrmptr->pkptr)>>>(32 - *pkbit) 
			;}
			
RA_TNI_DecodeCoupleInfo_L3:				
			*AR1 = AC1
			; *pkbit = *pkbit + cplqbits (-32)		
			||
			AC2 = AC2 <<< T1
			; AC2 = temp >>> (32-nbits)
			
			
			*AR5+ = AC2
			; cplindex[b++] = AC2
		}
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																				
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																		

	;}
	
	
	;----------------------------------------------------------------------
	; epilog code
	;
RA_TNI_DecodeCoupleInfo_EXIT:	

		.ARMS_on
	bit(ST2, #15) = #1
	; set the ARMS bit to enable control mode
	||
	XCDP = popboth() 
	XAR7 = popboth() 	
	||
	T0 = T2
	; T0 = nbits
	XAR6 = popboth() 	
	XAR5 = popboth() 	
	T2,T3 = pop()
	.remark 5673
	.remark 5573	
	.remark 5571	
	.remark 5505
	; restore regs. and exit
	return 

;------------------------------------------------------------------------------------------------
;End of _RA_TNI_DecodeCoupleInfo
; 
	.endif	;DECODECOUPLEINFO_ASM
