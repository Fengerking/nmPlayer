  #include "../../../inc/audio/v10/include/voWMADecID.h"
  .include "kxarm.h"
  .include "wma_member_arm.inc"
  .include "wma_arm_version.h"
	
  @AREA COMMON, CODE, READONLY
  .text .align 4
	
	.if	ARMVERSION	>= 7
	.if WMA_OPT_FFT_ARM == 1
	
	.globl  _icosPIbynp
	.globl  _isinPIbynp
	.globl  _FFT4DCT16_STEP
	
	.globl  _prvFFT4DCT
	.globl  _prvDctIV_ARM
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers for FFT
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
px					.req	r1
np					.req	r2
CR					.req	r3
SI					.req	r4
STEP				.req	r5


m					.req	r8
n					.req	r9
l					.req	r10
CR1_FFT					.req	r6
SI1_FFT					.req	r7

CR2_FFT					.req	r0
SI2					.req	r12
k					.req	r14
j					.req	r11

i					.req	r0

pxk					.req	r3
pxi					.req	r4

temp    			.req	r11
temp1				.req	r3
temp2				.req	r4
temp3				.req	r12
temp4				.req	r6
@******************************
@ Neon register 
@******************************
CR1_SI1				.req	D0				@D0[0] = CR1, D0[1] = SI1
NSI1_CR1			.req	D1				@D1[0] = -SI1, D1[1] = CR1
CR2_SI2				.req	D2				
NSI2_CR2			.req	D3

A0					.req	D4
AA1					.req	D5
AA2					.req	D6
AA3					.req	D7

B0					.req	Q2
B1					.req	Q3
B2					.req	Q4
B3					.req	Q5
B4					.req	Q7
B5					.req	Q6

CC0					.req	D8
CC1					.req	D9
CC2					.req	D10
CC3					.req	D11
CC4					.req	D12
CC5					.req	D13
CC6					.req	D14
CC7					.req	D15
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Constants for FFT
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
.set	iStackSpaceRev  , 8    
.set	iOffset_CR      , iStackSpaceRev-4
.set	iOffset_SI      , iStackSpaceRev-8

@//****************************************************************************
@//
@// void prvFFT4DCT(Void *ptrNotUsed, CoefType data[], Int nLog2np, FftDirection fftDirection)
@// 
@//****************************************************************************
	.if LINUX_RVDS == 1
  	PRESERVE8
	.endif
@  	AREA    |.text|, CODE
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
    
@   CR = BP2_FROM_FLOAT(cos(dPI/np))@
@   STEP = BP2_FROM_FLOAT(2*sin(-dPI/np))@

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
@   CR = BP2_FROM_BP1(icosPIbynp[nLog2np])@         // CR = (I32)(cos(PI/np) * NF2BP2)
	ldr   r12, DATABEGIN @=icosPIbynp
    
@   STEP = isinPIbynp[nLog2np]@                     // STEP = (I32)(2*sin(-PI/np) * NF2BP2)
	ldr   r11, DATABEGIN + 4 @==isinPIbynp
	
	ldr   r4, [r12, r2, LSL #2]	
	ldr   STEP, [r11, r2, LSL #2]	
	mov   r3, r4, ASR #1
@ fftDirection is always FFT_FORWARD at decoder side, so ignore it 
@ if (fftDirection == FFT_INVERSE) STEP *= -1@

gPrvFFT_ARM:
	mov     temp, #1	
	mov		r4, STEP, ASR #1          @ SI = STEP/2@
	mov     np, temp, LSL r2        @ np (r2) = 1<<nLog2np@
	
	str     r4, [sp, #iOffset_SI]   @
	str     r3, [sp, #iOffset_CR]   @
	mov     m, np, LSL #1           @ m = n = 2 * np
	mov		n, m
	
LoopFirstStage:
	cmp     m, #4                   @ m > 4?
	ble     SecondStage
	
	ldr     SI, [sp, #iOffset_SI]   @ get SI
	ldr     CR, [sp, #iOffset_CR]   @ get CR 
    
	smmul   CR1_FFT, STEP, SI     		@ 32*32, MULT_BP2(STEP,SI)
	mov     l, m, ASR #1            @ l = m >> 1@
	mvn     CR2_FFT, #3, 2              @ CR2 = BP2_FROM_FLOAT(1)
	
	smmul   SI1_FFT, STEP, CR     		@ 32*32, MULT_BP2(STEP,CR)
	sub     CR1_FFT, CR2_FFT, CR1_FFT, LSL #2   @ CR1_FFT = BP2_FROM_FLOAT(1) - MULT_BP2(STEP,SI)@
	mov     SI2, #0                 @ SI2 = 0
	str     CR1_FFT, [sp, #iOffset_CR]  @ save CR
	
	mov     SI1_FFT, SI1_FFT, LSL #2        @ shift arithmetic left 2
 	mov     STEP, SI1_FFT, LSL #1       @ STEP = MUL2(SI1)@ 
 	str     SI1_FFT, [sp, #iOffset_SI]  @ save SI
 	
TrivialButterfly:   
	mov     k, #0                   @ init k = 0
	vmov	CR1_SI1, CR1_FFT, SI1_FFT
	rsb		temp1, SI1_FFT, #0
	vmov	NSI1_CR1, temp1, CR1_FFT
	
LoopTrivalButterfly:
		add     		pxk, px, k, LSL #2      @ pxk = &px[k]@
		add     		pxi, pxk, m, LSL #1     @ pxi = &px[k+l]@
		vld1.32			{A0, AA1}, [pxk]	
		vld1.32			{AA2, AA3}, [pxi]
		vhsub.S32		B2, B0, B1				@C0, C1	
		vhadd.S32		B3, B0, B1
		add     		k, k, m                 @ k += m
		cmp     		k, n
		vqdmull.S32		B4, CR1_SI1, CC1[0]
		vqdmlal.S32		B4, NSI1_CR1, CC1[1]
		vst1.32			{CC2, CC3}, [pxk]!
		vqshl.S64		B4, #1
		vst1.32			{CC0}, [pxi]!
		vst2.32			{CC6[1], CC7[1]}, [pxi]!	@15 stalls
		blt     		LoopTrivalButterfly

NontrivialButterfly:
	mov     j, #4
	cmp     j, l
	bge     OutOfJ
	
LoopNontrivialButterfly_j:
		mov     k, j                    @ k = j
		smmul   temp1, STEP, SI1_FFT  		@ 32*32, MULT_BP2(STEP,SI1)        
        add     j, j, #4
        @ stall      
       
        smmul   temp2, STEP, CR1_FFT 		@ 32*32, MULT_BP2(STEP,CR1)
        sub     CR2_FFT, CR2_FFT, temp1, LSL #2 @ CR2_FFT -= MULT_BP2(STEP,SI1)
        @ stall
         
        smmul   temp1, STEP, CR2_FFT 		@ 32*32, MULT_BP2(STEP,CR2)
        add     SI2, SI2, temp2, LSL #2 @ SI2 += MULT_BP2(STEP,CR1)
        @ stall

        smmul   temp2, STEP, SI2  		@ 32*32, MULT_BP2(STEP,SI2)
        add     SI1_FFT, SI1_FFT, temp1, LSL #2 @ SI1_FFT += MULT_BP2(STEP,CR2)@
        @ stall

        sub     CR1_FFT, CR1_FFT, temp2, LSL #2 @ CR1_FFT -= MULT_BP2(STEP,SI2)@
		
		vmov	CR1_SI1, CR1_FFT, SI1_FFT
		rsb		temp1, SI1_FFT, #0
		vmov	NSI1_CR1, temp1, CR1_FFT
		vmov	CR2_SI2, CR2_FFT, SI2
		rsb		temp1, SI2, #0
		vmov	NSI2_CR2, temp1, CR2_FFT
		
LoopCoreButterfly_k:
			add     		pxk, px, k, LSL #2      @ pxk = &px[k]@
			add     		pxi, pxk, m, LSL #1     @ pxi = &px[k+l]@
			vld1.32			{A0, AA1}, [pxk]			
			vld1.32			{AA2, AA3}, [pxi]
			vsub.S32		B2, B0, B1
			vhadd.S32		B3, B0, B1							
			vqdmull.S32		B5, CR2_SI2, CC0[0]
			vqdmull.S32		B4, CR1_SI1, CC1[0]
			vst1.32			{CC2, CC3}, [pxk]!			
			vqdmlal.S32		B5, NSI2_CR2, CC0[1]	
			add     		k, k, m             @ k = k + m
			cmp     		k, n                @ k <= n?					
			vqdmlal.S32		B4, NSI1_CR1, CC1[1]
			vst4.32			{CC4[1], CC5[1], CC6[1], CC7[1]}, [pxi]!
@			15 stalls			
			ble     LoopCoreButterfly_k
			
		cmp     j, l
		blt     LoopNontrivialButterfly_j
		
OutOfJ:
	mov     m, l                    @ m = l
	b       LoopFirstStage
	
SecondStage:
	cmp     m, #2                       @ Now m is available, m > 2?
	ble     ThirdStage
	mov     j, #0
	
LoopSecondStage:
	add     	pxk, px, j, LSL #2         @ pxk = px+j
	add     	pxi, pxk, #8                @ pxi = pxk + 2@	
	vld1.32		{A0}, [pxk]              @ tk = *pxk
	vld1.32		{AA1}, [pxi]              @ ti = *pxi	
	vqadd.S32	CC0, A0, AA1
	vqsub.S32	CC1, A0, AA1
	add     	j, j, #4
	cmp     	j, n
	vst1.S32	CC0, [pxk]!
	vst1.S32	CC1, [pxi]!
	@6 stalls
	blt			LoopSecondStage
	
ThirdStage:
	cmp     n, #4                      @ Now n is available, n > 4?
	ble     EndOfprvFFT_ARM
	
	mov     j, #0
	mov     i, #0
	
LoopThirdStage:
		cmp     i, j
		bge     ThirdStageEscape
		@5 stalls
		add     	pxk, px, j, LSL #2 @ pxk = &px[j]@
		add     	pxi, px, i, LSL #2 @ pxi = &px[i]@
		vld1.32		{A0}, [pxk]
		vld1.32		{AA1}, [pxi]
		vst1.32		{A0}, [pxi]!
		add     	pxi, pxi, np, LSL #2 @ pxi  += np1@
		vst1.32		{AA1}, [pxk]!
		add     	pxk, pxk, np, LSL #2 @ pxk  += np1@
		vld1.32		{A0}, [pxi]
		vld1.32		{AA1}, [pxk]
		vst1.32		{A0}, [pxk]
		vst1.32		{AA1}, [pxi]
		
ThirdStageEscape:
		add			temp3, i, #2
		add			temp4, j, np                
		add			pxi, px, temp3, LSL #2     @ pxi = &px[i+2]@
		add			pxk, px, temp4, LSL #2     @ pxk = &px[j+np]@
		vld1.32		{A0}, [pxi]						
		vld1.32		{AA1}, [pxk]
		mov			k, np, ASR #1               @ k = n2@
		vst1.32		{A0}, [pxk]		
		vst1.32		{AA1}, [pxi]
		@3 stalls
Cmp_k_j:
		cmp     k, j                        @ k <= j?
    bgt     Out
		sub     j, j, k                 @ j -= k@
		mov     k, k, ASR #1            @ k = k / 2
		b       Cmp_k_j
Out:
		add     j, j, k                     @ j += k
		add     i, i, #4                    @ i = i + 4
		cmp     i, np                       @ i < np? 
		blt     LoopThirdStage
			
@ fftDirection is always FFT_FORWARD at decoder side, so ignore it 
@ if (fftDirection == FFT_INVERSE) // Normalization to match Intel library
@   for (i = 0@ i < 2 * np@ i++) data[i] /= np@
			
EndOfprvFFT_ARM:
	add     sp, sp, #iStackSpaceRev     @ give back rev stack space
	ldmfd   sp!, {r4 - r11, PC}         @ prvFFT4DCT
	@ENTRY_END   prvFFT4DCT
	
DATABEGIN:
	.word _icosPIbynp
	.word _isinPIbynp


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers for DCTIV
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

piCoefTop       .req  r0
piCoefBottom    .req  r1
CR_DCT          .req  r2
CI_DCT          .req  r3
CR1_DCT         .req  r4
CI1_DCT         .req  r5
STEP_DCT        .req  r6
CR2_DCT         .req  r4
CI2_DCT         .req  r5
iTi             .req  r9  
iTr             .req  r8
iBi             .req  r9
iBr             .req  r8
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

.set	iRegSpaceDCT        , 9*4   @ {r4 - r11, r14}

.set	iStackSpaceRevDCT   , 3*4    
.set	iOffset_FFTSize     , iStackSpaceRevDCT-4
.set	iOffset_nLog2cSB    , iStackSpaceRevDCT-8 
.set	iOffset_rgiCoef     , iStackSpaceRevDCT-12

.set	iOffset_CR1_DCT     , iRegSpaceDCT+iStackSpaceRevDCT
.set	iOffset_CI1_DCT     , iRegSpaceDCT+iStackSpaceRevDCT+4
.set	iOffset_STEP_DCT    , iRegSpaceDCT+iStackSpaceRevDCT+8
.set	iOffset_CR2_DCT     , iRegSpaceDCT+iStackSpaceRevDCT+12


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@	PRESERVE8
@    AREA    |.text|, CODE
_prvDctIV_ARM:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    stmfd   sp!, {r4 - r11, r14}
    sub     sp, sp, #iStackSpaceRevDCT  @setup stack

    mov     temp0_DCT, #1
    sub     temp1_DCT, r1, #2
    mov     i_DCT, temp0_DCT, LSL temp1_DCT             @ iFFTSize/2 = 1<<(nLog2cSB-2)@

    ldr     CR1_DCT, [sp, #iOffset_CR1_DCT]             @ get CR1
    ldr     CI1_DCT, [sp, #iOffset_CI1_DCT]             @ get CI1
    ldr     STEP_DCT, [sp, #iOffset_STEP_DCT]           @ get STEP

    mov     temp0_DCT, temp0_DCT, LSL r1                @ temp = 1<<nLog2cSB
    str     r1, [sp, #iOffset_nLog2cSB]

    sub     temp0_DCT, temp0_DCT, #1                    @ temp = (1<<nLog2cSB) - 1
    
    ldrd	iTr, [piCoefTop]							@ iTr = piCoefTop[0]@ temp5_DCT = piCoefTop[1]@
    add     piCoefBottom, piCoefTop, temp0_DCT, LSL #2  @ piCoefBottom = rgiCoef + (1<<nLog2cSB) - 1@
    
    str     i_DCT, [sp, #iOffset_FFTSize]
    str     piCoefTop, [sp, #iOffset_rgiCoef]

FirstDCTStageLoop:
		ldr     r14, [piCoefBottom]                     @ iBi = piCoefBottom[0]@ r14 is iBi here
    smmulr	temp1_DCT, CR_DCT, iTr       			@ MULT_BP1(CR,iTr)
    str			temp5_DCT, [piCoefBottom], #-8          @ piCoefBottom[0] = piCoefTop[1]@
    smmulr	temp2_DCT, CR_DCT, r14       			@ MULT_BP1(CR,iBi)
    subs    i_DCT, i_DCT, #1                        @ i --@	
    smmlsr	temp1_DCT, CI_DCT, r14, temp1_DCT		@ MULT_BP1(CI,iBi)
    
    mov     temp3_DCT, CR_DCT
    smmlar	temp2_DCT, CI_DCT, iTr, temp2_DCT		@ temp2 = MULT_BP1(CR,iBi) + MULT_BP1(CI,iTr)
    mov     temp1_DCT, temp1_DCT, LSL #1            @ temp1 = MULT_BP1(CR,iTr) - MULT_BP1(CI,iBi)

    smmul   temp5_DCT, STEP_DCT, CI_DCT  			@ MULT_BP1(STEP,CI)
    mov     temp2_DCT, temp2_DCT, LSL #1            @ temp2 = MULT_BP1(CR,iBi) + MULT_BP1(CI,iTr)@
    strd		temp1_DCT, [piCoefTop], #8              @ piCoefTop[0] = MULT_BP1(CR,iTr) - MULT_BP1(CI,iBi)@piCoefTop[1] = MULT_BP1(CR,iBi) + MULT_BP1(CI,iTr)@
    
    smmul   temp2_DCT, STEP_DCT, CR_DCT  			@ MULT_BP1(STEP,CR)
    sub     CR_DCT, CR1_DCT, temp5_DCT, LSL #1      @ CR = CR1 - MULT_BP1(STEP,CI)@
    mov     CR1_DCT, temp3_DCT                      @ CR1 = CR@
		ldrd		iTr, [piCoefTop]						@ iTr = piCoefTop[0]@ r9 = piCoefTop[1]@
	
    mov     temp3_DCT, CI_DCT
    add     CI_DCT, CI1_DCT, temp2_DCT, LSL #1      @ CI = CI1 + MULT_BP1(STEP,CR)@  
    mov     CI1_DCT, temp3_DCT                      @ CI1 = CI@

    bne     FirstDCTStageLoop


    ldr     i_DCT, [sp, #iOffset_FFTSize]

SecondDCTStageLoop:                                  
    smmulr   temp1_DCT, CR_DCT, iTr       			@ MULT_BP1(CR,iTr)
    mov     temp3_DCT, CR_DCT						@	

    smmulr   temp2_DCT, CR_DCT, iTi       			@ MULT_BP1(CR,iTi)
    subs    i_DCT, i_DCT, #1                        @ i --@
    
    smmlsr  temp1_DCT, CI_DCT, iTi, temp1_DCT		@ MULT_BP1(CR,iTr) + MULT_BP1(-CI,iTi)

    smmul   temp0_DCT, STEP_DCT, CR_DCT  			@ MULT_BP1(STEP,CR)
    
    smmlar	temp2_DCT, CI_DCT, iTr, temp2_DCT		@ MULT_BP1(CR,iTi) + MULT_BP1(CI,iTr)    
    mov     temp1_DCT, temp1_DCT, LSL #1            @ temp1 = MULT_BP1(CR,iTr) - MULT_BP1(CI,iTi)@
    
		smmul   temp5_DCT, STEP_DCT, CI_DCT  			@ MULT_BP1(STEP,CI)		
		mov     temp2_DCT, temp2_DCT, LSL #1            @ temp2 = MULT_BP1(CI,iTr) + MULT_BP1(CR,iTi)@
		strd		temp1_DCT, [piCoefTop], #8              @ piCoefTop[0] = MULT_BP1(CR,iTr) - MULT_BP1(CI,iTi)@piCoefTop[1] = MULT_BP1(CR,iTi) + MULT_BP1(CI,iTr)@
    
    mov     temp4_DCT, CI_DCT							
    
    sub     CR_DCT, CR1_DCT, temp5_DCT, LSL #1      @ CR = CR1 - MULT_BP1(STEP,CI)@
    mov     CR1_DCT, temp3_DCT                      @ CR1 = CR@
    
		ldrd		iTr, [piCoefTop]						@ iTr = piCoefTop[0]@ iTi = piCoefTop[1]@
    add     CI_DCT, CI1_DCT, temp0_DCT, LSL #1      @ CI = CI1 + MULT_BP1(STEP,CR)@
    
    mov     CI1_DCT, temp4_DCT                      @ CI1 = CI@

    bne     SecondDCTStageLoop

CallFFT:
@   prvFFT4DCT(NULL, rgiCoef, nLog2cSB - 1, FFT_FORWARD)@
		ldr     r2, [sp, #iOffset_nLog2cSB]
    ldr     r1, [sp, #iOffset_rgiCoef]    
    sub     r2, r2, #1
@   mov     r3, #0
    bl      _prvFFT4DCT

    ldr     temp1_DCT, [sp, #iOffset_nLog2cSB]
    ldr     piCoefTop, [sp, #iOffset_rgiCoef]

    mov     temp0_DCT, #1
    mov     temp0_DCT, temp0_DCT, LSL temp1_DCT         @ temp = 1<<nLog2cSB
    sub     temp0_DCT, temp0_DCT, #2                    @ temp = (1<<nLog2cSB) - 2
    add     piCoefBottom, piCoefTop, temp0_DCT, LSL #2  @ piCoefBottom = rgiCoef + (1<<nLog2cSB) - 2@
	
    mvn     CR_DCT, #2, 2                               @ CR = BP1_FROM_FLOAT(1)@
    mov     CI_DCT, #0   								@ CI = 0
                                   
		ldrd	iTr, [piCoefTop]							@iTr = piCoefTop[0]@ iTi = piCoefTop[1]@
    ldr     i_DCT, [sp, #iOffset_FFTSize]
    ldr     CR2_DCT, [sp, #iOffset_CR2_DCT]             @ get CR2

    mov     CI2_DCT, STEP_DCT, ASR #1                   @ DIV2 of STEP
    rsb     CI2_DCT, CI2_DCT, #0                        @ CI2  = -DIV2(STEP)@ 


ThirdDCTStageLoop:
    smmulr	temp1_DCT, CR_DCT, iTr       			@ MULT_BP1(CR,iTr)    
    smmulr	temp2_DCT, CI_DCT, iTr       			@ MULT_BP1(CI,iTr)    
    smmlsr	temp1_DCT, CI_DCT, iTi, temp1_DCT       @ MULT_BP1(CR,iTr) -  MULT_BP1(CI,iTi)@    
    smmlar	temp2_DCT, CR_DCT, iTi, temp2_DCT       @ MULT_BP1(CI,iTr) + MULT_BP1(CR,iTi)@
    mov     temp1_DCT, temp1_DCT, LSL #1            @ temp1 = MULT_BP1(CR,iTr) -  MULT_BP1(CI,iTi)@
    str     temp1_DCT, [piCoefTop], #4              @ piCoefTop[0] =  MULT_BP1(CR,iTr) -  MULT_BP1(CI,iTi)@
    rsb		temp2_DCT, temp2_DCT, #0				@ MULT_BP1(-CI,iTr) - MULT_BP1(CR,iTi)@
    subs    i_DCT, i_DCT, #1                        @ i --@
    smmulr	temp1_DCT, STEP_DCT, CI_DCT  			@ MULT_BP1(STEP,CI)@
    mov     temp2_DCT, temp2_DCT, LSL #1            @ temp2 = MULT_BP1(-CI,iTr) - MULT_BP1(CR,iTi)@
    ldrd	iBr, [piCoefBottom]						@ iBr@ iBi
    str     temp2_DCT, [piCoefBottom, #4]           @ piCoefBottom[1] =  MULT_BP1(-CI,iTr) - MULT_BP1(CR,iTi)@
    
    smmulr	temp2_DCT, STEP_DCT, CR_DCT  			@ MULT_BP1(STEP,CR)@
    mov     temp3_DCT, CR_DCT	 
    sub     CR_DCT, CR2_DCT, temp1_DCT, LSL #1      @ CR = CR2 - MULT_BP1(STEP,CI)@
    mov     CR2_DCT, temp3_DCT                      @ CR2 = CR@  
    mov     temp3_DCT, CI_DCT 

    smmulr	temp1_DCT, CR_DCT, iBr       			@ MULT_BP1(CR,iBr)
    add     CI_DCT, CI2_DCT, temp2_DCT, LSL #1      @ CI = CI2 + MULT_BP1(STEP,CR)@    
		smmulr	temp2_DCT, CR_DCT, iBi					@ MULT_BP1(CR,iBi)@
	
    smmlar	temp1_DCT, CI_DCT, iBi, temp1_DCT		@ MULT_BP1(CR,iBr) + MULT_BP1(CI,iBi)@
    mov     CI2_DCT, temp3_DCT                      @ CI2 = CI@
    
    smmlsr	temp2_DCT, CI_DCT, iBr, temp2_DCT		@ MULT_BP1(-CI,iBr) +  MULT_BP1(CR,iBi)@
    mov     temp1_DCT, temp1_DCT, LSL #1            @ temp1 = MULT_BP1(CR,iBr) + MULT_BP1(CI,iBi)@
    str     temp1_DCT, [piCoefTop], #4              @ piCoefTop[1] = MULT_BP1(CR,iBr) + MULT_BP1(CI,iBi)@
   
		mov     temp2_DCT, temp2_DCT, LSL #1            @ temp2 = MULT_BP1(-CI,iBr) +  MULT_BP1(CR,iBi)@
		ldrd	iTr, [piCoefTop]						@iTr = piCoefTop[0]@ iTi = piCoefTop[1]@
    str     temp2_DCT, [piCoefBottom], #-8          @ piCoefBottom[0] = MULT_BP1(-CI,iBr) +  MULT_BP1(CR,iBi)@
        
    bne     ThirdDCTStageLoop

EndOfprvDctIV_ARM:
    add     sp, sp, #iStackSpaceRevDCT  @ give back rev stack space
    ldmfd   sp!, {r4 - r11, PC}         @ prvDctIV_ARM
    @ENTRY_END   prvDctIV_ARM
    		
	.endif		@//WMA_OPT_FFT_ARM
	.endif		@//ARMVERSION
