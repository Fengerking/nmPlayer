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
;ArmInter4x8
;------------------------------------------

  macro
  CpyBlkRArm $ARMv6,$Pos
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
  ArmInter4x8 $Id, $Pos, $Round, $Add, $ARMv6

	ldr		r12, [sp, #40]
$Id.loop_again
	pld [r0,r2]
	CpyBlkRArm $ARMv6,$Pos 
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
;CpyBlkMArm
;------------------------------------------

  macro
  CpyBlkMRArm $ARMv6,$Pos
	if $Pos > 0

	  ldr	r14,[r0,#16]
	  ldr	r6,[r0,#12]
	  ldr	r12,[r0,#8]
		
	  mov	r14,r14,lsl #32-$Pos
	  mov	r12,r12,lsr #$Pos
	  orr	r14,r14,r6,lsr #$Pos
	  orr	r12,r12,r6,lsl #32-$Pos

	  ldr	r5,[r0,#8]
	  ldr	r6,[r0,#4]
	  ldr	r4,[r0],r2
		
	  mov	r5,r5,lsl #32-$Pos
	  mov	r4,r4,lsr #$Pos
	  orr	r5,r5,r6,lsr #$Pos
	  orr	r4,r4,r6,lsl #32-$Pos
	else
	if $ARMv6 > 0
	ldrd	r6,[r0,#8]
	ldrd	r4,[r0],r2
	else
	  ldr	r5,[r0,#4]
	  ldr	r12,[r0,#8]
	  ldr	r14,[r0,#12]
	  ldr	r4,[r0],r2
	endif
	endif
	if $ARMv6 > 0
	strd	r6,[r1,#8]
	strd	r4,[r1],r3
	else
	str		r5,[r1,#4]
	str		r12,[r1,#8]
	str		r14,[r1,#12]
	str		r4,[r1],r3
	endif
  mend

  macro
  CpyBlkMArm $Id, $Pos, $Round, $Add, $ARMv6

;	PldBI $ARM5,$Pos

	ldr		r11, [sp, #40]

;	mov		r11,#16
$Id.LABLOOP
	pld [r0,r2]
	CpyBlkMRArm $ARMv6,$Pos
	subs r11,r11,#1
	bne  $Id.LABLOOP

	ldmia	sp!, {r4 - r12, pc}
  mend

;------------------------------------------
;ArmCopy8x16: no aligment!, only used in Copy()
;------------------------------------------

  macro
  arm_copyblk8x16Row $ARMv6
    if $ARMv6>0
		ldrd	r6,[r0], r2
		strd	r6,[r1], r3
	else
		ldr		r5,[r0,#4]
		ldr		r4,[r0],r2
		str		r5,[r1,#4]
		str		r4,[r1],r3
	endif

  mend

  macro
  ArmCopy8x16 $Id, $Pos, $Round, $Add, $ARMv6


	ldr		r11, [sp, #40]

;	mov		r11,#15

$Id.LABLOOP
	pld [r0,r2]
	arm_copyblk8x16Row $ARMv6
	subs r11,r11,#1 
	bne  $Id.LABLOOP

	;unroll last (no PldA needed)
	arm_copyblk8x16Row $ARMv6
	ldmia	sp!, {r4 - r12, pc}
  mend

;------------------------------------------
;ArmCopy4x8: no aligment!, only used in Copy()
;------------------------------------------

  macro
  ArmCopy4x8 $Id, $Pos, $Round, $Add, $ARMv6

	mov		r11,#3

$Id.LABLOOP
	ldr		r6,[r0],r2
	ldr		r4,[r0],r2
	str		r6,[r1],r3
	str		r4,[r1],r3
	subs		r11,r11,#1 
	bne		$Id.LABLOOP

	;unroll last (no PldA needed)
	ldr		r6,[r0],r2
	ldr		r4,[r0],r2
	str		r6,[r1],r3
	str		r4,[r1],r3

	ldmia	sp!, {r4 - r12, pc}
  mend

;------------------------------------------
;ArmInter2_4x8 
;------------------------------------------

  macro
  arm_addblkRow $Pos, $ARMv6

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
	  if $ARMv6 > 0
		ldr		r6,[r1]  
		uqadd8  r4, r4, r14
		uhadd8	r6, r6, r4
		str		r6,[r1], r3 
	  else
		ldr		r6,[r1]
		and		r8,r12,r4,lsr #1
		orr		r4,r6,r4
		and		r6,r12,r6,lsr #1
		add		r6,r6,r8
		and		r4,r4,r14
		add		r6,r6,r4
		str		r6,[r1],r3
	  endif
  mend

  macro
  ArmInter2_4x8  $Id, $Pos, $Round, $Add, $ARMv6

;	PldBI $ARM5,$Pos
	ldr		r14,$Id.LABM

	ldr		r11, [sp, #40]

;	mov		r11,#8
	mvn		r12,r14,lsl #7

$Id.LABLOOP
	pld [r0,r2]
	arm_addblkRow $Pos, $ARMv6
	subs r11,r11,#1 
	bne  $Id.LABLOOP

	ldmia	sp!, {r4 - r12, pc}
$Id.LABM dcd 0x01010101
  mend

;------------------------------------------
; ArmInter4x8H
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

	mov		r8,r4,lsr #8
	orr		r8,r8,r5,lsl #24

	add		r0,r0,r2

	else

    ldr		r5,[r0,#4]
    ldr		r4,[r0],r2

    if $Pos+8 < 32
	  mov	r8,r4,lsr #$Pos+8
	  orr	r8,r8,r5,lsl #32-$Pos-8
    else
	  mov	r8,r5
    endif

	if $Pos > 0
	  mov	r4,r4,lsr #$Pos
	  orr	r4,r4,r5,lsl #32-$Pos
	endif
	endif
  mend

  macro
  CopyHorRow $Id, $Pos, $Round, $Add, $ARMv6

;r14 01010101
;r12 7f7f7f7f

	LoadHorRow	$Id,$Pos
	  if $ARMv6>0
	
	    if $Round>0
	    else
	      uqadd8  r4, r4, r14
	    endif
	    
	    uhadd8	r4, r4, r8
	
	    if $Add>0
	       ldr	r6,[r1]
	       uqadd8  r4, r4, r14	  
	       uhadd8	r4, r4, r6	
	    endif	
	    str	r4,[r1],r3
	  else
    	and		r6,r12,r4,lsr #1
	      if $Round>0
	         and	r4,r4,r8
	      else
	         orr	r4,r4,r8
	      endif
	      and		r8,r12,r8,lsr #1
	      and		r4,r4,r14
	      add		r4,r4,r6
	      add		r4,r4,r8
	      if $Add>0
	         ldr	r6,[r1]
	         and	r8,r12,r4,lsr #1
	         orr	r4,r6,r4
	         and	r6,r12,r6,lsr #1
	         add	r6,r6,r8
	         and	r4,r4,r14
	         add	r6,r6,r4
	         str	r6,[r1],r3
	      else
	      str	r4,[r1],r3
	      endif
	    endif
	
  mend

  macro
  ArmInter4x8H $Id, $Pos, $Round, $Add, $ARMv6

;	PldBI $ARM5,$Pos
	ldr		r14,$Id.LABM

	ldr		r11, [sp, #40]

;	mov		r11,#8
	mvn		r12,r14,lsl #7

$Id.LABLOOP
	pld [r0,r2]
	CopyHorRow $Id,$Pos,$Round,$Add, $ARMv6
	subs r11,r11,#1
	bne  $Id.LABLOOP

	ldmia	sp!, {r4 - r12, pc}
$Id.LABM dcd 0x01010101
  mend

;------------------------------------------
; ArmInter4x8V
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
  CopyVerRow $Id, $Pos, $Parity, $Round, $Add, $ARMv6

;r14 01010101
;r12 7f7f7f7f

	LoadVerRow $Id,$Pos,$Parity
	  if $ARMv6>0 
        if $Parity>0   
	      if $Round>0
	      else
	        uqadd8  r4, r4, r14
	      endif	
	      uhadd8	r4, r4, r8	
	      if $Add>0
	        ldr	r10,[r1]
	        uqadd8  r4, r4, r14  
	        uhadd8	r4, r4, r10			
	      endif	
	        str	r4,[r1],r3  
	    else
	      if $Round>0
	      else
	        uqadd8  r8, r8, r14
	      endif
	      	
	      uhadd8	r8, r8, r4
	      if $Add>0
	        ldr	r10,[r1]

	        uqadd8  r8, r8, r14	  
	        uhadd8	r8, r8, r10		
	      endif		
	      str	r8,[r1],r3  
	    endif
    else

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
   endif
  mend


  macro
  ArmInter4x8V $Id, $Pos, $Round, $Add, $ARMv6

	ldr		r4, [sp, #40]
	sub		sp,sp,#8
	mul		r4, r2, r4
	ldr		r14,$Id.LABM
	add		r4, r4, r0

;	add		r4,r0,r2,lsl #3
	add		r4,r4,r2
	mov		r7,r4
	str		r4,[sp]		;end src
	str		r3,[sp, #4]	;end src	


	mvn		r12,r14,lsl #7

	pld [r0,r2]
	LoadVerRow $Id,$Pos,1
$Id.LABLOOP
	pld [r0,r2]
	CopyVerRow $Id,$Pos,0,$Round,$Add, $ARMv6
	pld [r0,r2]
	CopyVerRow $Id,$Pos,1,$Round,$Add, $ARMv6

	cmp		r0,r7
	bne		$Id.LABLOOP
	add		sp,sp,#8
	ldmia	sp!, {r4 - r12, pc}
$Id.LABM dcd 0x01010101
  mend

;------------------------------------------
; ArmInter4x8HV
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
  CopyHorVerRow $Id, $Pos, $Parity, $Round, $Add, $ARMv6

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
  ArmInter4x8HV $Id, $Pos, $Round, $Add, $ARMv6
	if $ARMv6>0
	  ldr		r4, [sp, #40]
	  sub		sp,sp,#12
	  mul		r4, r2, r4
;	  str		r3,[sp,#8]	;dstpitch
	  mov		r7, r3
	  add		r4, r4, r0

;	  add		r4,r0,r2,lsl #3
	  add		r4,r4,r2
;	  str		r2,[sp]		;srcpitch
	  mov		r12, r2
	  str		r4,[sp,#4]	;end src

	  ldr		r14,$Id.LABM_ARMv6
;	  mvn		r12,r14,lsl #6
;	  pld [r0,r2]
	  LoadHorVerRow_ARMv6 $Id,$Pos,1, $ARMv6
$Id.LABLOOP_ARMv6
;	  pld [r0,r2]
	  CopyHorVerRow_ARMv6 $Id,$Pos,0,$Round,$Add, $ARMv6
;	  pld [r0,r2]
	  CopyHorVerRow_ARMv6 $Id,$Pos,1,$Round,$Add, $ARMv6
	  cmp		r0,r3
	  bne		$Id.LABLOOP_ARMv6

	  add		sp,sp,#12
	  ldmia	sp!, {r4 - r12, pc}
$Id.LABM_ARMv6 dcd 0x01010101	  
	else

	  ldr		r4, [sp, #40]
	  sub		sp,sp,#12
	  mul		r4, r2, r4
	  str		r3,[sp,#8]	;dstpitch
	  add		r4, r4, r0

;	  add		r4,r0,r2,lsl #3
	  add		r4,r4,r2
	  str		r2,[sp]		;srcpitch
	  str		r4,[sp,#4]	;end src


	  ldr		r14,$Id.LABM
	  mvn		r12,r14,lsl #6
	  pld [r0,r2]
	  LoadHorVerRow $Id,$Pos,1
$Id.LABLOOP
	  pld [r0,r2]
	  CopyHorVerRow $Id,$Pos,0,$Round,$Add, $ARMv6
	  pld [r0,r2]
	  CopyHorVerRow $Id,$Pos,1,$Round,$Add, $ARMv6
	  cmp		r0,r3
	  bne		$Id.LABLOOP

	  add		sp,sp,#12
	  ldmia	sp!, {r4 - r12, pc}
$Id.LABM dcd 0x03030303
    endif
  mend
  
  ; load needs r2,r3 for temporary (r2 is restored from stack)

  macro
  LoadHorVerRow_ARMv6 $Id, $Pos, $Parity, $ARMv6
  
;	ldr		r2,[sp]
	  
  if $Parity>0
;read result r4,r5 and r2,r3 (one pixel to right)
;r6,r7 can be used
    ldr		r5,[r0,#4]
    ldr		r4,[r0],r12

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
	uhadd8	r4, r4, r2
  else
	;read result r8,r9 and r2,r3 (one pixel to right)
	;r10,r11 can be used

    ldr		r9,[r0,#4]
    ldr		r8,[r0],r12

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
	uhadd8	r8, r8, r2
  endif
  mend

  macro
  CopyHorVerRow_ARMv6 $Id, $Pos, $Parity, $Round, $Add, $ARMv6

;r14 03030303
;r12 3f3f3f3f

	LoadHorVerRow_ARMv6 $Id,$Pos,$Parity, $ARMv6

	  if $Round>0
	    mov r3, r14				;0x01010101
	  else
	    mov r3, r14, lsl #1		;0x02020202
	  endif 
		
      if $Parity>0
	    uqadd8  r8, r8, r3		   
	    uhadd8	r8, r8, r4
;	    ldr	r2,[sp,#8]  ;dstpitch
	    ldr	r3,[sp,#4]	;end src for loop compare 	    
	    if $Add>0
	      ldr	r10,[r1]
	      if $Round>0
	      else
	        uqadd8  r8, r8, r14
	      endif				  
	      uhadd8	r8, r10, r8
	    endif		  
	    str	r8,[r1],r7 
      else
	    uqadd8  r4, r4, r3		   
	    uhadd8	r4, r4, r8
;	    ldr	r2,[sp,#8]  ;dstpitch
	    	
	    if $Add>0
	      ldr	r10,[r1]
	      if $Round>0
	      else
	        uqadd8  r4, r4, r14
	      endif			  
  	      uhadd8	r4, r10, r4
	    endif
	    str	r4,[r1],r7   
	  endif
  mend

;---------------------------------------------------
; general unaligned copy (use PldA)
;
;	GenCodeCpy ArmCopy4x8,ArmCopy4x8,0,0,0,1,1,0
;	GenCodeCpy ArmCopy8x16,ArmCopy8x16,0,0,0,1,1,0
;---------------------------------------------------
; general unaligned copy (use PldA)


;---------------------------------------------------
; smaller versions without PldA
;
;	GenCodeCpy ArmInter4x8,ArmInter4x8,0,0,1,0
;	GenCodeCpy ArmInter4x8V,ArmInter4x8V,0,0,0,0
;	GenCodeCpy ArmInter4x8H,ArmInter4x8H,0,0,0,0
;	GenCodeCpy ArmInter4x8HV,ArmInter4x8HV,0,0,0,0
;
;	GenCodeCpy ArmInter2_4x8 ,ArmInter2_4x8 ,0,1,0,0
;	GenCodeCpy ArmInter2_4x8V,ArmInter4x8V,0,1,0,0
;	GenCodeCpy ArmInter2_4x8H,ArmInter4x8H,0,1,0,0
;	GenCodeCpy ArmInter2_4x8HV,ArmInter4x8HV,0,1,0,0
;
;---------------------------------------------------
; smaller versions with PldA
;
;	GenCodeCpy PldAarm_copyblk,ArmInter4x8,0,0,1,1
;	GenCodeCpy PldAarm_copyblkv,ArmInter4x8V,0,0,0,1
;	GenCodeCpy PldAarm_copyblkh,ArmInter4x8H,0,0,0,1
;	GenCodeCpy PldAarm_copyblkhv,ArmInter4x8HV,0,0,0,1
;
;	GenCodeCpy PldAarm_addblk,ArmInter2_4x8 ,0,1,0,1
;	GenCodeCpy PldAarm_addblkv,ArmInter4x8V,0,1,0,1
;	GenCodeCpy PldAarm_addblkh,ArmInter4x8H,0,1,0,1
;	GenCodeCpy PldAarm_addblkhv,ArmInter4x8HV,0,1,0,1
;
;---------------------------------------------------
; larger versions with PldA
; (faster if there is enough intstruction cache available)
;---------------------------------------------------------------------------------------------
;   GenCodeCpy		$Name,			$Sub,		$Round,	$Add,	$Fast,	$ARM5,	$ARMv6,	$ARM11
;
;	GenCodeCpy ArmInter4x8,		ArmInter4x8,	0,	0,	1,	1,	1,	0
;	GenCodeCpy ArmInter4x8V,	ArmInter4x8V,	0,	0,	1,	1,	1,	0
;	GenCodeCpy ArmInter4x8H,	ArmInter4x8H,	0,	0,	1,	1,	1,	0
;	GenCodeCpy ArmInter4x8HV,	ArmInter4x8HV,	0,	0,	1,	1,	1,	0
;                                                                 	  	  	  	
;	GenCodeCpy ArmInter2_4x8 ,	ArmInter2_4x8 ,	0,	1,	1,	1,	1,	0
;	GenCodeCpy ArmInter2_4x8V,	ArmInter4x8V,	0,	1,	1,	1,	1,	0
;	GenCodeCpy ArmInter2_4x8H,	ArmInter4x8H,	0,	1,	1,	1,	1,	0
;	GenCodeCpy ArmInter2_4x8HV,	ArmInter4x8HV,	0,	1,	1,	1,	1,	0
;-----------------------------------------------------------------------------------------------
;   GenCodeCpy		$Name,			$Sub,		$Round,	$Add,	$Fast,	$ARM5,	$ARMv6,	$ARM11

	GenCodeCpy __voMPEG2D0194,ArmCopy4x8,		0,	0,	0,	0,	0,	0
	GenCodeCpy __voMPEG2D0195,ArmCopy8x16,		0,	0,	0,	0,	0,	0
	
	GenCodeCpy __voMPEG2D0186,	ArmInter4x8,	0,	0,	1,	1,	0,	0
	GenCodeCpy __voMPEG2D0187,	ArmInter4x8V,	0,	0,	1,	1,	0,	0
	GenCodeCpy __voMPEG2D0188,	ArmInter4x8H,	0,	0,	1,	1,	0,	0
	GenCodeCpy __voMPEG2D0189,	ArmInter4x8HV,	0,	0,	1,	1,	0,	0
                                                                  	  	  	  	
	GenCodeCpy __voMPEG2D0190,	ArmInter2_4x8 ,	0,	1,	1,	1,	0,	0
	GenCodeCpy __voMPEG2D0191,	ArmInter4x8V,	0,	1,	1,	1,	0,	0
	GenCodeCpy __voMPEG2D0192,	ArmInter4x8H,	0,	1,	1,	1,	0,	0
	GenCodeCpy __voMPEG2D0193,	ArmInter4x8HV,	0,	1,	1,	1,	0,	0
	END