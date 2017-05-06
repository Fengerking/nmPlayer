;************************************************************************
;									                                    *
;	VisualOn, Inc. Confidential and Proprietary, 2005		            *
;								 	                                    *
;***********************************************************************/

;R0 src
;R1 dst
;R2 srcpitch
;R3 dstpitch

	AREA	|.text|, CODE

  macro 
  PldA $ARM5,$Pos
    if $ARM5>0
	  if $Pos >= 0
  	    if $Pos > 0
	      pld [r0,r2,lsl #1]
	      add r0,r0,#8
	      pld [r0,r2,lsl #1]
	      sub r0,r0,#8
	    else
	      pld [r0,r2,lsl #1]
	      add r0,r0,#4
	      pld [r0,r2,lsl #1]
	      sub r0,r0,#4
	    endif
	  else
	    pld [r0,r2,lsl #1]

	    add r0,r0,#7
	    pld [r0,r2,lsl #1]
	    sub r0,r0,#7
	  endif
	endif
  mend

  macro 
  PldBI $ARM5,$Pos
    if $ARM5>0
	  if $Pos >= 0
  	    if $Pos > 0
	      add r10,r2,r2
	      add r10,r10,#8
	    else
	      add r10,r2,r2
	      add r10,r10,#4
	    endif
	  else
	      add r10,r2,r2
	      add r10,r10,#7
	  endif
	endif
  mend

  macro 
  PldB $ARM5,$Pos
	if $ARM5>0
      pld [r0,r2,lsl #1]
      pld [r0,r10]
	endif
  mend

  macro 
  GenCodeCpy $Name,$Sub,$Round,$Add,$Fast,$ARM5,$ARMv6, $ARM11

	align 16
	export $Name
$Name proc
	stmdb	sp!, {r4 - r12, lr}
	if $Fast>0	  
	if $ARM11>0
	$Sub	$Name.0_v11,0,$Round,$Add,0
	else
	movs	r4,r0,lsl #30
	beq		$Name.LAB4
	cmps	r4,#0x80000000
	beq		$Name.LAB2
	bhi		$Name.LAB3
$Name.LAB1
	bic		r0,r0,#3
	$Sub	$Name.8,8,$Round,$Add,$ARMv6
$Name.LAB2	
	bic		r0,r0,#3
	$Sub	$Name.16,16,$Round,$Add,$ARMv6
$Name.LAB3	
	bic		r0,r0,#3
	$Sub	$Name.24,24,$Round,$Add,$ARMv6
$Name.LAB4	
	$Sub	$Name.0,0,$Round,$Add,$ARMv6
	endif
	else
	$Sub	$Name.s,-1,$Round,$Add,$ARMv6
	endif
	endp
  mend

;------------------------------------------
;CInter4X8
;------------------------------------------

  macro
  CpyBlkRWmmx $ARMv6,$Pos
	if $Pos > 0

	  ldr	r6,[r0,#4]
	  ldr	r4,[r0],r2
		
	  mov	r4,r4,lsr #$Pos
	  orr	r4,r4,r6,lsl #32-$Pos

	  str	r4,[r1],r3
	else
		  ldr	r4,[r0],r2
		  str	r4,[r1],r3
	endif
  mend

  macro
  CInter4X8 $Id, $Pos, $Round, $Add, $ARMv6

	ldr		r12, [sp, #40]
$Id.loop_again
	pld [r0,r2]
	CpyBlkRWmmx $ARMv6,$Pos 
	subs	r12, r12, #1
	bne		$Id.loop_again


;	PldBI $ARM5,$Pos
;	PldB $ARM5,$Pos
;	CpyBlkRArm $Pos 
;	PldB $ARM5,$Pos
;	CpyBlkRArm $Pos 
;	PldB $ARM5,$Pos
;	CpyBlkRArm $Pos 
;	PldB $ARM5,$Pos
;	CpyBlkRArm $Pos 
;	PldB $ARM5,$Pos
;	CpyBlkRArm $Pos 
;	PldB $ARM5,$Pos
;	CpyBlkRArm $Pos 
;	PldB $ARM5,$Pos
;	CpyBlkRArm $Pos 
;	PldB $ARM5,$Pos
;	CpyBlkRArm $Pos 

	ldmia	sp!, {r4 - r12, pc}
  mend

;------------------------------------------
;AddBlock4X8 
;------------------------------------------

  macro
  wmmx_addblkRow $Pos

	if $Pos < 0

	ldrb	r4,[r0]
	ldrb	r6,[r0,#1]
	orr	r4,r4,r6,lsl #8
	ldrb	r6,[r0,#2]
	orr	r4,r4,r6,lsl #16
	ldrb	r6,[r0,#3]
	orr	r4,r4,r6,lsl #24
	add	r0,r0,r2

	ldrb	r5,[r0]
	ldrb	r6,[r0,#1]
	orr	r5,r5,r6,lsl #8
	ldrb	r6,[r0,#2]
	orr	r5,r5,r6,lsl #16
	ldrb	r6,[r0,#3]
	orr	r5,r5,r6,lsl #24
	add	r0,r0,r2

	else
	if $Pos > 0
	  ldr	r6,[r0,#4]
	  ldr	r4,[r0],r2		
	  mov	r4,r4,lsr #$Pos
	  orr	r4,r4,r6,lsl #32-$Pos

	  ldr	r6,[r0,#4]
	  ldr	r5,[r0],r2		
	  mov	r5,r5,lsr #$Pos
	  orr	r5,r5,r6,lsl #32-$Pos
	else
	  ldr	r4,[r0],r2
	  ldr	r5,[r0],r2
	endif
	endif

	ldr	r6,[r1]
	add	r8,r1,r3
	ldr	r7,[r8]
	tmcrr	wr0,r4,r5
	tmcrr	wr5,r6,r7
	wavg2br wr0,wr0,wr5
	tmrrc	r6,r7,wr0

	str	r6,[r1],r3
	str	r7,[r1],r3
  mend

  macro
  AddBlock4X8  $Id, $Pos, $Round, $Add, $ARM5

;	PldBI $ARM5,$Pos

	ldr		r11, [sp, #40]

;	mov		r11,#8

$Id.LABLOOP
	pld [r0,r2]
	wmmx_addblkRow $Pos
	subs r11,r11,#2 
	bne  $Id.LABLOOP

	ldmia	sp!, {r4 - r12, pc}
  mend

;------------------------------------------
; CInter4X8H
;------------------------------------------

  macro
  LoadHorRow $Id, $Pos

    ; result is r4,r5 and r8,r9 (one pixel to the right)
    ; r6,r7 can be used

	if $Pos < 0

	ldrb	r4,[r0]
	ldrb	r6,[r0,#1]
	ldrb	r5,[r0,#4]
	orr	r4,r4,r6,lsl #8
	ldrb	r6,[r0,#2]
	orr	r4,r4,r6,lsl #16
	ldrb	r6,[r0,#3]
	orr	r4,r4,r6,lsl #24

	mov	r8,r4,lsr #8
	orr	r8,r8,r5,lsl #24

	add	r0,r0,r2

	ldrb	r6,[r0]
	ldrb	r9,[r0,#1]
	ldrb	r7,[r0,#4]
	orr	r6,r6,r9,lsl #8
	ldrb	r9,[r0,#2]
	orr	r6,r6,r9,lsl #16
	ldrb	r9,[r0,#3]
	orr	r6,r6,r9,lsl #24

	mov	r9,r6,lsr #8
	orr	r9,r9,r7,lsl #24

	add	r0,r0,r2

	else

    	ldr	r5,[r0,#4]
    	ldr	r4,[r0],r2

    	ldr	r7,[r0,#4]
    	ldr	r6,[r0],r2

    	if $Pos+8 < 32
	  mov	r8,r4,lsr #$Pos+8
	  orr	r8,r8,r5,lsl #32-$Pos-8

	  mov	r9,r6,lsr #$Pos+8
	  orr	r9,r9,r7,lsl #32-$Pos-8
    	else
	  mov	r8,r5
	  mov	r9,r7
    	endif

	if $Pos > 0
	  mov	r4,r4,lsr #$Pos
	  orr	r4,r4,r5,lsl #32-$Pos

	  mov	r6,r6,lsr #$Pos
	  orr	r6,r6,r7,lsl #32-$Pos
	endif
	endif
  mend

  macro
  CopyHorRow $Id, $Pos, $Round, $Add

;r14 01010101
;r12 7f7f7f7f

	LoadHorRow	$Id,$Pos

	tmcrr	wr1,r4,r6
	tmcrr	wr2,r8,r9

	if $Round>0
	  wavg2b wr0,wr1,wr2
	else
	  wavg2br wr0,wr1,wr2
	endif

	if $Add>0
	  ldr	r6,[r1]
	  add	r5,r1,r3
	  ldr	r7,[r5]
	  tmcrr	wr5,r6,r7
	  wavg2br wr0,wr0,wr5
	  tmrrc	r6,r7,wr0	  
	  
	  str	r6,[r1],r3
	  str	r7,[r1],r3
	else
	  tmrrc	r6,r7,wr0
	  str	r6,[r1],r3
	  str	r7,[r1],r3
	endif
	
  mend

  macro
  CInter4X8H $Id, $Pos, $Round, $Add, $ARM5

;	PldBI $ARM5,$Pos

	ldr	r11, [sp, #40]

;	mov		r11,#8

$Id.LABLOOP
	pld [r0,r2]
	CopyHorRow $Id,$Pos,$Round,$Add
	subs r11,r11,#2
	bne  $Id.LABLOOP

	ldmia	sp!, {r4 - r12, pc}
  mend

;------------------------------------------
; CInter4X8V
;------------------------------------------

  macro
  LoadVerRow $Id, $Pos, $Parity
  if $Parity>0
    ; result is r8,r9 (r10=r8>>1,r11=r9>>1) 
    ; r10,r11 can be used

	if $Pos < 0
 	  ldrb	r8,[r0]
	  ldrb	r10,[r0,#1]
	  orr	r8,r8,r10,lsl #8 
	  ldrb	r10,[r0,#2]
	  orr	r8,r8,r10,lsl #16
	  ldrb	r10,[r0,#3]
	  orr	r8,r8,r10,lsl #24
	  add	r0,r0,r2
	else
    if $Pos > 0
	  ldr	r10,[r0,#4]
	  ldr	r8,[r0],r2

	  mov	r8,r8,lsr #$Pos
	  orr	r8,r8,r10,lsl #32-$Pos
    else
	  ldr	r8,[r0],r2
    endif
	endif
	and		r10,r12,r8,lsr #1
  else
    ; result is r4,r5 (r6=r4>>1,r7=r5>>1) 
    ; r6,r7 can be used

	if $Pos < 0
 	  ldrb	r4,[r0]
	  ldrb	r6,[r0,#1]
	  orr	r4,r4,r6,lsl #8
	  ldrb	r6,[r0,#2]
	  orr	r4,r4,r6,lsl #16
	  ldrb	r6,[r0,#3]
	  orr	r4,r4,r6,lsl #24
	  add	r0,r0,r2
	else
    if $Pos > 0
	  ldr	r6,[r0,#4]
	  ldr	r4,[r0],r2

	  mov	r4,r4,lsr #$Pos
	  orr	r4,r4,r6,lsl #32-$Pos
    else
	  ldr	r4,[r0],r2
    endif
	endif
	
	and		r6,r12,r4,lsr #1
  endif
  mend

  macro
  CopyVerRow $Id, $Pos, $Parity, $Round, $Add

;r14 01010101
;r12 7f7f7f7f

	LoadVerRow $Id,$Pos,$Parity

    if $Parity>0
	  if $Round>0
	    and	r4,r4,r8
	  else
	    orr	r4,r4,r8
	  endif
	  and	r4,r4,r14

	  add	r4,r4,r6

  	  add	r4,r4,r10

	  if $Add>0
	    ldr	r6,[r1]
	    and	r3,r12,r4,lsr #1
	    orr r4,r6,r4
	    and	r6,r12,r6,lsr #1
	    add r6,r6,r3
	    and r4,r4,r14
	    add r4,r4,r6
	    ldr	r3,[sp, #4]
	    ldr	r7,[sp]		;end src for loop compare
	    str	r4,[r1],r3
	  else
	    ldr	r7,[sp]		;end src for loop compare
	    str	r4,[r1],r3
	  endif
	else
	  if $Round>0
	    and	r8,r8,r4
	  else
	    orr	r8,r8,r4
	  endif
	  and	r8,r8,r14

	  add	r8,r8,r10
  	  add	r8,r8,r6

	  if $Add>0
	    ldr	r10,[r1]
	    and	r3,r12,r8,lsr #1
	    orr r8,r10,r8
	    and	r10,r12,r10,lsr #1
	    add r10,r10,r3
	    and r8,r8,r14
	    ldr	r3,[sp, #4]		
	    add r10,r10,r8
	    str	r10,[r1],r3
	  else
	    str	r8,[r1],r3
	  endif
	endif
  mend


  macro
  CInter4X8V $Id, $Pos, $Round, $Add, $ARM5



	ldr		r4, [sp, #40]
	sub		sp,sp,#8
	mul		r4, r2, r4
	ldr		r14,$Id.LABM
	add		r4, r4, r0

;	add		r4,r0,r2,lsl #3
	add		r4,r4,r2
	str		r4,[sp]		;end src
	str		r3,[sp, #4]	;end src	


	mvn		r12,r14,lsl #7

	pld [r0,r2]
	LoadVerRow $Id,$Pos,1
$Id.LABLOOP
	pld [r0,r2]
	CopyVerRow $Id,$Pos,0,$Round,$Add
	pld [r0,r2]
	CopyVerRow $Id,$Pos,1,$Round,$Add

	cmp		r0,r7
	bne		$Id.LABLOOP
	add		sp,sp,#8
	ldmia	sp!, {r4 - r12, pc}
$Id.LABM dcd 0x01010101
  mend

;------------------------------------------
; CInter4X8HV
;------------------------------------------

; load needs r2,r3 for temporary (r2 is restored from stack)

  macro
  LoadHorVerRow $Id, $Pos, $Parity
  if $Parity>0

	;read result r4,r5 and r2,r3 (one pixel to right)
	;r6,r7 can be used

    if $Pos<0

	ldrb	r4,[r0]
	ldrb	r6,[r0,#1]
	ldrb	r5,[r0,#4]
	orr	r4,r4,r6,lsl #8
	ldrb	r6,[r0,#2]
	orr	r4,r4,r6,lsl #16
	ldrb	r6,[r0,#3]
	orr	r4,r4,r6,lsl #24
	add	r0,r0,r2

	mov		r2,r4,lsr #8
	orr		r2,r2,r5,lsl #24

	else
    ldr		r5,[r0,#4]
    ldr		r4,[r0],r2

    if $Pos+8 < 32
	  mov	r2,r4,lsr #$Pos+8
	  orr	r2,r2,r5,lsl #32-$Pos-8
    else
	  mov	r2,r5
    endif

	if $Pos > 0
	  mov	r4,r4,lsr #$Pos
	  orr	r4,r4,r5,lsl #32-$Pos
	endif
	endif

	and		r6,r2,r14
	and		r2,r12,r2,lsr #2
	and		r7,r4,r14
	and		r4,r12,r4,lsr #2
	add		r4,r4,r2
	add		r6,r6,r7

  else
	;read result r8,r9 and r2,r3 (one pixel to right)
	;r10,r11 can be used

    if $Pos<0

	ldrb	r8,[r0]
	ldrb	r10,[r0,#1]
	ldrb	r9,[r0,#4]
	orr	r8,r8,r10,lsl #8
	ldrb	r10,[r0,#2]
	orr	r8,r8,r10,lsl #16
	ldrb	r10,[r0,#3]
	orr	r8,r8,r10,lsl #24
	add	r0,r0,r2
	
	mov		r2,r8,lsr #8
	orr		r2,r2,r9,lsl #24

	else
    ldr		r9,[r0,#4]
    ldr		r8,[r0],r2

    if $Pos+8 < 32
	  mov	r2,r8,lsr #$Pos+8
	  orr	r2,r2,r9,lsl #32-$Pos-8
    else
	  mov	r2,r9
    endif

	if $Pos > 0
	  mov	r8,r8,lsr #$Pos
	  orr	r8,r8,r9,lsl #32-$Pos
	endif
	endif

	and		r10,r2,r14
	and		r2,r12,r2,lsr #2
	and		r11,r8,r14
	and		r8,r12,r8,lsr #2
	add		r8,r8,r2
	add		r10,r10,r11
  endif
	ldr		r2,[sp]
  mend

  macro
  CopyHorVerRow $Id, $Pos, $Parity, $Round, $Add

;r14 03030303
;r12 3f3f3f3f

	LoadHorVerRow $Id,$Pos,$Parity

	if $Round>0
	  and r3,r14,r14,lsr #1		;0x01010101
	else
	  and r3,r14,r14,lsl #1		;0x02020202
	endif
    if $Parity>0
	  add	r8,r8,r4
	  add	r10,r10,r6

	  add	r10,r10,r3
	  and	r10,r14,r10,lsr #2

	  if $Add>0
	    add	r8,r8,r10 
	    orr	r12,r12,r12,lsl #1  ;0x7F7F7F7F
	    ldr	r10,[r1]
	    and	r3,r12,r8,lsr #1
	    orr r8,r10,r8
	    and	r10,r12,r10,lsr #1
	    add r10,r10,r3
	    and r3,r14,r14,lsr #1 ;0x01010101
	    mvn	r12,r14,lsl #6    ;restore r12
	    and r8,r8,r3	
	    ldr	r11,[sp,#8]  ;dstpitch	    	
	    add r10,r10,r8
	    ldr	r3,[sp,#4]	;end src for loop compare
	    str	r10,[r1],r11
	  else
	    add	r8,r8,r10
	    ldr	r10,[sp,#8]  ;dstpitch
	    ldr	r3,[sp,#4]	;end src for loop compare
	    str	r8,[r1],r10
	  endif
	else
	  add	r4,r4,r8
	  add	r6,r6,r10


	  add	r6,r6,r3
	  and	r6,r14,r6,lsr #2

	  if $Add>0
	    add	r4,r4,r6
	    orr	r12,r12,r12,lsl #1  ;0x7F7F7F7F
	    ldr	r6,[r1]
	    and	r3,r12,r4,lsr #1
	    orr r4,r6,r4
	    and	r6,r12,r6,lsr #1
	    add r6,r6,r3
	    and r3,r14,r14,lsr #1 ;0x01010101
	    mvn	r12,r14,lsl #6    ;restore r12
	    and r4,r4,r3
	    ldr	r3,[sp,#8]  ;dstpitch		
	    add r6,r6,r4
	    str	r6,[r1],r3
	  else
	    ldr	r3,[sp,#8]  ;dstpitch
	    add	r4,r4,r6
	    str	r4,[r1],r3
	  endif
	endif
  mend

  macro
  CInter4X8HV $Id, $Pos, $Round, $Add, $ARM5


	ldr		r4, [sp, #40]
	sub		sp,sp,#12
	mul		r4, r2, r4
	str		r3,[sp,#8]	;dstpitch
	add		r4, r4, r0

;	add		r4,r0,r2,lsl #3
	add		r4,r4,r2
	str		r2,[sp]		;srcpitch
	str		r4,[sp,#4]	;end src


	ldr		r14,$Id.LABM
	mvn		r12,r14,lsl #6
	pld [r0,r2]
	LoadHorVerRow $Id,$Pos,1
$Id.LABLOOP
	pld [r0,r2]
	CopyHorVerRow $Id,$Pos,0,$Round,$Add
	pld [r0,r2]
	CopyHorVerRow $Id,$Pos,1,$Round,$Add
	cmp		r0,r3
	bne		$Id.LABLOOP

	add		sp,sp,#12
	ldmia	sp!, {r4 - r12, pc}
$Id.LABM dcd 0x03030303
  mend

;---------------------------------------------------
; smaller versions without PldA
;
;	GenCodeCpy CInter4X8,	wmmx4x8_copyblk,	0,	0,	1,	1,	0,	0
;	GenCodeCpy CInter4X8V,	wmmx4x8_copyblkv,	0,	0,	1,	1,	0,	0
;	GenCodeCpy CInter4X8H,	wmmx4x8_copyblkh,	0,	0,	1,	1,	0,	0
;	GenCodeCpy CInter4X8HV,	wmmx4x8_copyblkhv,	0,	0,	1,	1,	0,	0
;                                                                  	  	  	  	
;	GenCodeCpy AddBlock4X8,	wmmx4x8_addblk ,	0,	1,	1,	1,	0,	0
;	GenCodeCpy CInter4X8V,	wmmx4x8_addblkv,	0,	1,	1,	1,	0,	0
;	GenCodeCpy CInter4X8H,	wmmx4x8_addblkh,	0,	1,	1,	1,	0,	0
;	GenCodeCpy CInter4X8HV,	wmmx4x8_addblkhv,	0,	1,	1,	1,	0,	0
;
;---------------------------------------------------
;--------------------------------------------------------------------------------------------
;   GenCodeCpy		$Name,			$Sub,		$Round,	$Add,	$Fast,	$ARM5,	$ARMv6,	$ARM11
;
;	GenCodeCpy wmmx4x8_copyblk,	CInter4X8,	0,	0,	1,	1,	0,	0
;	GenCodeCpy wmmx4x8_copyblkv,	CInter4X8V,	0,	0,	1,	1,	0,	0
;	GenCodeCpy wmmx4x8_copyblkh,	CInter4X8H,	0,	0,	1,	1,	0,	0
;	GenCodeCpy wmmx4x8_copyblkhv,	CInter4X8HV,	0,	0,	1,	1,	0,	0
;                                                                  	  	  	  	
;	GenCodeCpy wmmx4x8_addblk,	AddBlock4X8,	0,	1,	1,	1,	0,	0
;	GenCodeCpy wmmx4x8_addblkv,	CInter4X8V,	0,	1,	1,	1,	0,	0
;	GenCodeCpy wmmx4x8_addblkh,	CInter4X8H,	0,	1,	1,	1,	0,	0
;	GenCodeCpy wmmx4x8_addblkhv,	CInter4X8HV,	0,	1,	1,	1,	0,	0
;-------------------------------------------------------------------------------------------------
;   GenCodeCpy		$Name,			$Sub,		$Round,	$Add,	$Fast,	$ARM5,	$ARMv6,	$ARM11

	GenCodeCpy __voMPEG2D0196,	CInter4X8,	0,	0,	1,	1,	0,	0
	GenCodeCpy __voMPEG2D0197,	CInter4X8V,	0,	0,	1,	1,	0,	0
	GenCodeCpy __voMPEG2D0198,	CInter4X8H,	0,	0,	1,	1,	0,	0
	GenCodeCpy __voMPEG2D0199,	CInter4X8HV,	0,	0,	1,	1,	0,	0
                                                                  	  	  	  	
	GenCodeCpy __voMPEG2D0200,	AddBlock4X8,	0,	1,	1,	1,	0,	0
	GenCodeCpy __voMPEG2D0201,	CInter4X8V,	0,	1,	1,	1,	0,	0
	GenCodeCpy __voMPEG2D0202,	CInter4X8H,	0,	1,	1,	1,	0,	0
	GenCodeCpy __voMPEG2D0203,	CInter4X8HV,	0,	1,	1,	1,	0,	0
	END
