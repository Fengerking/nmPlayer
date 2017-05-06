@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@// Filename fft_arm_v6.s
@
@// Copyright (c) VisualOn SoftWare Co., Ltd. All rights reserved.
@
@//*@@@---@@@@******************************************************************
@//
@// Abstract:
@// 
@//     ARM specific transforms
@//     Optimized assembly armv6 routines to implement DCTIV & FFT and other routines
@//
@//     Custom build with 
@//          armasm -cpu arm1136 $(InputPath) "$(IntDir)/$(InputName).obj"
@//     and
@//          $(OutDir)\$(InputName).obj
@// 
@// Author:
@// 
@//     Witten Wen (Shanghai, China) September 1, 2008
@//
@//****************************************************************************
@//
@// void prvFFT4DCT(Void *ptrNotUsed, CoefType data[], Int nLog2np, FftDirection fftDirection)
@// void prvDctIV_ARM ( rgiCoef, nLog2cSb, CR, CI, CR1, CI1, STEP)
@//
@//****************************************************************************

  @OPT			2       @ disable listing 
  #include "../../../inc/audio/v10/include/voWMADecID.h"
  .include		"kxarm.h"
  .include		"wma_member_arm.inc"
  .include		"wma_arm_version.h"
  @OPT			1       @ enable listing

  @AREA    |.text|, CODE, READONLY
   .text   .align 4

  .if WMA_OPT_FFT_ARM == 1
	.globl  _icosPIbynp
	.globl  _isinPIbynp
	.globl  _FFT4DCT16_STEP

	.globl  _prvFFT4DCT
	.globl  _prvDctIV_ARM
   

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers for FFT
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
j       .req  r0
k       .req  r1

n       .req  r8
m       .req  r9
l       .req  r7

CR      .req  r6
SI      .req  r7
STEP    .req  r10


n1      .req  r2
np      .req  r3
px1     .req  r4
i       .req  r7

CR1_FFT     .req  r2
SI1     .req  r3
CR2_FFT     .req  r4
SI2     .req  r5

px      .req  r10
pxk     .req  r12
pxi     .req  r14
tk      .req  r8
ti      .req  r9
ur      .req  r10
ui      .req  r10
tmp     .req  r11
temp    .req  r11
temp1   .req  r6
temp2   .req  r7


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Constants for FFT
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
.set iStackSpaceRev  , 8*4    
.set iOffset_px      , iStackSpaceRev-4
.set iOffset_np      , iStackSpaceRev-8
.set iOffset_CR      , iStackSpaceRev-12                
.set iOffset_SI      , iStackSpaceRev-16
.set iOffset_STEP    , iStackSpaceRev-20
.set iOffset_l       , iStackSpaceRev-24
.set iOffset_m       , iStackSpaceRev-28
.set iOffset_n       , iStackSpaceRev-32

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   MACROs for FFT
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .macro FFTbUTTERFLY0_N
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@
    ldrd	r8, [$0]
    ldrd	r10, [$1]    
    mov		r8, r8, asr #1
    sub		r6, r8, r10, asr #1
    add		r8, r8, r10, asr #1
    mov		r9, r9, asr #1
    sub		r7, r9, r11, asr #1
    add		r9, r9, r11, asr #1   
    strd	r8, [$0], #8
    strd	r6, [$1], #8
    .endmacro

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.macro FFTbUTTERFLY_N @ vxk, vxi, rCR, rSI @ assuming reg 1,2 are free
	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	ldrd	r8, [$0]
	ldrd	r6, [$1]
	mov     r8, r8, asr #1
	sub     r10, r8, r6, asr #1
	add     r8, r8, r6, asr #1		@r6, r11 is free
	smmulr  r6, $2, r10    	@ 32*32
	mov     r9, r9, asr #1
	smmulr  r11, $3, r10		@ 32*32
	sub     r10, r9, r7, asr #1
	add     r9, r9, r7, asr #1
	smmlsr  r6, $3, r10, r6		@ +32*32@ MULT_CbP2(CR,ur) - MULT_CbP2(SI,ui)
	strd    r8, [$0], #8      @r7, r8, r9 is free	
	smmlar  r7, $2, r10, r11    @ +32*32: MULT_bP2(SI,ur) + MULT_bP2(CR,ui)	
	mov     r8, r6, lsl #2    @ temp1 = MULT_bP2(CR,ur) - MULT_bP2(SI,ui)
	mov     r9, r7, lsl #2    @ temp2 = MULT_CbP2(CR,ui) + MULT_CbP2(SI,ur)
	strd    r8, [$1], #8
	.endmacro


    @@@@@@@@@@@@@@@@@@@@@@@@
	.macro FFTbUTTERFLY0 @vxk, vxi
    @@@@@@@@@@@@@@@@@@@@@@@@
	ldrd	r8, [$0]              @ tk = *pxk
	ldrd	r10, [$1]              @ ti = *pxi	
	add     r6, r8, r10            @ temp = *pxk + *pxi
	sub     r8, r8, r10              @ ur = *pxk - *pxi
	add     r7, r9, r11            @ temp = *pxk + *pxi
	sub     r9, r9, r11              @ ui = *pxk - *pxi@	
	strd	r8, [$1], #8          @ *pxi++ = ui@
	strd	r6, [$0], #8        @ *pxk++ += *pxi@
	.endmacro

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@//****************************************************************************
@//
@// void prvFFT4DCT(Void *ptrNotUsed, CoefType data[], Int nLog2np, FftDirection fftDirection)
@// 
@//****************************************************************************
	.if LINUX_RVDS == 1
  	PRESERVE8
	.endif
  	@AREA    |.text|, CODE
_prvFFT4DCT:

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Input parameters
@ r0 = ptrNotUsed
@ r1 = data
@ r2 = nLog2np
@ r3 = fftDirection

    stmfd   sp!, {r4 - r11, r14}
    sub     sp, sp, #iStackSpaceRev @ rev stack space

@   if ( nLog2np < 16 )
    cmp   r2, #16
    blt   gLOG2NPLT16

@   CR = bP2_FROM_FLOAT(cos(dPI/np))@
@   STEP = bP2_FROM_FLOAT(2*sin(-dPI/np))@

@   call C stub function
    stmfd sp!, {r1 - r2}  @save r1, r2
    sub   sp, sp, #4      @allocate pCR

    mov   r0, r2
    mov   r1, sp
    bl    _FFT4DCT16_STEP

    mov   r4, r0
    ldr   r3, [sp]

    add   sp, sp, #4      @release pCR
    ldmfd sp!, {r1 - r2}  @restore r1, r2
    b     gPrvFFT_ARM

gLOG2NPLT16:
@   CR = bP2_FROM_bP1(icosPIbynp[nLog2np])@         // CR = (I32)(cos(PI/np) * NF2bP2)
    ldr   r12, icosPIbynp
    
@   STEP = isinPIbynp[nLog2np]@                     // STEP = (I32)(2*sin(-PI/np) * NF2bP2)
    ldr   r11, isinPIbynp
    
    ldr   r4, [r12, r2, lsl #2]
    mov   r3, r4, asr #1   
    ldr   r4, [r11, r2, lsl #2]

@ fftDirection is always FFT_FORWARD at decoder side, so ignore it 
@ if (fftDirection == FFT_INVERSE) STEP *= -1;


gPrvFFT_ARM:
    mov     temp, #1
    mov     r2, temp, lsl r2        @ np (r2) = 1<<nLog2np;
    mov     r5, r4, asr #1          @ SI = STEP/2;

    str     r1, [sp, #iOffset_px]   @
    str     r2, [sp, #iOffset_np]   @

    str     r3, [sp, #iOffset_CR]   @
    str     r4, [sp, #iOffset_STEP] @
    str     r5, [sp, #iOffset_SI]   @

    mov     m, r2, lsl #1           @ m = n = 2 * np

    str     m, [sp, #iOffset_n]     @ save n
    str     m, [sp, #iOffset_m]     @ save m

LoopFirstStage:
    cmp     m, #4                   @ m > 4?
    ble     SecondStage

    ldr     STEP,[sp, #iOffset_STEP]@ get STEP
    ldr     SI, [sp, #iOffset_SI]   @ get SI
    ldr     CR, [sp, #iOffset_CR]   @ get CR 

    smmul   CR1_FFT, STEP, SI     		@ 32*32, MULT_bP2(STEP,SI)
    mov     l, m, asr #1            @ l = m >> 1;
    mvn     CR2_FFT, #3, 2              @ CR2_FFT = bP2_FROM_FLOAT(1)
    str     l, [sp, #iOffset_l]     @ save l  

    smmul   SI1, STEP, CR     		@ 32*32, MULT_bP2(STEP,CR)
    sub     CR1_FFT, CR2_FFT, CR1_FFT, lsl #2   @ CR1 = bP2_FROM_FLOAT(1) - MULT_bP2(STEP,SI)@
    mov     SI2, #0                 @ SI2 = 0
    str     CR1_FFT, [sp, #iOffset_CR]  @ save CR

    mov     SI1, SI1, lsl #2        @ shift arithmetic left 2
    mov     STEP, SI1, lsl #1       @ STEP = MUL2(SI1)@    

    str     SI1, [sp, #iOffset_SI]  @ save SI
    str     STEP,[sp, #iOffset_STEP]@ save STEP
     
Trivialbutterfly:   
    mov     k, #0                   @ init k = 0

LoopTrivalbutterfly:
        ldr     px, [sp, #iOffset_px]   @ get px
        ldr     j, [sp, #iOffset_m]     @ j = m 

        add     pxk, px, k, lsl #2      @ pxk = &px[k]@
        add     pxi, pxk, j, lsl #1     @ pxi = &px[k+l]@

        FFTbUTTERFLY0_N pxk, pxi        
        FFTbUTTERFLY_N pxk, pxi, CR1_FFT, SI1

        ldr     n, [sp, #iOffset_n]  
        add     k, k, j                 @ k += m 
        cmp     k, n
        blt     LoopTrivalbutterfly

Nontrivialbutterfly:
    ldr     l, [sp, #iOffset_l]
    mov     j, #4
    cmp     j, l
    bge     OutOfJ

LoopNontrivialbutterfly_j:
        ldr     STEP, [sp, #iOffset_STEP]
		mov     k, j                    @ k = j
        smmul   temp1, STEP, SI1  		@ 32*32, MULT_bP2(STEP,SI1)        
        add     j, j, #4
        @ stall      
       
        smmul   temp2, STEP, CR1_FFT 		@ 32*32, MULT_bP2(STEP,CR1)
        sub     CR2_FFT, CR2_FFT, temp1, lsl #2 @ CR2 -= MULT_bP2(STEP,SI1)
        @ stall
         
        smmul   temp1, STEP, CR2_FFT 		@ 32*32, MULT_bP2(STEP,CR2)
        add     SI2, SI2, temp2, lsl #2 @ SI2 += MULT_bP2(STEP,CR1)
        @ stall

        smmul   temp2, STEP, SI2  		@ 32*32, MULT_bP2(STEP,SI2)
        add     SI1, SI1, temp1, lsl #2 @ SI1 += MULT_bP2(STEP,CR2_FFT)@
        @ stall

        sub     CR1_FFT, CR1_FFT, temp2, lsl #2 @ CR1 -= MULT_bP2(STEP,SI2)@  
        				
LoopCorebutterfly_k:
            ldr     px, [sp, #iOffset_px] @ get px
            ldr     m, [sp, #iOffset_m] @ get m

            add     pxk, px, k, lsl #2  @ pxk = &px[k]@
            add     pxi, pxk, m, lsl #1 @ pxi = &px[k+l]@
            add     k, k, m             @ k = k + m

            FFTbUTTERFLY_N pxk, pxi, CR2_FFT, SI2
            FFTbUTTERFLY_N pxk, pxi, CR1_FFT, SI1    

            ldr     n, [sp, #iOffset_n] @ get n
            cmp     k, n                @ k <= n?
            ble     LoopCorebutterfly_k

        ldr     l, [sp, #iOffset_l]
        cmp     j, l
        blt     LoopNontrivialbutterfly_j

OutOfJ:
    mov     m, l                    @ m = l
    str     l, [sp, #iOffset_m]     @ save m 
    b       LoopFirstStage    
        
SecondStage:
    ldr     px1, [sp, #iOffset_px]      @ get px
    cmp     m, #2                       @ Now m is available, m > 2?
    ble     ThirdStage

    mov     j, #0
    ldr     n1, [sp, #iOffset_n]

LoopSecondStage:
    add     pxk, px1, j, lsl #2         @ pxk = px+j
    add     pxi, pxk, #8                @ pxi = pxk + 2;

    FFTbUTTERFLY0 pxk, pxi

    add     j, j, #4
    cmp     j, n1
    blt     LoopSecondStage

ThirdStage:
    cmp     n1, #4                      @ Now n is available, n > 4?
    ble     EndOfprvFFT_ARM

    ldr     np, [sp, #iOffset_np]       @ get np
    mov     j, #0
    mov     i, #0

LoopThirdStage:    
        cmp     i, j
        bge     ThirdStageEscape
        
		add     pxk, px1, j, lsl #2 @ pxk = &px[j]@
        ldrd    r10, [pxk]        @ temp1 = *pxk;
        add     pxi, px1, i, lsl #2 @ pxi = &px[i]@        
        ldrd    r8, [pxi]          @ tmp = *pxi;        
        strd    r10, [pxi], #8    @ *pxi++ = *pxk;  
        add     pxi, pxi, np, lsl #2 @ pxi  += np1;       
        strd    r8, [pxk], #8      @ *pxk++ = tmp;
        add     pxk, pxk, np, lsl #2 @ pxk  += np1; 
        ldrd    r8, [pxi]          @ tmp = *pxi;        		  
        ldrd    r10, [pxk]        @ temp1 = *pxk;        
        strd    r8, [pxk], #8      @ *pxk++ = tmp;        
        strd    r10, [pxi], #8    @ *pxi++ = *pxk;
		
ThirdStageEscape:

	add     temp1, i, #2
    add     pxi, px1, temp1, lsl #2     @ pxi = &px[i+2]@
    ldrd    r8, [pxi]                  @ tmp = *pxi;
    add     temp1, j, np                
    add     pxk, px1, temp1, lsl #2     @ pxk = &px[j+np]@    
    ldrd    r10, [pxk]                @ temp1 = *pxk
    strd    r8, [pxk], #4              @ *pxk++ = tmp;
    mov     k, np, asr #1               @ k = n2;
    strd    r10, [pxi], #4            @ *pxi++ = *pxk; 

cmp_k_j: 
    cmp     k, j                        @ k <= j?
    bgt     Out
        sub     j, j, k                 @ j -= k;
        mov     k, k, asr #1            @ k = k / 2
        b       cmp_k_j
Out:
    add     j, j, k                     @ j += k
    add     i, i, #4                    @ i = i + 4
    cmp     i, np                       @ i < np? 
    blt     LoopThirdStage

@ fftDirection is always FFT_FORWARD at decoder side, so ignore it 
@ if (fftDirection == FFT_INVERSE) // Normalization to match Intel library
@   for (i = 0; i < 2 * np; i++) data[i] /= np;

    
EndOfprvFFT_ARM:
    add     sp, sp, #iStackSpaceRev     @ give back rev stack space
    ldmfd   sp!, {r4 - r11, PC}         @ prvFFT4DCT

icosPIbynp: .word _icosPIbynp
isinPIbynp: .word _isinPIbynp

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers for DCTIV
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

piCoefTop       .req  r0
piCoefbottom    .req  r1
CR_DCT          .req  r2
CI_DCT          .req  r3
CR1_DCT         .req  r4
CI1_DCT         .req  r5
STEP_DCT        .req  r6
CR2_DCT         .req  r4
CI2_DCT         .req  r5
iTi             .req  r9  
iTr             .req  r8
ibi             .req  r9
ibr             .req  r8
i_DCT           .req  r7
temp0_DCT       .req  r12
temp1_DCT       .req  r10
temp2_DCT       .req  r11
temp3_DCT       .req  r14
temp4_DCT       .req  r1
temp5_DCT		.req	r9


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Constants for DCTIV
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.set iRegSpaceDCT        , 9*4   @ {r4 - r11, r14}

.set iStackSpaceRevDCT   , 3*4    
.set iOffset_FFTSize     , iStackSpaceRevDCT-4
.set iOffset_nLog2cSb    , iStackSpaceRevDCT-8 
.set iOffset_rgiCoef     , iStackSpaceRevDCT-12

.set iOffset_CR1_DCT     , iRegSpaceDCT+iStackSpaceRevDCT
.set iOffset_CI1_DCT     , iRegSpaceDCT+iStackSpaceRevDCT+4
.set iOffset_STEP_DCT    , iRegSpaceDCT+iStackSpaceRevDCT+8
.set iOffset_CR2_DCT     , iRegSpaceDCT+iStackSpaceRevDCT+12


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	PRESERVE8
    @AREA    |.text|, CODE
_prvDctIV_ARM:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    stmfd   sp!, {r4 - r11, r14}
    sub     sp, sp, #iStackSpaceRevDCT  @setup stack

    mov     temp0_DCT, #1
    sub     temp1_DCT, r1, #2
    mov     i_DCT, temp0_DCT, lsl temp1_DCT             @ iFFTSize/2 = 1<<(nLog2cSb-2)@

    ldr     CR1_DCT, [sp, #iOffset_CR1_DCT]             @ get CR1
    ldr     CI1_DCT, [sp, #iOffset_CI1_DCT]             @ get CI1
    ldr     STEP_DCT, [sp, #iOffset_STEP_DCT]           @ get STEP

    mov     temp0_DCT, temp0_DCT, lsl r1                @ temp = 1<<nLog2cSb
    str     r1, [sp, #iOffset_nLog2cSb]

    sub     temp0_DCT, temp0_DCT, #1                    @ temp = (1<<nLog2cSb) - 1
    
    ldrd	iTr, [piCoefTop]							@ iTr = piCoefTop[0]@ temp5_DCT = piCoefTop[1]@
    add     piCoefbottom, piCoefTop, temp0_DCT, lsl #2  @ piCoefbottom = rgiCoef + (1<<nLog2cSb) - 1;
    
    str     i_DCT, [sp, #iOffset_FFTSize]
    str     piCoefTop, [sp, #iOffset_rgiCoef]

FirstDCTStageLoop:
		ldr     r14, [piCoefbottom]                     @ ibi = piCoefbottom[0]@ r14 is ibi here
    smmulr	temp1_DCT, CR_DCT, iTr       			@ MULT_bP1(CR,iTr)
    str		temp5_DCT, [piCoefbottom], #-8          @ piCoefbottom[0] = piCoefTop[1]@
    smmulr	temp2_DCT, CR_DCT, r14       			@ MULT_bP1(CR,ibi)
    subs    i_DCT, i_DCT, #1                        @ i --@	
    smmlsr	temp1_DCT, CI_DCT, r14, temp1_DCT		@ MULT_bP1(CI,ibi)
    
    mov     temp3_DCT, CR_DCT
    smmlar	temp2_DCT, CI_DCT, iTr, temp2_DCT		@ temp2 = MULT_bP1(CR,ibi) + MULT_bP1(CI,iTr)
    mov     temp1_DCT, temp1_DCT, lsl #1            @ temp1 = MULT_bP1(CR,iTr) - MULT_bP1(CI,ibi)

    smmul   temp5_DCT, STEP_DCT, CI_DCT  			@ MULT_bP1(STEP,CI)
    mov     temp2_DCT, temp2_DCT, lsl #1            @ temp2 = MULT_bP1(CR,ibi) + MULT_bP1(CI,iTr)@
    strd	temp1_DCT, [piCoefTop], #8              @ piCoefTop[0] = MULT_bP1(CR,iTr) - MULT_bP1(CI,ibi);piCoefTop[1] = MULT_bP1(CR,ibi) + MULT_bP1(CI,iTr)@
    
    smmul   temp2_DCT, STEP_DCT, CR_DCT  			@ MULT_bP1(STEP,CR)
    sub     CR_DCT, CR1_DCT, temp5_DCT, lsl #1      @ CR = CR1 - MULT_bP1(STEP,CI)@
    mov     CR1_DCT, temp3_DCT                      @ CR1 = CR;
		ldrd	iTr, [piCoefTop]						@ iTr = piCoefTop[0]@ r9 = piCoefTop[1]@
	
    mov     temp3_DCT, CI_DCT
    add     CI_DCT, CI1_DCT, temp2_DCT, lsl #1      @ CI = CI1 + MULT_bP1(STEP,CR)@  
    mov     CI1_DCT, temp3_DCT                      @ CI1 = CI;

    bne     FirstDCTStageLoop


    ldr     i_DCT, [sp, #iOffset_FFTSize]

SecondDCTStageLoop:                                  
    smmulr   temp1_DCT, CR_DCT, iTr       			@ MULT_bP1(CR,iTr)
    mov     temp3_DCT, CR_DCT						@	

    smmulr   temp2_DCT, CR_DCT, iTi       			@ MULT_bP1(CR,iTi)
    subs    i_DCT, i_DCT, #1                        @ i --@
    
    smmlsr  temp1_DCT, CI_DCT, iTi, temp1_DCT		@ MULT_bP1(CR,iTr) + MULT_bP1(-CI,iTi)

    smmul   temp0_DCT, STEP_DCT, CR_DCT  			@ MULT_bP1(STEP,CR)
    
    smmlar	temp2_DCT, CI_DCT, iTr, temp2_DCT		@ MULT_bP1(CR,iTi) + MULT_bP1(CI,iTr)    
    mov     temp1_DCT, temp1_DCT, lsl #1            @ temp1 = MULT_bP1(CR,iTr) - MULT_bP1(CI,iTi)@
    
		smmul   temp5_DCT, STEP_DCT, CI_DCT  			@ MULT_bP1(STEP,CI)		
		mov     temp2_DCT, temp2_DCT, lsl #1            @ temp2 = MULT_bP1(CI,iTr) + MULT_bP1(CR,iTi)@
		strd	temp1_DCT, [piCoefTop], #8              @ piCoefTop[0] = MULT_bP1(CR,iTr) - MULT_bP1(CI,iTi);piCoefTop[1] = MULT_bP1(CR,iTi) + MULT_bP1(CI,iTr)@
    
    mov     temp4_DCT, CI_DCT							
    
    sub     CR_DCT, CR1_DCT, temp5_DCT, lsl #1      @ CR = CR1 - MULT_bP1(STEP,CI)@
    mov     CR1_DCT, temp3_DCT                      @ CR1 = CR;
    
		ldrd	iTr, [piCoefTop]						@ iTr = piCoefTop[0]@ iTi = piCoefTop[1]@
    add     CI_DCT, CI1_DCT, temp0_DCT, lsl #1      @ CI = CI1 + MULT_bP1(STEP,CR)@
    
    mov     CI1_DCT, temp4_DCT                      @ CI1 = CI;

    bne     SecondDCTStageLoop

CallFFT:
@   prvFFT4DCT(NULL, rgiCoef, nLog2cSb - 1, FFT_FORWARD)@
		ldr     r2, [sp, #iOffset_nLog2cSb]
    ldr     r1, [sp, #iOffset_rgiCoef]    
    sub     r2, r2, #1
@   mov     r3, #0
    bl      _prvFFT4DCT

    ldr     temp1_DCT, [sp, #iOffset_nLog2cSb]
    ldr     piCoefTop, [sp, #iOffset_rgiCoef]

    mov     temp0_DCT, #1
    mov     temp0_DCT, temp0_DCT, lsl temp1_DCT         @ temp = 1<<nLog2cSb
    sub     temp0_DCT, temp0_DCT, #2                    @ temp = (1<<nLog2cSb) - 2
    add     piCoefbottom, piCoefTop, temp0_DCT, lsl #2  @ piCoefbottom = rgiCoef + (1<<nLog2cSb) - 2;
	
    mvn     CR_DCT, #2, 2                               @ CR = bP1_FROM_FLOAT(1)@
    mov     CI_DCT, #0   								@ CI = 0
                                   
		ldrd	iTr, [piCoefTop]							@iTr = piCoefTop[0]@ iTi = piCoefTop[1]@
    ldr     i_DCT, [sp, #iOffset_FFTSize]
    ldr     CR2_DCT, [sp, #iOffset_CR2_DCT]             @ get CR2

    mov     CI2_DCT, STEP_DCT, asr #1                   @ DIV2 of STEP
    rsb     CI2_DCT, CI2_DCT, #0                        @ CI2  = -DIV2(STEP)@ 


ThirdDCTStageLoop:
    smmulr	temp1_DCT, CR_DCT, iTr       			@ MULT_bP1(CR,iTr)    
    smmulr	temp2_DCT, CI_DCT, iTr       			@ MULT_bP1(CI,iTr)    
    smmlsr	temp1_DCT, CI_DCT, iTi, temp1_DCT       @ MULT_bP1(CR,iTr) -  MULT_bP1(CI,iTi)@    
    smmlar	temp2_DCT, CR_DCT, iTi, temp2_DCT       @ MULT_bP1(CI,iTr) + MULT_bP1(CR,iTi)@
    mov     temp1_DCT, temp1_DCT, lsl #1            @ temp1 = MULT_bP1(CR,iTr) -  MULT_bP1(CI,iTi)@
    str     temp1_DCT, [piCoefTop], #4              @ piCoefTop[0] =  MULT_bP1(CR,iTr) -  MULT_bP1(CI,iTi)@
    rsb		temp2_DCT, temp2_DCT, #0				@ MULT_bP1(-CI,iTr) - MULT_bP1(CR,iTi)@
    subs    i_DCT, i_DCT, #1                        @ i --@
    smmulr	temp1_DCT, STEP_DCT, CI_DCT  			@ MULT_bP1(STEP,CI)@
    mov     temp2_DCT, temp2_DCT, lsl #1            @ temp2 = MULT_bP1(-CI,iTr) - MULT_bP1(CR,iTi)@
    ldrd	ibr, [piCoefbottom]						@ ibr; ibi
    str     temp2_DCT, [piCoefbottom, #4]           @ piCoefbottom[1] =  MULT_bP1(-CI,iTr) - MULT_bP1(CR,iTi)@
    
    smmulr	temp2_DCT, STEP_DCT, CR_DCT  			@ MULT_bP1(STEP,CR)@
    mov     temp3_DCT, CR_DCT	 
    sub     CR_DCT, CR2_DCT, temp1_DCT, lsl #1      @ CR = CR2 - MULT_bP1(STEP,CI)@
    mov     CR2_DCT, temp3_DCT                      @ CR2 = CR;  
    mov     temp3_DCT, CI_DCT 

    smmulr	temp1_DCT, CR_DCT, ibr       			@ MULT_bP1(CR,ibr)
    add     CI_DCT, CI2_DCT, temp2_DCT, lsl #1      @ CI = CI2 + MULT_bP1(STEP,CR)@    
		smmulr	temp2_DCT, CR_DCT, ibi					@ MULT_bP1(CR,ibi)@
	
    smmlar	temp1_DCT, CI_DCT, ibi, temp1_DCT		@ MULT_bP1(CR,ibr) + MULT_bP1(CI,ibi)@
    mov     CI2_DCT, temp3_DCT                      @ CI2 = CI;
    
    smmlsr	temp2_DCT, CI_DCT, ibr, temp2_DCT		@ MULT_bP1(-CI,ibr) +  MULT_bP1(CR,ibi)@
    mov     temp1_DCT, temp1_DCT, lsl #1            @ temp1 = MULT_bP1(CR,ibr) + MULT_bP1(CI,ibi)@
    str     temp1_DCT, [piCoefTop], #4              @ piCoefTop[1] = MULT_bP1(CR,ibr) + MULT_bP1(CI,ibi)@
   
		mov     temp2_DCT, temp2_DCT, lsl #1            @ temp2 = MULT_bP1(-CI,ibr) +  MULT_bP1(CR,ibi)@
		ldrd	iTr, [piCoefTop]						@iTr = piCoefTop[0]@ iTi = piCoefTop[1]@
    str     temp2_DCT, [piCoefbottom], #-8          @ piCoefbottom[0] = MULT_bP1(-CI,ibr) +  MULT_bP1(CR,ibi)@
        
    bne     ThirdDCTStageLoop

EndOfprvDctIV_ARM:
    add     sp, sp, #iStackSpaceRevDCT  @ give back rev stack space
    ldmfd   sp!, {r4 - r11, PC}         @ prvDctIV_ARM
    @ENTRY_END   prvDctIV_ARM



    .endif @ WMA_OPT_FFT_ARM

    @.end

