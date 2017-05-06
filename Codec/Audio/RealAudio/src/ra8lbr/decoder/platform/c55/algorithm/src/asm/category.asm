; ***** BEGIN LICENSE BLOCK *****   
; Source last modified: $Id: category.asm,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
; 	File 		:	assembly_category.asm 
;   Description : C55X assembly routines for functions present in category.c
;
;				:RA_TNI_Categorize
;
; NOTE : Define [CATEGORIZE_ASM .set 0x01] to enable assembly version of _RA_TNI_Categorize
;
;------------------------------------------------------------------------------------------------------------
; NOTE : Assembly implementation of merged version of RA_TNI_Categorize and RA_TNI_ExpandCategory
	.mmregs

	.if	CATEGORIZE_ASM

	
	.include coder.h55  	
	
	.def _RA_TNI_Categorize

	.global _RA_TNI_expbits_tab				;const char RA_TNI_expbits_tab[8] = { 52, 47, 43, 37, 29, 22, 16, 0 };
									; defined in tables.c

;------------------------------------------------------------------------------------------------
; _RA_TNI_Categorize : ( C-callable)
; 
;/*
; * Compute the categorizations.
; * This is a symmetrical operation, so the decoder goes through
; *   the same process as the encoder when computing the categorizations.
; *   The decoder builds them all and chooses the one given by rate_code,
; *		which is extracted from the bitstream by RA_TNI_DecodeEnvelope()
; * rms_index[i] is the RMS power of region i
; * NCATZNS = (1 << RATEBITS) = max number of categorizations we can try
; *
; * cat runs from 0 to 7. Lower number means finer quantization.
; * RA_TNI_expbits_tab[i] tells us how many bits we should expect to spend
; *   coding a region with cat = i
; *
; * When finished:
; *   catbuf contains the categorization specified by rate_code
; *
; */
;
; Input : XAR0 -> rmsindex (short*)
;		: XAR1 -> catbuf (short*)
;		: XAR2 -> bitstrmPtr (RA_TNI_Obj*)
;
;		: T0 -> availbits (short)
;		: T1 -> ncatzns (short)
;		: AR3 -> rate_code (short)
;
; Returns : void
;
; Registers touched : XAR0, XAR1, XAR2, XAR3, XAR4, T0, T1, T2, T3BRC0, XDP
;						AC0 ; AC1, AC2, AC3
;
; NOTE : The space after catbuf is reused to allocate the locals for this
; 		function, and accessed via the XDP pointer
;
;------------------------------------------------------------------------------------------------

	.cpl_on					; compiler mode on !!
	
;	.data
;LONGMAX	.long 0x7fffffff		; upheap sentinel for maxheap	NOT USED !!
;LONGMIN	.long 0x80000000		; upheap sentinel for minheap	NOT USED !!											

	.eval	2*MAXCATZNS, MAXCATZNS2
	.eval 	2*(MAXCREGN + 1), HEAPSIZE				; long maxheap[MAXCREGN+1]
	.eval   MAXCREGN +  MAXCATZNS2 + (HEAPSIZE)*2 + 9, LOCAL_SIZE		; LOCAL_SIZE is the stack size for locals (odd no)
																		; currently 779			
	;[fj] 19/02/2003 : Reducing stack size 																		
	.eval   MAXCREGN + 2, XDP_ADJUST ; Amount to increment XDP by, to 
								  	; accomodate catbuf already on buffer
								  		 
DP_C55x	.set 0x002E			; mmap address of DP register	
	
	.text
_RA_TNI_Categorize:
    .noremark 5573
    .noremark 5673
    .noremark 5549
    .noremark 5505
    .noremark 5571 
	; SP -> odd
	;----------------------------------------------------------------------
	; prolog code
	
	push(T2,T3)
	; save for later restore
	||
	bit(ST2, #15) = #0
	; clear the ARMS bit to use DSP Mode addressing
	.ARMS_off
	
	; adjustment to compensate for different statistics at higher bitrates
	AC0 = T0 - *AR2(#(RA_TNI_Obj.nsamples))
	; AC0 = availbits - bitstrmPtr->nsamples
	AC1 = AC0 << #16
	; AC1(31-0) = availbits - bitstrmPtr->nsamples
	||
	bit(ST1, ST1_CPL ) = #0
	; clear the cpl bit to use dp offset addressing
	.cpl_off
	

	pshboth(XAR5)
	; save for later restore
	||	
	AC1 = AC1*#5
	; AC1 = (availbits - bitstrmPtr->nsamples)*5

	pshboth(XAR6)
	; save for later restore
	||
	AC1 = AC1 << #-3	
	; AC1 = ((availbits - bitstrmPtr->nsamples)*5) >> 3
	
	XDP = XAR1
	; XDP -> catbuf

	pshboth(XAR7)
	; save for later restore
;	||
	AC2 = #32
	; AC2(delta) = #32
	
	pshboth( XCDP)
	; save for later restore
	||
	AC3 = 0x07
	
	XCDP = XAR0
	; XCDP = rms_index

	; DP -> even
	*(#DP_C55x) = *(#DP_C55x) + #XDP_ADJUST
	; adjust XDP to account for  catbuf on  buffer
	; XDP -> even
	
	; The local frame configuration : To set up ::	
	;------------------------------------------------------------------------
	;
	;	DP ->   		|   	availbits		| 	
	;	(even)			|		ratecode		|
	;					|		ncatzns			|
	;		 	 		|		nminheap		|	
	;		 	 		|	  	nmaxheap		| 	
	;		 	 		|	  	minptr			| 	
	;		 	 		|	  	maxptr			| 	
	;				    |		cat				|
	;	DP + 8 ->   	|   	 minheap[0]		| 	<- XAR7
	;	  	 		 	:			.			:
	;		 	 		:			.			:	
	;		 	 		|	  minheap[MAXCREGN]	| 	<- XAR7 + 2*MAXCREGN	
	; 	+HEAPSIZE->     |	 	 maxheap[0]		|	<- XAR6
	;		  	 	 	:			.			:
	;			 	 	:			.			:	
	;			 	 	|	  maxheap[MAXCREGN]	| 	<- XAR6 + 2*MAXCREGN	
	; 	+HEAPSIZE->	 	|   	changes[0]		| 	<- XAR5
	;	  	 	 		:			.			:
	;		 	 		:			.			:	
	;		 	 		| changes[MAXCATZNS2-1]	| 	<- XAR5 + MAXCATZNS2 - 1
	;	+MAXCATZNS2->   | 	  mincat[0]			|	<- XAR4
	;	  	 	 		:			.			:
	;		 	 		:			.			:	
	;		 	 		|   mincat[MAXCREGN-1]	| 	<- XAR4 + MAXCREGN - 1
	;					| 		-hole-			|	
	;------------------------------------------------------------------------
	
 
	;**********************************************************************************************	
	;----------------------------------------------------------------------
	; Function code
	;
	;**------------------------------
	;Current assignments :
	;
	;	XCDP	:  rms_index
	;
	;	XAR0	: rms_index
	;	XAR1 	: catbuf
	;	XAR2 	: bitstrmPtr
	;	XAR4	: 
	;	XAR5	: 
	;   XAR6 	: 
	;	XAR7	: 
	;
	;	AR3		:	rate_code	( to assign to RA_TNI_expbits_tab )
	; 
	; 	T0		:	availbits
	; 	T1		:	ncatzns
	; 	T2		:	cregions (to assign)
	;	T3		:   offset	(-32)
	;
	; 	AC0		: availbits - bitstrmPtr->nsamples
	;   AC1		: ((availbits - bitstrmPtr->nsamples)*5) >> 3 
	;   AC2		: delta (32)
	;   AC3		: 0x07
	;
	;	XDP		:	locals
	; 
	;**--------------------------------
	nop				; workaround for CPU_84 issue with this instrn
	nop
	nop
	nop
	
	@2 = T1
	; store ncatzn on stack
	||	
	if( AC0 > 0 ) execute (AD_Unit ) 					; if ( availbits > bitstrmPtr->nsamples)
		T0 = *AR2(#(RA_TNI_Obj.nsamples)) + AC1		;		T0 = bitstrmPtr->nsamples + AC1


	;**********************************************************
	; Initial categorization process
	
    T2 = *AR2( #(RA_TNI_Obj.nregions) )
    ; T2 = bitstrmPtr->nregions
    T2 = T2 + *AR2(#(RA_TNI_Obj.cplstart))
	; T2 (cregions) = bitstrmPtr->nregions + bitstrmPtr-> cplstart
	
	mar(T3 = #-32)
	; T3(offset) = -32
	
	@1 = AR3
	; store rate_code on stack
	
	XAR3 = _RA_TNI_expbits_tab
	; XAR3 -> expbits tab
	
	mar(T2 - #1)
	; T2 = cregions - #1 (trip count )
	||
	@0 = T0
	; store  availbits on stack
	
INIT_CATZN_LOOP:
	
		BRC0 = T2
		; load trip count of cregions - #1 in BRC0
		||
		AC0 = 0
		; AC0 (expbits)
	
					
		T3 = T3+AC2
		; T3 = offset + delta	
		||
		;**------------------------------
		;Current assignments :
		;
		;	CDP		: rate_code
		;
		;	XAR0	: rms_index
		;   XAR3    : RA_TNI_expbits_tab
		; 
		; 	T0		:	cat (to assign)
		; 	T1		:	ncatzns (mirrored on stack)
		; 	T2		:	cregions -1
		;	T3		:   offset	(-32)
		;
		;	AC0 	: expbits (0) 
		;	AC1 	:  
		;   AC2		: delta (32)
		;   AC3		; 0x07
		;
		;	XDP 	: locals
		;**--------------------------------
		; do 'cregion' times
		localrepeat{		
			T0 = T3 - *AR0+
			; T0 = offset + delta - *rms_index++ 
			T0 = T0 >> #1
			; T0 = (offset + delta - *rms_index ) >> 1
			nop;added to void silicon exception causing remark 5505
			if( T0 < 0 ) execute (AD_Unit)
			||	T0 = #0
			; if( cat < 0 ) cat = 0
			
			T0 = min( AC3, T0)
			; T0 = min( 7, cat)
		
			AC0 = AC0 + *AR3( T0 )
			; AC0 (expbits) = 	AC0 + RA_TNI_expbits_tab[cat]
		}
		
		; calculate expbits - availbits + 32
		; and realign AR0 to point to rms_index
		
		AC0 = AC0 - @0
		; AC0 = expbits - availbits (on stack)
		||
		AR0 = CDP
		; XAR0 -> rms_index
		
		AC0 = AC0 + #32
		; AC0 = expbits - availbits + 32
		nop;added to avoid silicon exception causing remark 5505		
		if( AC0 < 0 ) execute (AD_Unit)		; if( expbits < availbits - 32 ) then reset offset to original value
		||	T3 = T3 - AC2					;    else offset is updated to offset + delta
		; T3(offset) = T3-delta
		
		AC2 = AC2 >> #1
		; delta  = delta >> 1
						
	if(AC2 != 0 ) goto INIT_CATZN_LOOP
	; repeat until (delta == 0)
	;**********************************************************
	
	AC0 = #1				
	; dbl(*(#LONGMIN) )
	||
	; set up the pointers to maxheap, minheap, mincat and changes
	XAR7 = XDP
	; set up XAR7 to make it point to minheap array
	mar(AR7 + #8)
	; XAR7 -> minheap
	|| 
	AC0 = AC0 <<< #31
	; AC0 = 0x80000000
	
	XAR6 = XAR7
	; set up XAR6 to make it point to maxheap array
	
	
	mar(AR6 + HEAPSIZE)
	; XAR6 -> maxheap 
	||
	dbl(*AR7) = AC0
	; minheap[0] = 0x80000000
	
	AC0 = AC0 - #1
	; AC0 = 0x7fffffff
	||	
	XAR5 = XAR6
	; set up XAR5 to make it point to changes array
	mar( AR5 + HEAPSIZE )	
	; XAR5 -> changes
	||
	dbl(*AR6) = AC0
	; maxheap[0] = 0x7fffffff
		
	XAR4 = XAR5
	; set up XAR4 to make it point to mincat array
	AR4 = AR4+MAXCATZNS2
	; XAR4 -> mincat
	
	
	
	@3 = #0
	; nminheap = 0 
	
	@4 = #0 
	; nmaxheap = 0 		
	;------------------------------
	;Current assignments :
	;
	;	CDP		: rms_index	 
	;
	;	XAR0	: 0x07
	;	XAR1 	: catbuf
	;	XAR2 	: bitstrmPtr
	;	XAR4	: mincat (MAXCREGN)
	;	XAR5	: changes (MAXCATZNS2)
	;   XAR6 	: maxheap (HEAPSIZE)	( long[] )
	;	XAR7	: minheap (HEAPSIZE)	( long[] )
	;
	;	XAR3	:	RA_TNI_expbits_tab
	; 
	; 	T0		:	cat (to assign) 	
	; 	T1		:	k (to assign)
	; 	T2		:	cregions  - 1 
	;	T3		:   offset	 
	;
	; 	AC0		: expbits(to assign)
	;   AC1		: val (to assign)
	;   AC2		: r (initially 0)
	;   AC3		:  
	;
	;	XDP		:	locals
	; 
	;--------------------------------	
	
	;**********************************************************
	; to use the selected offset to build the min and max heaps
	;**********************************************************
	
	BRC0 = T2
	; load trip count of cregions - #1 in BRC0
	||
	AC0 = #0
	; AC0 (expbits)

	mar(AR0 = #07)
	||
	; do 'cregion' times
	blockrepeat{		
	
		T0 = T3 - *CDP
		; T0 = offset  - *rms_index
		
		T0 = T0 >> #1
		; T0 = (offset  - *rms_index ) >> 1
	    nop;added to avoid silicon exception causing remark 5505
		if( T0 < 0 ) execute (AD_Unit)
		||	T0 = #0
		; if( cat < 0 ) cat = 0
		
		T0 = min( AR0, T0)
		; AR0 = min( 7, cat)
	
		AC0 = AC0 + *AR3( T0 )
		; AC0 (expbits) = 	AC0 + RA_TNI_expbits_tab[cat]
		||	
		*AR4+ = T0
		; *mincat++ = cat
		*AR1+ = T0
		; *catbuf++ = cat
		||		
		AC3 = T0
		; AC3  = cat
	
		AC1 = T3 - *CDP+
		; AC1 = offset -  *rms_index++
						
	    AC1 = AC1 -( AC3<<#1)
	    ; AC1(val) = offset - *rms_index++ - cat<<1
		||	
		bit(ST0, #12) = #0
		; clear TC2
		
	    AC1 = AC1 << #16
	    ; val = val<<16	
		||	    	    	    
	    TC1 =  T0 < AR0
		; TC1 = 1, if (cat < 7)
		
		.noremark 5503
		if( T0 > 0 ) execute(AD_Unit)
		||	bit(ST0, #12) = #1
		; if( cat > 0 ), set TC2
		.remark 5503
		
		AC1 = AC1 | AC2
		; AC1(val) = (val<<16 ) | r
		||
		if( !TC1 )	goto MINHEAP_LABEL		; if ( ! cat < 7 ) , do not process maxheap
		
		
		; if( cat < 7 )  {
		; ----------Build maxheap -----------------
		
		@4 = @4 + #2
		; ++nmaxheap 	(dword addressing)
	
		BRC1 = #0xFFF
		; max trip count in BRC1
		||	
		T0 = @4
		; T0 = nmaxheap
	
		; while(1) 		; almost 
		blockrepeat{
				
			mar(T1 = T0)
			; k -> T0
			||
			T0 = T0 >> #1
			bit(T0, @0 ) = #0 ; for dword addressing
			; T0 = PARENT(k) 
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																				
			
			AC3 = dbl(*AR6(T0))
			; AC3 = maxheap[PARENT(k)]		
			
			TC1 = AC1 > AC3
			; if( val > maxheap[PARENT(k)] ) then TC1 = 1
		
			if( !TC1 ) goto MAXHEAP_WHILE_END
			; if( ! val > maxheap[PARENT(k)] ) then break
			
			dbl(*AR6(T1))= AC3
			; maxheap[k] = maxheap[PARENT(k)]		
		}
MAXHEAP_WHILE_END:		
		dbl(*AR6(T1))= AC1
		; maxheap[k] = val
		;------------------------------------------
		;}
		
		;------------------------------
		;Current assignments :
		;
		;	CDP		: rms_index	+ r 
		;
		;	XAR0	: 0x07
		;	XAR1 	: catbuf + r
		;	XAR2 	: bitstrmPtr
		;	XAR4	: mincat + r 
		;	XAR5	: changes (MAXCATZNS2)
		;   XAR6 	: maxheap (HEAPSIZE)	( long[] )
		;	XAR7	: minheap (HEAPSIZE)	( long[] )
		;
		;	XAR3	:	RA_TNI_expbits_tab
		; 
		; 	T0		:	
		; 	T1		:	
		; 	T2		:	cregions  - 1 
		;	T3		:   offset	 
		;
		; 	AC0		: expbits
		;   AC1		: val
		;   AC2		: r 
		;   AC3		:  
		;
		;	XDP		:	locals
		; 
		;--------------------------------	
			
		
MINHEAP_LABEL
		if( !TC2 ) goto ENDOFHEAP_LABEL		; if ( ! cat > 0 )
		
		; if ( cat > 0 ) {
		; ----------Build minheap -----------------
		
		@3 = @3 + #2
		; ++nminheap 	(dword addressing)
	
		BRC1 = #0xFFF
		; max trip count in BRC1
		||	
		T0 = @3
		; T0 = nminheap
	
		; while(1) 		; almost 
		blockrepeat{
				
			mar(T1 = T0)
			; k -> T0
			||
			T0 = T0 >> #1
			bit(T0, @0 ) = #0 ; for dword addressing
			; T0 = PARENT(k) 
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																				
			
			AC3 = dbl(*AR7(T0))
			; AC3 = minheap[PARENT(k)]		
			
			TC1 = AC1 < AC3
			; if( val < minheap[PARENT(k)] ) then TC1 = 1
		
			if( !TC1 ) goto MINHEAP_WHILE_END
			; if( ! val < minheap[PARENT(k)] ) then break
			
			dbl(*AR7(T1))= AC3
			; minheap[k] = maxheap[PARENT(k)]		
		}
MINHEAP_WHILE_END:		
		dbl(*AR7(T1))= AC1
		; minheap[k] = val
		
				
		;------------------------------------------
		;}

ENDOFHEAP_LABEL:		
		
		AC2 = AC2 + #1
		; r++
	}
	
	; reset catbuf, mincat
	mar(T2 + #1 )
	; T2 = cregions

	mar( AR1 - T2 )
	; AR1 -> catbuf
	||
	mar( AR4 - T2 )
	; AR4 -> mincat	
	
	; finished building the heaps
	;**********************************************************
	
	;------------------------------
	;Current assignments :
	;
	;	CDP		:  to assign to changes[minptr]  
	;
	;	XAR0	:  to assign to minbits
	;	XAR1 	: catbuf
	;	XAR2 	:  to assign to maxbits
	;	XAR4	: mincat (MAXCREGN)
	;	XAR5	: changes (MAXCATZNS2) ( to assign to changes[maxptr]  )
	;   XAR6 	: maxheap (HEAPSIZE)	( long[] )
	;	XAR7	: minheap (HEAPSIZE)	( long[] )
	;
	;	XAR3	: expbit	 
	; 
	; 	T0		:	
	; 	T1		:	
	; 	T2		:	cregions  
	;	T3		:   
	;
	; 	AC0		: expbits
	;   AC1		: 
	;   AC2		: 
	;   AC3		:  
	;
	;	XDP		:	locals
	; 
	;--------------------------------	
		
	
	AR0 = AC0
	; AR0 ( minbits ) = expbits
	||
	T0 = @2
	; T0 = ncatzns
	
	AR2 = AC0
	; AR2 (maxbits ) = expbits
	||
	mar( AR5 + T0)
	; XAR5(maxptr) -> changes[ncatzns]
	
	mar(T0 - #2)
	; T0 = ncatzns-2
	||		; TODO : Please verify the consitency of this parallelism
	BRC0 = T0
	; load trip count of ncatzns-2
	
	AC0 = @0 * #2
	; AC0 = availbits * 2

	mar(T1 = AR0)
	; T1 = minbits
	||		
	T1 = T1 + AR2
	; T1 = minbits + maxbits
	
	XCDP = XAR5
	; XCDP (minptr) -> changes[ncatzns]
	||	
	;------------------------------
	;Current assignments :
	;
	;	CDP		: changes[minptr]  
	;
	;	XAR0	: minbits
	;	XAR1 	: catbuf
	;	XAR2 	: maxbits
	;	XAR4	: mincat (MAXCREGN)
	;	XAR5	: changes[maxptr]  
	;   XAR6 	: maxheap (HEAPSIZE)	( long[] )
	;	XAR7	: minheap (HEAPSIZE)	( long[] )
	;
	;	XAR3	: expbit	 
	; 
	; 	T0		:  
	; 	T1		: minbits + maxbits
	; 	T2		:	  
	;	T3		:   
	;
	; 	AC0		: availbits*2
	;   AC1		: 
	;   AC2		: 
	;   AC3		:  
	;
	;	XDP		:	locals
	; 
	;--------------------------------	
			
	;**********************************************************
	; Derive the additional categories
	;**********************************************************
	; do ( ncatzn-1) times
	blockrepeat{
			
		TC1 = T1 <= AC0
		; if( minbits + maxbits <= availbits*2 ), set TC1
		||
		T3 = @3
		; T3 = nminheap
		AC3 = @4	
		if( !TC1 ) goto ELSE_LABEL  ; if( !( minbits + maxbits <= availbits*2 ) )  process the mincat array
		
		
		;AC3 = nmaxheap
				
		;------------------------------------------------
		; if( minbits + maxbits <= availbits*2 ) {
			
			AC1 = dbl(*AR7(#2))
			; AC1 = minheap[1]
	
			if( T3 == 0 ) goto BREAK_LABEL		
			; if (!nminheap) break
			||
			T0 = AC1		
			; T0 (r) = minheap[1] & 0xFFFF (lo part of..)
			
			*-AR5 = T0
			; changes[--maxptr] = r
			
		
			T2 = *AR1(T0)
			; T2 = catbuf[r]
		
			*AR1(T0) = *AR1(T0) - #1
			; catbuf[r] = catbuf[r] - 1
			||
			swap(T2,T0)
			; T2 -> r
			; T0 -> catbuf[r]

		    AR2 = AR2 - *AR3(T0)
		    ; maxbits = maxbits - RA_TNI_expbits_tab[catbuf[r]]
			||
			mar(T0 - #1)
			; T0 -> catbuf[r] - 1

			AR2 = AR2 + *AR3(T0)
		    ; maxbits = maxbits + RA_TNI_expbits_tab[catbuf[r]-1]
			||
			AC1 = #0x02
			
			AC1 = AC1 << #16
			; AC1 = 2 << 16
			||
			T1 = T3
			; T1 = nminheap
				
			AC2 = dbl(*AR7(#2) )
			; AC2 = minheap[1]
            nop
            nop;added to avoid silicon exception causing remark5505		
			AC1 = AC1 + AC2
			; AC1 =  2 << 16 + minheap[1]
			||
			if( T0 == #0 ) execute (AD_Unit)		; if( catbuf[r] == 0 )
				AC1 = dbl(*AR7(T1))				; 	AC1 = minheap[ nminheap] 
				||										
				mar( T3 - #2)					;		T3 = T3 - 2
			
			dbl(*AR7(#2) ) = AC1 			
			; minheap[1] =	minheap[1] + 2 << 16  OR minheap[ nminheap ] 
			; AC1 -> val
			
			@3 = T3		
			; update nminheap on stack
			||
			T2 = #2
			; T2 = k
			
			BRC1 = #0xFFF
			; max trip count in BRC1
			||	
			AC2 = T3
			; AC2 = nminheap
	
			AC2 = AC2 >> #1
			bit( AC2, @0) = #0
			; AC2 = PARENT(nminheap)
			||
			;------------------------------
			;Current assignments :
			;
			;	CDP		: changes[minptr]  
			;
			;	XAR0	: minbits
			;	XAR1 	: catbuf
			;	XAR2 	: maxbits
			;	XAR4	: mincat
			;	XAR5	: changes[--maxptr]  
			;   XAR6 	: maxheap (HEAPSIZE)	( long[] )
			;	XAR7	: minheap (HEAPSIZE)	( long[] )
			;
			;	XAR3	: expbit	 
			; 
			; 	T0		: child 
			; 	T1		: right
			; 	T2		:	 k 
			;	T3		: nminheap  
			;
			; 	AC0		: availbits*2
			;   AC1		: val
			;   AC2		: PARENT(nminheap)
			;   AC3		:  
			;
			;	XDP		:	locals
			; 
			;--------------------------------	
			; while(1) 		; almost
			blockrepeat{
					
				TC1 = T2 <= AC2
				; if( k <= PARENT(nminheap) ), set TC1
				||
				mar(T0 = T2)
				; T0 (child) = k
				
				
				if( !TC1 ) goto END_OF_MINHEAP_UPDATE_WHILE
				; if( ! k <= PARENT(nminheap) ) break
		
				T0 = T0 << #1
				; T0 = LCHILD(k)
				
				T1 = T0 + #2
				; T1 (right) = RCHILD(k)
			
				AC3 = dbl(*AR7(T0) )
				; AC3 = minheap[child]
				
				TC1 = T1 <= T3		
				; if( right <= nminheap ), set TC1
				||
				AC3 = dbl(*AR7(T1) ) - AC3
				; AC3 = minheap[right] - minheap[child]
				nop;added to avoid silicon exception causing remark 5505
				if(AC3 >= 0 ) execute (AD_Unit)		; if( minheap[right] >= minheap[child] )
				||	bit( ST0, #13) = #0				; 		clear TC1
					
				
				if( TC1 ) execute (AD_Unit)			; if( right <= nminheap && minheap[right] < minheap[child] )
				||	T0 = T1							;		child = right
				
				
				AC3 = dbl(*AR7(T0))
				; AC3 = minheap[child]
				||
				swap(T2,T0)
				; T0 -> k
				; T2 -> child
				
				TC1 = AC1 < AC3			
				; if ( val < minheapp[child] ) , set TC1
				||
				dbl(*AR7(T0)) = AC3
				; minheap[k] = minheap[child]
				
				if( TC1) goto END_OF_MINHEAP_UPDATE_WHILE
								;	||
								;	swap(T2, T0)
									; T0 -> child
									; T2 -> k
									
				; if the above branch is executed then
				;		T0 -> k
				
				; if the above branch is not executed then	
				; 		T2(k) -> child 			 ; for the next pass of the loop
				
			}
			
						
END_OF_MINHEAP_UPDATE_WHILE:
			
			;------------------------------
			;Current assignments :
			;
			;	CDP		: minptr  
			;
			;	XAR0	: minbits
			;	XAR1 	: catbuf
			;	XAR2 	: maxbits
			;	XAR4	: mincat
			;	XAR5	: maxptr
			;   XAR6 	: maxheap (HEAPSIZE)	( long[] )
			;	XAR7	: minheap (HEAPSIZE)	( long[] )
			;
			;	XAR3	: expbit	 
			; 
			; 	T0		: k
			; 	T1		:
			; 	T2		:
			;	T3		:
			;
			; 	AC0		: availbits*2
			;   AC1		: val
			;   AC2		:
			;   AC3		:  
			;
			;	XDP		:	locals
			; 
			;--------------------------------	
			
			
			dbl(*AR7(T0) ) = AC1
			; minheap[k] = val
			||
			goto BREAK_LABEL
			; skip over the else processing	

		; }
		;------------------------------------------------
		
ELSE_LABEL

		;------------------------------
		;Current assignments :
		;
		;	CDP		: changes[minptr]  
		;
		;	XAR0	: minbits
		;	XAR1 	: catbuf
		;	XAR2 	: maxbits
		;	XAR4	: mincat (MAXCREGN)
		;	XAR5	: changes[maxptr]  
		;   XAR6 	: maxheap (HEAPSIZE)	( long[] )
		;	XAR7	: minheap (HEAPSIZE)	( long[] )
		;
		;	XAR3	: expbit	 
		; 
		; 	T0		:  
		; 	T1		: 
		; 	T2		:	  
		;	T3		:   
		;
		; 	AC0		: availbits*2
		;   AC1		: 
		;   AC2		: 
		;   AC3		: nmaxheap
		;
		;	XDP		:	locals
		; 
		;--------------------------------	
		;------------------------------------------------
		; if( minbits + maxbits > availbits*2 ) {
		
			AC1 = dbl(*AR6(#2))
			; AC1 = maxheap[1]
	
			if( AC3 == 0 ) goto BREAK_LABEL		
			; if (!nmaxheap) break
			||
			T0 = AC1		
			; T0 (r) = maxheap[1] & 0xFFFF (lo part of..)
			
			*CDP+ = T0
			; changes[minptr++] = r
			||
			T3 = AC3
			; T3 = nmaxheap
		
			T2 = *AR4(T0)
			; T2 = mincat[r]
		
			*AR4(T0) = *AR4(T0) + #1
			; mincat[r] = mincat[r] + 1
			||
			T0 = T2
			; T0 -> mincat[r]

		    AR0 = AR0 - *AR3(T0)
		    ; minbits = minbits - RA_TNI_expbits_tab[mincat[r]]
			||
			mar(T0 + #1)
			; T0 -> mincat[r] + 1

			AR0 = AR0 + *AR3(T0)
		    ; minbits = minbits + RA_TNI_expbits_tab[mincat[r]+1]
			||
			AC2 = #0x02
			AC2 = AC2 << #16
			; AC2 = 2 << 16
			||
			T1 = T3
			; T1 = nmaxheap
		
			AC1 = dbl(*AR6(#2) )
			; AC1 = maxheap[1]
			
			T0 = T0 - #7
			; T0 = mincat[r] - 7
			nop;added to avoid silicon exception causing remark 5505	
			AC1 = AC1 - AC2
			; AC1 =   maxheap[1] - 2 << 16 
			||
			if( T0 == #0 ) execute (AD_Unit)		; if( mincat[r] == 7 )
				AC1 = dbl(*AR6(T1))				; 	AC1 = maxheap[ nmaxheap ] 
				||										
				mar( T3 - #2)					;	T3 = 	nmaxheap--
			
			dbl(*AR6(#2) ) = AC1 			
			; maxheap[1] =	maxheap[1] - 2 << 16  OR maxheap[ nmaxheap ] 
			; AC1 -> val
			
			@4 = T3		
			; update nmaxheap on stack
			||
			T2 = #2
			; T2 = k
			
			BRC1 = #0xFFF
			; max trip count in BRC1
			||	
			AC2 = T3
			; AC2 = nmaxheap
	
			AC2 = AC2 >> #1
			bit( AC2, @0) = #0
			; AC2 = PARENT(nmaxheap)
			||
			;------------------------------
			;Current assignments :
			;
			;	CDP		: changes[minptr]  
			;
			;	XAR0	: minbits
			;	XAR1 	: catbuf
			;	XAR2 	: maxbits
			;	XAR4	: mincat
			;	XAR5	: changes[--maxptr]  
			;   XAR6 	: maxheap (HEAPSIZE)	( long[] )
			;	XAR7	: minheap (HEAPSIZE)	( long[] )
			;
			;	XAR3	: expbit	 
			; 
			; 	T0		: child 
			; 	T1		: right
			; 	T2		:	 k 
			;	T3		: nmaxheap  
			;
			; 	AC0		: availbits*2
			;   AC1		: val
			;   AC2		: PARENT(nmaxheap)
			;   AC3		:  
			;
			;	XDP		:	locals
			; 
			;--------------------------------	
			; while(1) 		; almost
			blockrepeat{
					
				TC1 = T2 <= AC2
				; if( k <= PARENT(nminheap) ), set TC1
				||
				mar(T0 = T2)
				; T0 (child) = k
				
				
				if( !TC1 ) goto END_OF_MAXHEAP_UPDATE_WHILE
				; if( ! k <= PARENT(nminheap) ) break
	
				T0 = T0 << #1
				; T0 = LCHILD(k)
				
				T1 = T0 + #2
				; T1 (right) = RCHILD(k)
			
				AC3 = dbl(*AR6(T0) )
				; AC3 = maxheap[child]
				
				TC1 = T1 <= T3		
				; if( right <= nmaxheap ), set TC1
				||
				AC3 = dbl(*AR6(T1) ) - AC3
				; AC3 = maxheap[right] - maxheap[child]
				nop
				if(AC3 <= 0 ) execute (AD_Unit)		; if( maxheap[right] <= maxheap[child] )
				||	bit( ST0, #13) = #0				; 		clear TC1
					
				
				if( TC1 ) execute (AD_Unit)			; if( right <= nminheap && maxheap[right] > maxheap[child] )
				||	T0 = T1							;		child = right
				
				
				AC3 = dbl(*AR6(T0))
				; AC3 = maxheap[child]
				||
				swap(T2,T0)
				; T0 -> k
				; T2 -> child
				
				TC1 = AC1 > AC3			
				; if ( val > maxheap[child] ) , set TC1
				||
				dbl(*AR6(T0)) = AC3
				; maxheap[k] = maxheap[child]
				
				if( TC1) goto END_OF_MAXHEAP_UPDATE_WHILE
								;	||
								;	swap(T2, T0)
									; T0 -> child
									; T2 -> k
									
				; if the above branch is executed then
				;		T0 -> k
				
				; if the above branch is not executed then	
				; 		T2(k) -> child 			 ; for the next pass of the loop
				
			}
			
						
END_OF_MAXHEAP_UPDATE_WHILE:
			
			;------------------------------
			;Current assignments :
			;
			;	CDP		: minptr
			;
			;	XAR0	: minbits
			;	XAR1 	: catbuf
			;	XAR2 	: maxbits
			;	XAR4	: mincat
			;	XAR5	: maxptr
			;   XAR6 	: maxheap (HEAPSIZE)	( long[] )
			;	XAR7	: minheap (HEAPSIZE)	( long[] )
			;
			;	XAR3	: expbit	 
			; 
			; 	T0		: k
			; 	T1		:
			; 	T2		:
			;	T3		:
			;
			; 	AC0		: availbits*2
			;   AC1		: val
			;   AC2		:
			;   AC3		:  
			;
			;	XDP		:	locals
			; 
			;--------------------------------	
			
			
			dbl(*AR6(T0) ) = AC1
			; maxheap[k] = val
		
		
	


		; }
		;------------------------------------------------		
		; calculate minbits + maxbits for next  iteration
		
	
BREAK_LABEL:
		mar(T1 = AR0)
		; T1 = minbits
		||		
		T1 = T1 + AR2
		; T1 = minbits + maxbits
	
	
	}
	; finished categorization 
	;**********************************************************
	
	;**********************************************************
	; 	calculate the selected categorization
	;**********************************************************
	
	
	T2 = @1 
	; T2 = rate_code
	if(T2 == 0 ) goto END_CATEGORIZE_LABEL
	
	mar(T2-#1)
	; T2 = rate_code - 1
	||				; Please verify this //ism again
	BRC0 = T2
	; load trip count of rate_code  -1 
	
	; do rate_code times
	localrepeat{
	
		T0 = *AR5+;
		; T0 = changes[maxptr++]
		
		*AR1(T0) = *AR1(T0) + #1
		; catbuf[ changes[maxptr] ] =	catbuf[ changes[maxptr] ] + 1
								
	}
	
	; catbuf contains the desired categorization
	;**********************************************************
	
	
	;**********************************************************************************************	
	;----------------------------------------------------------------------
	; epilog code
END_CATEGORIZE_LABEL:	
	
	
	XCDP = popboth()
	XAR7 = popboth()
	||
	bit(ST1, ST1_CPL ) = #1
	; restore cpl mode
	.cpl_on
	
	XAR6 = popboth()
	XAR5 = popboth()
	;restore XCDP, XAR7, XAR6 and XAR5
	
	T2, T3 = pop()
	; restore
	||
	bit(ST2, #15) = #1
	;set the ARMS bit to use control Mode addressing
	.ARMS_on
    .remark 5573
    .remark 5673
    .remark 5549
    .remark 5505
    .remark 5571	
	return 

; _RA_TNI_Categorize
;----------------------------------------------------------------------------------------------


	.endif	;	CATEGORIZE_ASM
	
	
	
;------------------------------------------------------------------------------------------------

;dead code
