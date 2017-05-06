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
  CopyBegin
	add ip,r2,#7
	pld [r0,r2]
	pld [r0,#7]
	stmdb sp!,{r4, lr}
	ldr	r4, [sp, #8]
	pld [r0,ip]
	pld [r0]
  mend

  macro
  CopyEnd
	ldmia sp!,{r4, pc}  
  mend

  macro 
  PldA
	pld [r0,r2,lsl #1]
  mend

  macro 
  PldBI
	add ip,r2,#4
  mend

  macro 
  PldB
	pld [r0,r2,lsl #1] ;2*pitch
	pld [r0,ip,lsl #1] ;2*pitch+8
  mend

  macro
  PrepareAlignVer $Name
    ands r14,r0,#7
	tmcr wcgr1,r14
	mov r14,r4
	beq $Name.LABA
	bic r0,r0,#7
  mend

  macro
  PrepareAlignHor $Name
    and r14,r0,#7
	tmcr wcgr1,r14
    add r14,r14,#1
	bic r0,r0,#7
	cmp r14,#8
	moveq r14,r4
	beq $Name.LABW
	tmcr wcgr2,r14
	mov r14,r4
  mend

;------------------------------------------
;COPYBLOCK
;------------------------------------------

  macro
  CInter8X8 $Name

	align 16
	export $Name
$Name proc
    CopyBegin
	sub r1,r1,r3
	PrepareAlignVer $Name

	PldBI
$Name.LABLOOPA
	PldB
	wldrd wr0,[r0]
	wldrd wr1,[r0,#8]
	add	r0,r0,r2
	add r1,r1,r3
	walignr1 wr0,wr0,wr1
	wstrd wr0,[r1]
	subs r14,r14,#1
	bne $Name.LABLOOPA
	CopyEnd

$Name.LABA
	PldA
	wldrd wr0,[r0]
	add	r0,r0,r2
	add r1,r1,r3
	wstrd wr0,[r1]
	subs r14,r14,#1
	bne $Name.LABA
	CopyEnd

  mend

;------------------------------------------
;ADDBLOCK
;------------------------------------------

  macro
  AddBlock $Name
	align 16
	export $Name
$Name proc
	CopyBegin
	PrepareAlignVer $Name

	PldBI
$Name.LABLOOPA
	PldB
	wldrd wr0,[r0]
	wldrd wr1,[r0,#8]
	add	r0,r0,r2
	wldrd wr5,[r1]
	walignr1 wr0,wr0,wr1
	wavg2br wr0,wr0,wr5
	wstrd wr0,[r1]
	add	r1,r1,r3
	subs r14,r14,#1
	bne $Name.LABLOOPA
	CopyEnd

$Name.LABA
	PldA
	wldrd wr0,[r0]
	add	r0,r0,r2
	wldrd wr5,[r1]
	wavg2br wr0,wr0,wr5
	wstrd wr0,[r1]
	add	r1,r1,r3
	subs r14,r14,#1
	bne $Name.LABA
	CopyEnd

  mend

;------------------------------------------
; COPYBLOCKHOR
;------------------------------------------

  macro
  CopyHorRow $Round, $Add, $Wrap
    PldB
	wldrd wr0,[r0]
	wldrd wr1,[r0,#8]
	add	r0,r0,r2
    if $Add > 0
	  wldrd wr5,[r1]
    else
	  add r1,r1,r3
    endif
    walignr1 wr2,wr0,wr1
    if $Wrap = 0
	  walignr2 wr1,wr0,wr1
    endif
    if $Round > 0
	  wavg2b wr0,wr1,wr2
    else
	  wavg2br wr0,wr1,wr2
    endif
    if $Add > 0
	  wavg2br wr0,wr0,wr5
		wstrd wr0,[r1]
		add	r1,r1,r3
    else
	  wstrd wr0,[r1]
	endif
  mend

  macro
  CInter8X8H $Name, $Round, $Add
	align 16
	export $Name
$Name proc
	CopyBegin
    if $Add = 0
	  sub r1,r1,r3
	endif
	PldBI
	PrepareAlignHor $Name

$Name.LABLOOPA
	CopyHorRow $Round,$Add,0
	subs r14,r14,#1
	bne $Name.LABLOOPA
	CopyEnd

$Name.LABW
	CopyHorRow $Round,$Add,1
	subs r14,r14,#1
	bne $Name.LABW
	CopyEnd

  mend

;------------------------------------------
; COPYBLOCKVER
;------------------------------------------

  macro
  SetVerRow $Round, $Add
  if $Add > 0
    wldrd wr5,[r1]
  else
    add r1,r1,r3
  endif
  if $Round > 0
	wavg2b wr1,wr0,wr2
  else
 	wavg2br wr1,wr0,wr2
  endif
  if $Add > 0
    wavg2br wr1,wr1,wr5
	wstrd wr1,[r1]
	add	r1,r1,r3
  else
    wstrd wr1,[r1]
  endif
  mend

  macro
  CInter8X8V $Name, $Round, $Add
	align 16
	export $Name
$Name proc
	CopyBegin
    if $Add = 0
	  sub r1,r1,r3
	endif
	PrepareAlignVer $Name

	PldBI
	PldB
	wldrd wr0,[r0]
	wldrd wr1,[r0,#8]
	add	r0,r0,r2
	walignr1 wr0,wr0,wr1

$Name.LABLOOPA
	PldB
	wldrd wr2,[r0]
	wldrd wr1,[r0,#8]
	add	r0,r0,r2
	walignr1 wr2,wr2,wr1
	SetVerRow $Round,$Add
	PldB
	wldrd wr0,[r0]
	wldrd wr1,[r0,#8]
	add	r0,r0,r2
	walignr1 wr0,wr0,wr1
	SetVerRow $Round,$Add
	subs r14,r14,#2
	bne $Name.LABLOOPA
	CopyEnd

$Name.LABA

	PldA	
	wldrd wr0,[r0]
	add	r0,r0,r2

$Name.LABLOOPB
	PldA	
	wldrd wr2,[r0]
	add	r0,r0,r2
	SetVerRow $Round,$Add
	PldA	
	wldrd wr0,[r0]
	add	r0,r0,r2
	SetVerRow $Round,$Add
	subs r14,r14,#2
	bne $Name.LABLOOPB
	CopyEnd
  mend

;------------------------------------------
; COPYBLOCKHORVER
;------------------------------------------

; wr6 0x03
; wr7 ~0x03

  macro
  LoadHorVerRow $Parity, $Wrap
  PldB
  if $Parity
	wldrd wr0,[r0]
	wldrd wr1,[r0,#8]
	add	r0,r0,r2
	walignr1 wr2,wr0,wr1
    if $Wrap = 0
	  walignr2 wr1,wr0,wr1
    endif

	wand wr0,wr2,wr6
	wand wr3,wr1,wr6
	wand wr2,wr2,wr7
	wand wr1,wr1,wr7
	wsrldg wr2,wr2,wcgr0  
	wsrldg wr1,wr1,wcgr0  
	waddb wr2,wr2,wr1
	waddb wr1,wr3,wr0 
  else
	wldrd wr3,[r0]
	wldrd wr4,[r0,#8]
	add	r0,r0,r2
	walignr1 wr5,wr3,wr4
    if $Wrap = 0
	  walignr2 wr4,wr3,wr4
    endif

	wand wr0,wr5,wr6
	wand wr3,wr4,wr6
	wand wr5,wr5,wr7
	wand wr4,wr4,wr7
	wsrldg wr5,wr5,wcgr0  
	wsrldg wr4,wr4,wcgr0  
	waddb wr5,wr5,wr4
	waddb wr4,wr3,wr0
  endif
  mend

  macro
  SetHorVerRow $Add
  if $Add > 0
    wldrd wr9,[r1]
  else
    add r1,r1,r3
  endif
	waddb wr0,wr1,wr4
	waddb wr0,wr0,wr8   ;rounding
	wand wr0,wr0,wr7
	waddb wr3,wr2,wr5   
	wsrldg wr0,wr0,wcgr0  
	waddb wr0,wr0,wr3 
  if $Add > 0
    wavg2br wr0,wr0,wr9
	wstrd wr0,[r1]
	add	r1,r1,r3
  else
    wstrd wr0,[r1]
  endif
  mend

  macro
  CInter8X8HV $Name, $Round, $Add
	align 16
	export $Name
$Name proc
	CopyBegin
    if $Add = 0
	  sub r1,r1,r3
	endif
    if $Round > 0
	  mov r14,#1
    else
	  mov r14,#2
    endif
	tbcstb wr8,r14
	mov r14,#3 
	tbcstb wr6,r14
	mvn r14,#3 
	tbcstb wr7,r14
	mov r14,#2
	tmcr wcgr0,r14
	PldBI
	PrepareAlignHor $Name

	LoadHorVerRow 1,0

$Name.LABLOOPA
	LoadHorVerRow 0,0
	SetHorVerRow $Add
	LoadHorVerRow 1,0
	SetHorVerRow $Add
	subs r14,r14,#2
	bne $Name.LABLOOPA
	CopyEnd

$Name.LABW
	LoadHorVerRow 1,1

$Name.LABLOOPB
	LoadHorVerRow 0,1
	SetHorVerRow $Add
	LoadHorVerRow 1,1
	SetHorVerRow $Add
	subs r14,r14,#2
	bne $Name.LABLOOPB
	CopyEnd
  mend

	CInter8X8 WmmxInter8x8
	CInter8X8V WmmxInter8x8V,0,0
	CInter8X8H WmmxInter8x8H,0,0
	CInter8X8HV WmmxInter8x8HV,0,0

	CInter8X8V WmmxInter8x8VRD,1,0
	CInter8X8H WmmxInter8x8HRD,1,0
	CInter8X8HV WmmxInter8x8HVRD,1,0

	AddBlock wmmx_addblk
	CInter8X8V wmmx_addblkv,0,1
	CInter8X8H wmmx_addblkh,0,1
	CInter8X8HV wmmx_addblkhv,0,1

	END

