;//*@@@+++@@@@******************************************************************
;//
;// Microsoft Windows Media
;// Copyright (C) Microsoft Corporation. All rights reserved.
;//
;//*@@@---@@@@******************************************************************

;// Module Name:
;//
;//     huffdec_arm.s
;//
;// Abstract:
;// 
;//     ARM Arch-4 specific multiplications
;//
;//      Custom build with 
;//          armasm $(InputDir)\$(InputName).s -o=$(OutDir)\$(InputName).obj 
;//      and
;//          $(OutDir)\$(InputName).obj
;// 
;// Author:
;// 
;//     Sil Sanders (sils) October 9, 2001
;//
;// Revision History:
;//
;//     Jerry He (yamihe) Dec 8, 2003 
;//
;//*************************************************************************


;// For ARM, each loop below is 11 cycles long as compiled and could be reduced to 8.  
;// 3 cycles per double bit at 196 kbps = 3*88000 = 294000 and 16% of 206MHz is 33MHz.
;// So savings would be 0.9% speedup and was obsevered to be .7% to 1.1% and some small improvement for all profiles.
;//
;// However, unrolling these loops leads to even faster (if larger) code, with improvements at all bitrates from 0.5% to 1.7%.
;// there are 71 instructions in the unrolled code vs 25 in the loop.  


  OPT         2       ; disable listing 
  INCLUDE     kxarm.h
  INCLUDE     wma_member_arm.inc
  INCLUDE	  wma_arm_version.h
  OPT         1       ; enable listing

 
  AREA    |.text|, CODE, READONLY

  IF WMA_OPT_INVERSQUAN_LOWRATE_ARM = 1

	IF LINUX_RIM = 1
	IMPORT  voWMAMemset
	ELSE
  	IMPORT  memset
  	ENDIF
  IMPORT  g_InverseFraction
  IMPORT  qstCalcQuantStep_ARM
  EXPORT  prvInverseQuantizeLowRate

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; WMARESULT prvInverseQuantizeLowRate (CAudioObject* pau, PerChannelInfo* ppcinfo, Int* rgiWeightFactor)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

r3pau                       rn  r3
r8pau                       rn  r8
r4ppcinfo                   rn  r4

OFFiRecon                   equ 0x00
OFFrgiCoefRecon             equ 0x04
OFFcQSIMWFracBits           equ 0x08
OFFcMaxWeightFracBits       equ 0x0C
OFFuiQuantStepXInvMaxWeight equ 0x10
OFFiShiftCoefQ              equ 0x14
OFFiBark                    equ 0x18
OFFrgbBandNotCoded_iBark    equ 0x18
OFFiCoefQ                   equ 0x1C
OffrgiNoisePower_iNoiseBand equ 0x1C
OFFuiInvMaxWeight           equ 0x20
OFFiBarkX4                  equ 0x24
OFFrgiCoefQ                 equ 0x28
OFFpffltSqrtBWRatio         equ 0x28
OFFpWF_pCoefRecon           equ 0x2C
OFFpiCoefQ                  equ 0x2C
OFFrgbBandNotCoded          equ 0x30
OFFrgffltSqrtBWRatio        equ 0x34
OFFrgiNoisePower            equ 0x38
OFFcFracBits                equ 0x38
OFFptRandState              equ 0x3C 
OFFffltNoisePower           equ 0x40
OFFffltNoisePower_iFracBits equ OFFffltNoisePower+FastFloat_iFracBits
OFFffltNoisePower_iFraction equ OFFffltNoisePower+FastFloat_iFraction
OFFiUBLimit                 equ 0x48
OFFcSubFrameSampleHalf      equ 0x4C

OFFLocalSize                equ 0x5c	;0x54
OFFLocalSaveReg             equ 10*4
OFFpau                      equ OFFLocalSize-4	;0+OFFLocalSaveReg+OFFLocalSize
OFFpWF                      equ OFFLocalSize-8	;0x54	;4+OFFLocalSaveReg+OFFLocalSize
;OFFpWF2						equ OFFLocalSize-4
INV_MAX_WEIGHT_FRAC_BITS    equ 30

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	IF LINUX_RVDS = 1
  	PRESERVE8
	ENDIF
  	AREA    |.text|, CODE
  	LEAF_ENTRY prvInverseQuantizeLowRate
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; 255  : {

    mov       r12, sp
;    stmfd     sp!, {r0, r2}  ; stmfd
    stmfd     sp!, {r4 - r12, lr}  ; stmfd
    sub       sp, sp, #OFFLocalSize 
|$M4302|
    mov       r3pau, r0
    str       r2, [sp, #OFFpWF]
    str		  r0, [sp, #OFFpau]
;    str		  r2, [sp, #OFFpWF2]
    mov       r4ppcinfo, r1


; 256  :    Int iBark = 0;
; 257  :    Int iCoefQ = 0;
; 258  :    Int iRecon = 0;
; 259  :    Int iShiftCoefQ = 20 + 11 - pau->m_iMaxEscSize;  // avoid overflow when |coefQ| can be >= 2048.

    ldr       r0, pCAudioObject_m_tRandState  
	ldrsh     r7, [r4ppcinfo, #PerChannelInfo_m_cSubFrameSampleHalf]

    ldr       r2, [r3pau, #CAudioObject_m_iMaxEscSize]
    add       r0, r3pau, r0
    str       r0, [sp, #OFFptRandState]
	str       r7, [sp, #OFFcSubFrameSampleHalf]

;   if (ppcinfo->m_wtMaxWeight <= 0)
;	   return TraceResult(WMA_E_BROKEN_FRAME);
	ldr       r0, [r4ppcinfo, #PerChannelInfo_m_wtMaxWeight]
	cmp       r0, #0
	ble       gBrokenFrame


; 260  :     UInt uiWeightFactor, uiMaxWeight, uiQuantStepXInvMaxWeight;
; 261  :     Int QuantStepXMaxWeightXWeightFactor, qrand, iNoise;
; 262  :    Int cQSIMWFracBits, cFracBits, cFracBits2, cWFFracBits, cMaxWeightFracBits;
; 263  :     Int *rgiCoefRecon = ppcinfo->m_rgiCoefRecon;
; 264  :     const I32* const rgiCoefQ                = ppcinfo->m_rgiCoefQ;
; 265  :     const U8* const rgbBandNotCoded          = ppcinfo->m_rgbBandNotCoded;

    ldr       r5, [r4ppcinfo, #PerChannelInfo_m_rgbBandNotCoded]
    mov       r7, #0
    rsb       r2, r2, #31
    ldr       r1, [r4ppcinfo, #PerChannelInfo_m_rgiCoefRecon]
    str       r5, [sp, #OFFrgbBandNotCoded]

; 266  :     const Int* const rgiNoisePower           = ppcinfo->m_rgiNoisePower;
; 267  :     const FastFloat* const rgffltSqrtBWRatio = ppcinfo->m_rgffltSqrtBWRatio;
; 268  :     U8  cNoiseBand;
; 269  :     Int iNoiseBand = 0;
; 270  :    const Int iDitherFactor = 0x51EB851F;       // LPC  0.04 * 2^35 
; 271  :    UInt uiInvMaxWeight = INVERSE_MAX_WEIGHT(ppcinfo->m_wtMaxWeight);

;    ldr       r0, [r4ppcinfo, #PerChannelInfo_m_wtMaxWeight] 
    str       r2, [sp, #OFFiShiftCoefQ]
    mov       r2, #2, 14                                        ; 0x80000
    ldr       r9, [r4ppcinfo, #PerChannelInfo_m_rgiCoefQ]
    cmp       r0, r2
    ldr       lr, [r4ppcinfo, #PerChannelInfo_m_rgiNoisePower]
    ldr       r2, [r4ppcinfo, #PerChannelInfo_m_rgffltSqrtBWRatio]
    str       r1, [sp, #OFFrgiCoefRecon]
    mov       r1, #13
;   str       r3pau, [sp, #OFFpau]
    mvncc     r1, #0
    str       r7, [sp, #OFFiBark]  
    str       r7, [sp, #OFFiCoefQ] 
    str       r7, [sp, #OFFiRecon]
    str       r9, [sp, #OFFrgiCoefQ]
    str       lr, [sp, #OFFrgiNoisePower]
    str       r2, [sp, #OFFrgffltSqrtBWRatio] 
    bcc       |$L4301| 
    ands      lr, r0, #0xF, 4                                   ; 0xF0000000
    bne       |$L4259|
|$L4017|
    mov       r0, r0, lsl #4
    sub       r1, r1, #4
    ands      r12, r0, #0xF, 4                                   ; 0xF0000000 
    beq       |$L4017|
|$L4259|
    ands      r2, r0, #2, 2                                     ; 0x80000000
    bne       |$L4020|
|$L4019|
    mov       r0, r0, lsl #1
    sub       r1, r1, #1
    ands      lr, r0, #2, 2                                     ; 0x80000000
    beq       |$L4019|
|$L4020|
    subs      r6, r1, #1
    mov       r2, r0, lsl #1
    mvnmi     r1, #0											; if(r1<0)r1=0xffffffff;
    bmi       |$L4301|
    ldr       r0, pg_InverseFraction
    mov       r1, r2, lsr #24
    ldr       r5, [r0, +r1, lsl #2]!
    ldr       r12, [r0, #4]
    mov       r0, r2, lsl #8
    sub       r1, r5, r12
   
    UMULL     R12, R0, R1, R0
    ;;; stall for issue = 2
    ldr       lr, [r3pau, #CAudioObject_m_qstQuantStep+4]
    ;;;
    sub       r2, r5, r0
    mov       r1, r2, lsr r6
|$L4301|

; 280  :     //Calculate QuantStep X invMaxWeight
; 281  :     cQSIMWFracBits  = pau->m_qstQuantStep.iFracBits;                           
; 282  :     uiQuantStepXInvMaxWeight = MULT_HI_DWORD(pau->m_qstQuantStep.iFraction,uiInvMaxWeight);
; 284  :    cQSIMWFracBits += (INV_MAX_WEIGHT_FRAC_BITS-32);
; 285  :    // Float QSIMWF = uiQuantStepXInvMaxWeight*1.0F/(1<<cQSIMWFracBits)
; 286  :    NormUInt( &uiQuantStepXInvMaxWeight, &cQSIMWFracBits, 0x3FFFFFFF );

    ldr       r5, [r3pau, #CAudioObject_m_qstQuantStep]
    SMULL     R12, R0, R1, lr
    ;;; stall for issue = 2
    str       r1, [sp, #OFFuiInvMaxWeight] 
    sub       r1, r5, #32-INV_MAX_WEIGHT_FRAC_BITS
    cmp       r0, #0
    str       r0, [sp, #OFFuiQuantStepXInvMaxWeight] 
    str       r1, [sp, #OFFcQSIMWFracBits]
    mvn       r11, #0xE, 4                      ; 0x1fffffff
    mvn       r2, #3, 2                         ; 0x3fffffff
    beq       |$L4025|
    cmp       r0, r11
    bcs       |$L4031|
|$L4030|
    mov       r0, r0, lsl #2
    add       r1, r1, #2
    cmp       r0, r11
    bcc       |$L4030| 
|$L4031|
    cmp       r0, r2
    movcc     r0, r0, lsl #1
    addcc     r1, r1, #1
    str       r0, [sp, #OFFuiQuantStepXInvMaxWeight] 
    str       r1, [sp, #OFFcQSIMWFracBits]
|$L4025|

; 290  :    uiMaxWeight = ppcinfo->m_wtMaxWeight<<MORE_WF_FRAC_BITS;;
; 291  :    cMaxWeightFracBits = MORE_WF_FRAC_BITS;     // really should be WEIGHTFACTOR_FRACT_BITS+MORE_WF_FRAC_BITS but this way is for shift delta
; 293  :    NormUInt( &uiMaxWeight, &cMaxWeightFracBits, 0x3FFFFFFF );

    ldr       r0, [r4ppcinfo, #PerChannelInfo_m_wtMaxWeight] 
    mov       r10, r7
    str       r10, [sp, #OFFcMaxWeightFracBits] 
    mov       r1, r7
    cmp       r0, #0
    beq       |$L4035|
    cmp       r0, r11
    bcs       |$L4041|
|$L4040|
    mov       r0, r0, lsl #2
    add       r1, r1, #2
    cmp       r0, r11
    bcc       |$L4040|
|$L4041|
    cmp       r0, r2
    addcc     r1, r1, #1
    mov       r10, r1
    str       r10, [sp, #OFFcMaxWeightFracBits] 
|$L4035|

; 294  : 
; 295  :     if (!pau->m_fNoiseSub) {

    ldr       r1, [r3pau, #CAudioObject_m_fNoiseSub]
    ldr       r2, [r3pau, #CAudioObject_m_cLowCutOff]
    cmp       r1, #0
    bne       |$LNotNoNoise|  ; 00000298

; 296  :        // Unusual case, but 8kHz Mono 8kpbs gets here
; 297  :         memset (rgiCoefRecon, 0, pau->m_cLowCutOff * sizeof (Int));

    mov       r1, #0
    mov       r2, r2, lsl #2
    ldr       r0, [sp, #OFFrgiCoefRecon]
    IF	LINUX_RIM = 1
    bl			voWMAMemset
    ELSE
    bl        memset
    ENDIF 
    ldr       r3pau, [sp, #OFFpau]
    ;;;

; 298  :         for (iRecon = pau->m_cLowCutOff; iRecon < pau->m_cHighCutOff; iRecon++, iCoefQ++)

    ldr       r7, [r3pau, #CAudioObject_m_cLowCutOff]
    ldr       lr, [r3pau, #CAudioObject_m_cHighCutOff]
    ;;;
    cmp       r7, lr
    bge       |$Loop1NoSubx|
    ldr       r0, [sp, #OFFrgiCoefRecon]
    mov       r6, r9
    ldr       r12, [sp, #OFFpWF]
    add       r8, r10, #21
    add       r5, r0, r7, lsl #2
    sub       r9, r12, r0
|$Loop1NoSub|

; 299  :        {   // rgfltCoefRecon [iRecon] = Float ((Float) (rgiCoefQ [iCoefQ]) * rgfltWeightFactor [iRecon] * dblQuantStep * fltInvMaxWeight);
; 300  :            int iCoef;
; 302  :            uiWeightFactor = rgiWeightFactor [iRecon] << cMaxWeightFracBits; 
; 303  :            cWFFracBits = WEIGHTFACTOR_FRACT_BITS+cMaxWeightFracBits;
; 304  :            //// Float WeightFactor = uiWeightFactor*1.0F/(1<<cWFFracBits)
; 306  :            NormUInt( &uiWeightFactor, &cWFFracBits, 0x3FFFFFFF );      

    ldr       r2, [r9, +r5]
    mov       r4, r8
    movs      r0, r2, lsl r10
    beq       |$L4045|
    cmp       r0, r11
    bcs       |$L4051|
|$L4050|
    mov       r0, r0, lsl #2
    add       r4, r4, #2
    cmp       r0, r11
    bcc       |$L4050|
|$L4051|
    mvn       r1, #3, 2                                 ; 0x3fffffff
    cmp       r0, r1
    movcc     r0, r0, lsl #1
    addcc     r4, r4, #1
|$L4045|

; 309  :            QuantStepXMaxWeightXWeightFactor = MULT_HI(uiQuantStepXInvMaxWeight,uiWeightFactor);        
; 310  :            cFracBits = cQSIMWFracBits+cWFFracBits-31;
; 311  :            //// Float QuantStep*WeightFactor/InvMaxWeight = QuantStepXMaxWeightXWeightFactor/(1.0F*(1<<cFracBits))

    ldr       r1, [sp, #OFFuiQuantStepXInvMaxWeight] 
;   ldrsh     r2, [r6]
	ldr       r2, [r6]
    SMULL     r12, r1, r0, r1
    ;;; stall for issue = 2


; 315  :             iCoef = MULT_HI( (Int)rgiCoefQ[iCoefQ]<<iShiftCoefQ, QuantStepXMaxWeightXWeightFactor );
; 316  :            cFracBits += (iShiftCoefQ-31);
; 317  :            //// Float Coef = iCoef/(1.0F*(1<<cFracBits))

    ldr       lr, [sp, #OFFiShiftCoefQ]  
    MOV       r1,  r1, lsl #1
    mov       r2, r2, lsl lr
    SMULL     r12, r0, r1, r2
    ;;; stall for issue = 2
    ldr       r2, [sp, #OFFcQSIMWFracBits]
    ldr       r1, [sp, #OFFiShiftCoefQ]  
    MOV       r0,  r0, lsl #1


; 319  :            rgiCoefRecon[iRecon] = Align2FracBits( iCoef, cFracBits, TRANSFORM_FRACT_BITS );
; 320  :            //// Float CoefRecon = rgiCoefRecon[iRecon]/32.0F

    add       r2, r4, r2
    add       lr, r2, r1
    subs      r1, lr, #2*31+TRANSFORM_FRACT_BITS
    rsbmi     r12, r1, #0
    movmi     r0, r0, lsl r12
    bmi       |$L4060|
    cmp       r1, #32
    mov       r0, r0, asr r1
    movge     r0, #0
|$L4060|
    add       r7, r7, #1
    str       r0, [r5]
;   add       r6, r6, #2
    add       r6, r6, #4
    ldr       r1, [r3pau, #CAudioObject_m_cHighCutOff] 
    add       r5, r5, #4
    cmp       r7, r1
    blt       |$Loop1NoSub|
|$Loop1NoSubx|

; 323  :        }
; 324  :         memset (rgiCoefRecon + pau->m_cHighCutOff, 0, (pau->m_cSubband - pau->m_cHighCutOff) * sizeof (Int));

    mov       r1, #0
    ldr       r0, [r3pau, #CAudioObject_m_cHighCutOff] 
;   ldrsh     r2, [r3pau, #PerChannelInfo_m_cSubFrameSampleHalf]
	ldr       r2, [sp, #OFFcSubFrameSampleHalf]
    ldr       lr, [sp, #OFFrgiCoefRecon]
    sub       r2, r2, r0
    mov       r2, r2, lsl #2
    add       r0, lr, r0, lsl #2
    
    IF	LINUX_RIM = 1
    bl			voWMAMemset
    ELSE
    bl        memset
    ENDIF 
    
    ldr       r3pau, [sp, #OFFpau]

; 571  :        }
; 572  :    }
; 579  :    FUNCTION_PROFILE_STOP(&fp);
; 580  :    return WMA_OK;

    mov       r0, #0
    add       sp, sp, #OFFLocalSize  
    ldmia     sp, {r4 - r11, sp, pc}  ; ldmfd

; 581  : }

pg_InverseFraction  DCD       |g_InverseFraction|

|$LNotNoNoise|

; 329  :     cNoiseBand = rgbBandNotCoded [0];
; 331  :     if (iRecon < pau->m_cLowCutOff ) {

    ldr       r9, pciRandA    
    ldr       r0, [r3pau, #CAudioObject_m_cLowCutOff] 
    ldr       r8, pciRandC  
    cmp       r0, #0
    ble       |$Loop2LowCutOffx|

; 332  :         uiWeightFactor = rgiWeightFactor [pau->m_cLowCutOff] << cMaxWeightFracBits; 
; 333  :         cWFFracBits = WEIGHTFACTOR_FRACT_BITS+cMaxWeightFracBits;
; 334  :        NormUInt( &uiWeightFactor, &cWFFracBits, 0x3FFFFFFF );      // weightFactor with cWFFracBits fractional bits

    ldr       r2, [sp, #OFFpWF]
    add       r4, r10, #WEIGHTFACTOR_FRACT_BITS
    ldr       r1, [r2, +r0, lsl #2]
    ;;;
    movs      r0, r1, lsl r10
    beq       |$L4064|
    cmp       r0, r11
    bcs       |$L4070|
|$L4069|
    mov       r0, r0, lsl #2
    add       r4, r4, #2
    cmp       r0, r11
    bcc       |$L4069|
|$L4070|
    mvn       lr, #3, 2                             ; 0x3fffffff
    cmp       r0, lr
    movcc     r0, r0, lsl #1
    addcc     r4, r4, #1
|$L4064|

; 336  :         QuantStepXMaxWeightXWeightFactor = MULT_HI(uiQuantStepXInvMaxWeight,uiWeightFactor);       
; 337  :         cFracBits = cQSIMWFracBits+cWFFracBits-31;
; 338  :         //// Float QuantStep*WeightFactor/MaxWeight = QuantStepXMaxWeightXWeightFactor/(1.0F*(1<<cFracBits))

    ldr       r2, [sp, #OFFuiQuantStepXInvMaxWeight] 
    ;;;
    SMULL     r12, r6, r2, r0
    ;;; stall for issue = 2
    ldr       r2, [sp, #OFFcQSIMWFracBits]
    ldr       lr, [r3pau, #CAudioObject_m_cLowCutOff]
    MOV       r6,  r6, lsl #1

; 340  :        while  (iRecon < pau->m_cLowCutOff)     {

    add       r2, r4, r2
    sub       r0, r2, #31
    cmp       lr, #0
    ble       |$Loop2LowCutOffx| 
    ldr       r7, [sp, #OFFrgiCoefRecon]
    ldr       r10, [sp, #OFFiRecon]
    ldr       r4, [sp, #OFFptRandState]
    sub       r5, r0, #3
|$Loop2LowCutOff|

; 341  :             int iNoiseScaled,iNoiseQuant;         
; 342  :             qrand = quickRand (&(pau->m_tRandState));

    ldr       r2, [r4, #tagRandState_uiRand]
    ldr       r12, [r4, #tagRandState_iPrior]
    mul       r1, r2, r9
    ldr       r2, pciDitherFactor
    ;;;
    add       r0, r1, r8
    mov       lr, r0, asr #4
    str       r0, [r4, #tagRandState_uiRand]
    add       r0, lr, r0, asr #2
    sub       r1, r0, r12

; 343  :             iNoise = MULT_HI(iDitherFactor,qrand);
; 344  :            cFracBits2 = 35+29-31;                                          // == 33
; 345  :            //// Float Noise = iNoise/(1024.0F*(1<<(cFracBits2-10)))

    SMULL     r12, r2, r1, r2
    ;;; stall for issue = 2
    str       r0, [r4, #tagRandState_iPrior]
    ;;;
    MOV       r2,  r2, lsl #1


; 346  :             iNoiseQuant = MULT_HI(iNoise,QuantStepXMaxWeightXWeightFactor);
; 347  :            cFracBits2 += (cFracBits-31);
; 348  :            //// Float NoiseQuant = iNoiseQuant/(1024.0F*(1<<(cFracBits2-10)))

    SMULL     r12, r0, r6, r2
    ;;; stall for issue = 2
    cmp       r5, #0
    rsbmi     r2, r5, #0
    MOV       r0,  r0, lsl #1


; 350  :             //rescale iNoiseQuant so that (float)iNoiseScaled/(1<<TRANSFORM_FRACT_BITS)
; 351  :            iNoiseScaled = Align2FracBits( iNoiseQuant, cFracBits2, TRANSFORM_FRACT_BITS );
; 352  :            //// Float NoiseScaled = iNoiseScaled/32.0F

    movmi     r0, r0, lsl r2
    bmi       |$L4087|
    cmp       r5, #32
    mov       r0, r0, asr r5
    movge     r0, #0
|$L4087|

; 354  :             rgiCoefRecon [iRecon] = iNoiseScaled;
; 358  :            iRecon++;

    str       r0, [r7]
    add       r10, r10, #1
    ldr       r1, [r3pau, #CAudioObject_m_cLowCutOff] 
    add       r7, r7, #4
    cmp       r10, r1
    blt       |$Loop2LowCutOff|
    str       r10, [sp, #OFFiRecon]
|$Loop2LowCutOffx|

; 359  :        }
; 360  :    }

; 362  :    while  (iRecon < pau->m_iFirstNoiseIndex) {

r4cWFFracBits   rn  r4
r5QuantStepXMaxWeightXWeightFactor  rn  r5
r6iShiftCoefQ   rn  r6
r6iCoefScaled   rn  r6
r8piCoefRecon   rn  r8
r9qrand         rn  r9
r10prgiCoefQ    rn  r10

r7uiRand        rn  r7
r11iPrior       rn  r11

    ldr       r12, [sp, #OFFiRecon]
    ldr       lr, [r3pau, #CAudioObject_m_iFirstNoiseIndex]
    ldr       r10prgiCoefQ, [sp, #OFFrgiCoefQ]
    cmp       r12, lr
    bge       |$Loop3BelowFirstNoiseX|
    mov       lr, r12
    ldr       r12, [sp, #OFFpWF]
    ldr       r0, [sp, #OFFrgiCoefRecon]
    sub       r2, r12, r0
    add       r8piCoefRecon, r0, lr, lsl #2
    ldr       lr, [sp, #OFFptRandState]
    str       r2, [sp, #OFFpWF_pCoefRecon]
    ldr       r7uiRand, [lr, #tagRandState_uiRand]
    ldr       r11iPrior, [lr, #tagRandState_iPrior]

|$Loop3BelowFirstNoise|

; 363  :         int iCoef,iCoefScaled,iCoefRecon,iNoiseScaled,iNoiseQuant;         
; 365  :         qrand = quickRand (&(pau->m_tRandState));
; 367  :         if (iRecon >= pau->m_rgiBarkIndex [iBark + 1]) 
; 368  :            iBark++;
; 374  :         uiWeightFactor = rgiWeightFactor [iRecon] << cMaxWeightFracBits; 
; 375  :         cWFFracBits = WEIGHTFACTOR_FRACT_BITS+cMaxWeightFracBits;
; 376  :        //// Float WeightFactor = uiWeightFactor/(1.0F*(1<<cWFFracBits))
; 380  :        NormUInt( &uiWeightFactor, &cWFFracBits, 0x3FFFFFFF );      // weightFactor with cWFFracBits fractional bits

    ldr       r0, pciRandA
    ldr       r12, pciRandC
    ldr       r4cWFFracBits, [sp, #OFFcMaxWeightFracBits] 
    mul       lr, r7uiRand, r0
    add       r4cWFFracBits, r4cWFFracBits, #WEIGHTFACTOR_FRACT_BITS
    mov       r0, r11iPrior
    add       r7uiRand, lr, r12
    mov       r11iPrior, r7uiRand, asr #4
    add       r11iPrior, r11iPrior, r7uiRand, asr #2
    sub       r9qrand, r11iPrior, r0

    ldr       lr, [r3pau, #CAudioObject_m_rgiBarkIndex]
    ldr       r0, [sp, #OFFiBark] 
    ldr       r1, [sp, #OFFiRecon]
    add       r12, lr, r0, lsl #2
    ldr       lr, [sp, #OFFpWF_pCoefRecon]  
    ldr       r2, [r12, #4]
    ldr       r12, [r8piCoefRecon, +lr]                     ; pWF[iRecon]
    cmp       r1, r2
    ldr       r2, [sp, #OFFcMaxWeightFracBits] 
    addge     r0, r0, #1
    strge     r0, [sp, #OFFiBark] 
    movs      r0, r12, lsl r2
    beq       |$L4100|
    mvn       r1, #0xE, 4                           ; 0x1fffffff
    cmp       r0, r1
    bcs       |$L4106|
|$L4105|
    mov       r0, r0, lsl #2
    add       r4cWFFracBits, r4cWFFracBits, #2
    cmp       r0, r1
    bcc       |$L4105|
|$L4106|
    mvn       r1, #3, 2                             ; 0x3fffffff
    cmp       r0, r1
    movcc     r0, r0, lsl #1
    addcc     r4cWFFracBits, r4cWFFracBits, #1
|$L4100|

; 383  :         QuantStepXMaxWeightXWeightFactor = MULT_HI(uiQuantStepXInvMaxWeight,uiWeightFactor);       
; 384  :         cFracBits = cQSIMWFracBits+cWFFracBits-31;
; 385  :         //// Float QuantStep*WeightFactor/MaxWeight = QuantStepXMaxWeightXWeightFactor/(1.0F*(1<<cFracBits))

    ldr       r1, [sp, #OFFuiQuantStepXInvMaxWeight]  
    ldr       r6iShiftCoefQ, [sp, #OFFiShiftCoefQ] 
;   ldrsh     r2, [r10prgiCoefQ]
	ldr       r2, [r10prgiCoefQ]
    SMULL     r12, r5QuantStepXMaxWeightXWeightFactor, r1, r0
    ;;; stall for issue = 2
    ldr       lr, [sp, #OFFcQSIMWFracBits]
    mov       r2, r2, lsl r6iShiftCoefQ
    MOV       r5QuantStepXMaxWeightXWeightFactor,  r5QuantStepXMaxWeightXWeightFactor, lsl #1

; 389  :         iCoef = MULT_HI( (Int)rgiCoefQ[iCoefQ]<<iShiftCoefQ, QuantStepXMaxWeightXWeightFactor );
; 390  :        cFracBits += (iShiftCoefQ-31);
; 391  :        //// Float Coef = iCoef/(1.0F*(1<<cFracBits))

    SMULL     r12, r0, r5QuantStepXMaxWeightXWeightFactor, r2
    ;;; stall for issue = 2
    add       r2, r4cWFFracBits, lr
    add       r2, r2, r6iShiftCoefQ
    MOV       r0,  r0, lsl #1


; 394  :        iCoefScaled = Align2FracBits( iCoef, cFracBits, TRANSFORM_FRACT_BITS );
; 395  :        //// Float CoefScaled = iCoefScaled/32.0F

    subs      r1, r2, #2*31+TRANSFORM_FRACT_BITS           ; TODO - express this as a sum of constants
    rsbmi     r1, r1, #0
    movmi     r6iCoefScaled, r0, lsl r1
    bmi       |$L4115|
    cmp       r1, #32
    mov       r6iCoefScaled, r0, asr r1
    movge     r6iCoefScaled, #0
|$L4115|

; 397  :         iNoise = MULT_HI(iDitherFactor,qrand);
; 398  :        cFracBits2 = 35+29-31;                                          // == 33
; 399  :        //// Float Noise = iNoise/(1024.0F*(1<<(cFracBits2-10)))

    ldr       r0, pciDitherFactor
    ldr       lr, [sp, #OFFcQSIMWFracBits]
    SMULL     r12, r0, r9qrand, r0
    ;;; stall for issue = 2
    add       lr, r4cWFFracBits, lr
    subs      r1, lr, #29+TRANSFORM_FRACT_BITS
    MOV       r0, r0, lsl #1


; 400  :         iNoiseQuant = MULT_HI(iNoise,QuantStepXMaxWeightXWeightFactor);
; 401  :        cFracBits2 += ((cQSIMWFracBits+cWFFracBits-31)-31);
; 402  :        //// Float NoiseQuant = iNoiseQuant/(1024.0F*(1<<(cFracBits2-10)))

    SMULL     r12, r0, r5QuantStepXMaxWeightXWeightFactor, r0
    ;;; stall for issue = 2

; 406  :        iNoiseScaled = Align2FracBits( iNoiseQuant, cFracBits2, TRANSFORM_FRACT_BITS );
; 407  :        //// Float NoiseScaled = iNoiseScaled/32.0F

    rsbmi     r2, r1, #1
    movmi     r0, r0, lsl r2
    bmi       |$L4124|
    MOV       r0,  r0, lsl #1
    cmp       r1, #32
    mov       r0, r0, asr r1
    movge     r0, #0
|$L4124|

; 408  : 
; 409  :         iCoefRecon = iCoefScaled + iNoiseScaled;
; 411  :         rgiCoefRecon [iRecon] = iCoefRecon;
; 412  :        //// Float CoefRecon = rgiCoefRecon [iRecon]/32.0F
; 416  :         iRecon++;
; 417  :        iCoefQ++;

    add       r1, r0, r6iCoefScaled
    ldr       r0, [sp, #OFFiRecon]
    ldr       lr, [sp, #OFFiCoefQ] 
;   add       r10prgiCoefQ, r10prgiCoefQ, #2
    add       r10prgiCoefQ, r10prgiCoefQ, #4
    add       r0, r0, #1
    str       r1, [r8piCoefRecon], #+4
    str       r0, [sp, #OFFiRecon]
    add       lr, lr, #1
    ldr       r1, [r3pau, #CAudioObject_m_iFirstNoiseIndex] 
    str       lr, [sp, #OFFiCoefQ] 
    cmp       r0, r1
    blt       |$Loop3BelowFirstNoise|

    ldr       lr, [sp, #OFFptRandState]
    ;;;
    str       r7uiRand, [lr, #tagRandState_uiRand]
    str       r11iPrior, [lr, #tagRandState_iPrior]

|$Loop3BelowFirstNoiseX|

; 418  :     }
; 419  : 
; 420  :     while  (iRecon < pau->m_cHighCutOff) {

r7iRecon        rn  r7

    ldr       lr, [sp, #OFFiRecon]
    ldr       r1, [r3pau, #CAudioObject_m_cHighCutOff]
    ldr       r0, [sp, #OFFiBark]  
    cmp       lr, r1
    bge       |$Loop4NoiseRegionX|  ; 00000918
    ldr       r12, [sp, #OFFrgbBandNotCoded]  
    mov       r11, r0, lsl #2
    ldr       r2, [sp, #OFFrgiCoefQ] 
    add       r8, r12, r0
    ldr       r0, [sp, #OFFiCoefQ]  
    ldr       r6, [sp, #OFFrgffltSqrtBWRatio] 
    ldr       lr, [sp, #OFFrgiNoisePower]  
;   add       r10, r2, r0, lsl #1
	add       r10, r2, r0, lsl #2
    str       r11, [sp, #OFFiBarkX4]  
    str       r8, [sp, #OFFrgbBandNotCoded_iBark]  
    str       r10, [sp, #OFFpiCoefQ] 
    str       r6, [sp, #OFFpffltSqrtBWRatio]  
    str       lr, [sp, #OffrgiNoisePower_iNoiseBand] 
|$Loop4NoiseRegion|

; 421  :        if (iRecon >= pau->m_rgiBarkIndex [iBark + 1]) 
; 422  :            iBark++;

    ldr       r0, [r3pau, #CAudioObject_m_rgiBarkIndex] 
    ldr       r7iRecon, [sp, #OFFiRecon]
    add       r12, r11, r0
    ldr       lr, [r12, #4]
    ;;;
    cmp       r7iRecon, lr
    blt       |$L3565|
    add       r8, r8, #1
    add       r11, r11, #4
    str       r8, [sp, #OFFrgbBandNotCoded_iBark]  
    str       r11, [sp, #OFFiBarkX4] 
|$L3565|

; 425  :        if (rgbBandNotCoded [iBark] == 1)

    ldrb      r12, [r8]
    ;;;
    cmp       r12, #1
    bne       |$LBandIsCoded|

; 426  :         {
; 427  :            FastFloat ffltNoisePower;
; 428  :            UInt uiNoisePowerXinvMaxWeight;
; 429  :            Int iUBLimitOniRecon = min(pau->m_rgiBarkIndex [iBark + 1], pau->m_cHighCutOff);
; 433  :            qstCalcQuantStep( &ffltNoisePower, rgiNoisePower[iNoiseBand] );
; 434  :            //// Float Noise Power = ffltNoisePower.iFraction/(1.0F*(1<<ffltNoisePower.iFracBits))

    add       r2, r11, r0
    ldr       r4, [r2, #4]
    ;;;
    cmp       r4, r1
    movge     r4, r1

    ldr       r1, [sp, #OffrgiNoisePower_iNoiseBand]  
    add       r0, sp, #OFFffltNoisePower  
    ldr       r1, [r1]
    bl        qstCalcQuantStep_ARM
    ldr       r3pau, [sp, #OFFpau]

; 436  :            ffltMultiply( &ffltNoisePower, &ffltNoisePower, &rgffltSqrtBWRatio[iNoiseBand] );
; 437  :            //// Float Noise Power = ffltNoisePower.iFraction/(1.0F*(1<<ffltNoisePower.iFracBits))

    ldr       r1, [r6, #4]
    ldr       r0, [sp, #OFFffltNoisePower_iFraction] 
    ldr       r2, [sp, #OFFffltNoisePower_iFracBits] 
    SMULL     r12, r0, r1, r0
    ;;; stall for issue = 2
    mvn       r5, #0xE, 4                   ; 0x1FFFFFFF
    ldr       lr, [r6]
    MOV       r0, r0, lsl #1
    str       r0, [sp, #OFFffltNoisePower_iFraction] 
    mov       r1, r0
    add       r2, lr, r2
    movs      lr, r1, asr #31
    sub       r12, r2, #31
    eor       r2, r1, lr
    rsbs      r1, lr, r2
    moveq     r1, #0
    mov       r2, #0
    streq     r1, [sp, #OFFffltNoisePower_iFracBits]  
    beq       |$L4138|
    cmp       r1, r5
    bcs       |$L4143|
|$L4142|
    mov       r1, r1, lsl #2
    add       r2, r2, #2
    cmp       r1, r5
    bcc       |$L4142|
|$L4143|
    mvn       lr, #3, 2                     ; 0x3fffffff
    cmp       r1, lr
    addcc     r2, r2, #1
    mov       r0, r0, lsl r2
    add       r12, r2, r12
    str       r0, [sp, #OFFffltNoisePower_iFraction]  
    str       r12, [sp, #OFFffltNoisePower_iFracBits] 
|$L4138|

; 439  :             uiNoisePowerXinvMaxWeight = MULT_HI(ffltNoisePower.iFraction,uiInvMaxWeight>>1)<<1;
; 440  :            cFracBits = ffltNoisePower.iFracBits + (INV_MAX_WEIGHT_FRAC_BITS-31);
; 441  :            //// Float NoisePower/MaxWeight = uiNoisePowerXinvMaxWeight/(1.0F*(1<<cFracBits))
; 442  :            NormUInt( &uiNoisePowerXinvMaxWeight, &cFracBits, 0x3FFFFFFF );

    ldr       r2, [sp, #OFFuiInvMaxWeight] 
    ldr       r1, [sp, #OFFffltNoisePower_iFracBits] 
    mov       r2, r2, lsr #1
    SMULL     r12, r9, r2, r0
    ;;; stall for issue = 2
    sub       r1, r1, #31-INV_MAX_WEIGHT_FRAC_BITS
    ;;;
    movs      r9, r9, lsl #2
    mov       r0, r9
    beq       |$L4271|
    cmp       r9, r5
    bcs       |$L4154|
|$L4153|
    mov       r0, r0, lsl #2
    add       r1, r1, #2
    cmp       r0, r5
    bcc       |$L4153|
|$L4154|
    mvn       lr, #3, 2                     ; 0x3fffffff
    cmp       r0, lr
    movcc     r0, r0, lsl #1
    addcc     r1, r1, #1
    mov       r9, r0
|$L4271|

; 443  : 
; 444  :             while (iRecon < iUBLimitOniRecon)

    cmp       r7iRecon, r4
    bge       |$Loop5NoiseSubX|
    ldr       r0, [sp, #OFFcMaxWeightFracBits] 
    sub       r1, r1, #2
    add       r7, r0, #21
    ldr       r0, [sp, #OFFiRecon]
    str       r1, [sp, #OFFcFracBits]  
    sub       r10, r4, r0
    ldr       r1, [sp, #OFFrgiCoefRecon]
    ldr       lr, [sp, #OFFpWF]
    ldr       r5, [sp, #OFFptRandState]
    add       r12, r10, r0
    ldr       r11, pciRandC
    add       r6, r1, r0, lsl #2
    str       r12, [sp, #OFFiRecon]
    sub       r8, lr, r1

r2uiRand        rn  r2
r5iPrior        rn  r5

    ldr       r2uiRand, [r5, #tagRandState_uiRand]
    ldr       r5iPrior, [r5, #tagRandState_iPrior]

|$Loop5NoiseSub|

; 445  :             {
; 446  :                 Int iNoiseRand,iNoiseWeighted,iCoefRecon;
; 447  : 
; 448  :                 qrand = quickRand (&(pau->m_tRandState));                                      // FB = 29                                              

    ldr       r1, pciRandA
    ldr       lr, [r6, +r8]
    mul       r2uiRand, r1, r2uiRand
    mov       r0, r5iPrior
    ldr       r1, [sp, #OFFcMaxWeightFracBits]
    add       r2uiRand, r2uiRand, r11
    mov       r5iPrior, r2uiRand, asr #4
    add       r5iPrior, r5iPrior, r2uiRand, asr #2
    sub       r0, r5iPrior, r0

; 449  :                 iNoiseRand = MULT_HI(uiNoisePowerXinvMaxWeight,qrand);
; 450  :                 cFracBits2 = cFracBits+29-31;
; 451  :                //// Float NoiseRand = iNoiseRand/(1.0F*(1<<cFracBits2))

    SMULL     r12, r0, r9, r0
    ;;; stall for issue = 2
    mov       r4, r7
    movs      r1, lr, lsl r1

; 456  :                uiWeightFactor = rgiWeightFactor [iRecon] << cMaxWeightFracBits; 
; 457  :                cWFFracBits = WEIGHTFACTOR_FRACT_BITS+cMaxWeightFracBits;
; 458  :                //// Float WeightFactor =   uiWeightFactor/(1024.0F*(1<<(cWFFracBits-10)))
; 462  :                NormUInt( &uiWeightFactor, &cWFFracBits, 0x3FFFFFFF );      // uiWeightFactor with cWFFracBits fractional bits

    MOV       r0,  r0, lsl #1
    beq       |$L4166_| 
    mvn       lr, #0xE, 4                       ; 0x1fffffff
    cmp       r1, lr
    bcs       |$L4172|
|$L4171_|
    mov       r1, r1, lsl #2
    add       r4, r4, #2
    cmp       r1, lr
    bcc       |$L4171_|
|$L4172|
    mvn       lr, #3, 2                         ; 0x3fffffff
    cmp       r1, lr
    movcc     r1, r1, lsl #1
    addcc     r4, r4, #1
|$L4166_|

; 465  :                 iNoiseWeighted = MULT_HI(iNoiseRand,uiWeightFactor);   
; 466  :                cFracBits2 += (cWFFracBits-31);
; 467  :                //// Float NoiseWeighted = iNoiseWeighted/(1024.0F*(1<<(cFracBits2-10)))

    SMULL     r12, r0, r1, r0
    ;;; stall for issue = 2
    ldr       lr, [sp, #OFFcFracBits] 
    ;;;
    add       lr, lr, r4
    MOV       r0,  r0, lsl #1

; 469  :                iCoefRecon = Align2FracBits( iNoiseWeighted, cFracBits2, TRANSFORM_FRACT_BITS );  //scale so that (float)iCoefRecon/(1<<TRANSFORM_FRACT_BITS)
; 470  :                 rgiCoefRecon [iRecon] = iCoefRecon;
; 471  :                //// Float CoefRecon = rgiCoefRecon [iRecon]/32.0F
; 475  :                iRecon++;   

    subs      r1, lr, #31+TRANSFORM_FRACT_BITS
    rsbmi     r1, r1, #0
    movmi     r0, r0, lsl r1
    bmi       |$L4181|
    cmp       r1, #32
    mov       r0, r0, asr r1
    movge     r0, #0
|$L4181|
    subs      r10, r10, #1
    str       r0, [r6], #+4
    bhi       |$Loop5NoiseSub|

    ldr       lr, [sp, #OFFptRandState]
    ldr       r10, [sp, #OFFpiCoefQ] 
    str       r2uiRand, [lr, #tagRandState_uiRand]
    str       r5iPrior, [lr, #tagRandState_iPrior]

    ldr       r11, [sp, #OFFiBarkX4] 
    ldr       r6, [sp, #OFFpffltSqrtBWRatio] 
    ldr       r8, [sp, #OFFrgbBandNotCoded_iBark] 
|$Loop5NoiseSubX|

; 476  :            }
; 477  : 
; 478  :            iNoiseBand++;

    ldr       r0, [sp, #OffrgiNoisePower_iNoiseBand] 
    add       r6, r6, #8
    str       r6, [sp, #OFFpffltSqrtBWRatio]
    add       lr, r0, #4
    str       lr, [sp, #OffrgiNoisePower_iNoiseBand] 
    b         |$Loop4NoiseRegionEpilog|

; 479  :        }
; 480  :        else 

pciRandA        DCD       0x19660d      ; const U32 a = 1664525;
pciRandC        DCD       0x3c6ef35f    ; const U32 c = 1013904223;
pciDitherFactor DCD       0x51eb851f    ; const Int iDitherFactor = 0x51EB851F;     // LPC  0.04 * 2^35 
pCAudioObject_m_tRandState  DCD CAudioObject_m_tRandState
pWMA_E_BROKEN_FRAME         DCD 0x80040002
    
        
|$LBandIsCoded|

r6uiRand        rn  r6
r8iPrior        rn  r8
r11qrand        rn  r11

; 481  :        {   // This should be the same as the first < FirstNoiseIndex loop
; 482  :            // Float fltNoise = pau->m_fltDitherLevel * ((Float) quickRand (&(pau->m_tRandState)) / (Float) 0x20000000);
; 483  :            // rgfltCoefRecon [iRecon] = (Float) ((rgiCoefQ [iCoefQ] + fltNoise) * rgfltWeightFactor[iRecon] * dblQuantStep * fltInvMaxWeight);         
; 484  : 
; 485  :            Int iCoef,iNoiseQuant,iCoefScaled,iCoefRecon;
; 429  :            Int iUBLimitOniRecon = min(pau->m_rgiBarkIndex [iBark + 1], pau->m_cHighCutOff);

    add       r2, r11, r0
    ldr       r2, [r2, #4]
    ldr       r0, [sp, #OFFptRandState]
    cmp       r2, r1
    movge     r2, r1

    str       r2, [sp, #OFFiUBLimit]
    ldr       r8iPrior, [r0, #tagRandState_iPrior]
    ldr       r6uiRand, [r0, #tagRandState_uiRand]
    

|$Loop7BandIsCoded|

; 491  :            qrand = quickRand (&(pau->m_tRandState));

    ldr       r2, pciRandA  
    ldr       lr, pciRandC 
    mul       r6uiRand, r2, r6uiRand
    mov       r12, r8iPrior
    ldr       r1, [sp, #OFFpWF]
    add       r6uiRand, r6uiRand, lr
    mov       r8iPrior, r6uiRand, asr #4
    add       r8iPrior, r8iPrior, r6uiRand, asr #2
    sub       r11qrand, r8iPrior, r12

; 494  :            uiWeightFactor = rgiWeightFactor [iRecon] << cMaxWeightFracBits; 
; 495  :            cWFFracBits = WEIGHTFACTOR_FRACT_BITS+cMaxWeightFracBits;
; 496  :            //// Float WeightFactor = uiWeightFactor/(1024.0F*(1<<(cWFFracBits-10)))

    ldr       r0, [sp, #OFFcMaxWeightFracBits]  
    ldr       lr, [r1, +r7iRecon, lsl #2]
    add       r4, r0, #WEIGHTFACTOR_FRACT_BITS
    movs      r1, lr, lsl r0

; 498  :            NormUInt( &uiWeightFactor, &cWFFracBits, 0x3FFFFFFF );      // uiWeightFactor with cWFFracBits fractional bits

    beq       |$L4193| 
    mvn       r0, #0xE, 4                   ; 0x1fffffff
    cmp       r1, r0
    bcs       |$L4199|
|$L4198|
    mov       r1, r1, lsl #2
    add       r4, r4, #2
    cmp       r1, r0
    bcc       |$L4198|
|$L4199|
    mvn       r12, #3, 2                     ; 0x3fffffff
    cmp       r1, r12
    movcc     r1, r1, lsl #1
    addcc     r4, r4, #1
|$L4193|

; 501  :            QuantStepXMaxWeightXWeightFactor = MULT_HI(uiQuantStepXInvMaxWeight,uiWeightFactor);
; 502  :            cFracBits = cQSIMWFracBits+cWFFracBits-31;
; 503  :            //// Float QuantStep*WightFactor/MaxWeight = QuantStepXMaxWeightXWeightFactor/(1.0F*(1<<cFracBits))

r5QuantStepXMaxWeightXWeightFactor  rn  r5

    ldr       r0, [sp, #OFFuiQuantStepXInvMaxWeight]  
    ldr       r2, [sp, #OFFcQSIMWFracBits]
    SMULL     r12, r5QuantStepXMaxWeightXWeightFactor, r1, r0
    ;;; stall for issue = 2
    ldr       r0, pciDitherFactor
    add       r1, r4, r2

; 506  :            iNoise = MULT_HI(iDitherFactor,qrand); 
; 507  :            cFracBits2 = 35+29-31;                          // FP = 33
; 508  :            //// Float Noise = iNoise/(1024.0F*(1<<(33-10)))

    SMULL     r12, r0, r11qrand, r0
    ;;; stall for issue = 2
    MOV       r5QuantStepXMaxWeightXWeightFactor,  r5QuantStepXMaxWeightXWeightFactor, lsl #1
    sub       r4, r1, #31
    MOV       r0, r0, lsl #1

; 510  :            iNoiseQuant = MULT_HI(iNoise,QuantStepXMaxWeightXWeightFactor);
; 511  :            cFracBits2 += (cFracBits-31);
; 512  :            //// Float NoiseQuant = iNoiseQuant/(1024.0F*(1<<(cFracBits2-10)))

r11iNoiseQuant       rn  r11

    SMULL     r12, r11iNoiseQuant, r5QuantStepXMaxWeightXWeightFactor, r0
    ;;; stall for issue = 2
;   ldrsh     r0, [r10]
    ldr       r0, [r10]
    ldr       r2, [sp, #OFFiShiftCoefQ] 
    MOV       r11iNoiseQuant,  r11iNoiseQuant, lsl #1

; 515  :             iCoef = MULT_HI( (Int)rgiCoefQ[iCoefQ]<<iShiftCoefQ, QuantStepXMaxWeightXWeightFactor );
; 516  :            cFracBits += (iShiftCoefQ-31);
; 517  :            //// Float Coef = iCoef/(1.0F*(1<<cFracBits))

    mov       r0, r0, lsl r2
    SMULL     r12, r0, r5QuantStepXMaxWeightXWeightFactor, r0
    ;;; stall for issue = 2
    add       lr, r4, r2
    add       r2, r4, #2
    MOV       r0,  r0, lsl #1


; 519  :            iCoefScaled = Align2FracBits( iCoef, cFracBits, TRANSFORM_FRACT_BITS );  
; 520  :            //// Float CoefScaled = iCoefScaled/32.0F

    subs      r1, lr, #31+TRANSFORM_FRACT_BITS
    rsbmi     lr, r1, #0
    movmi     r0, r0, lsl lr
    bmi       |$L4208|
    cmp       r1, #32
    mov       r0, r0, asr r1
    movge     r0, #0
|$L4208|

; 523  :             iCoefRecon = iCoefScaled + (iNoiseQuant>>(cFracBits2-TRANSFORM_FRACT_BITS));
; 525  :             rgiCoefRecon [iRecon] = iCoefRecon;
; 530  :             iRecon++;
; 531  :            iCoefQ++;

    sub       r2, r2, #5
    add       r1, r0, r11iNoiseQuant, asr r2
    ldr       r0, [sp, #OFFrgiCoefRecon]
;   add       r10, r10, #2
    add       r10, r10, #4
    str       r1, [r0, +r7iRecon, lsl #2]
    ldr       r2, [sp, #OFFiUBLimit]
    add       r7iRecon, r7iRecon, #1

    cmp       r7iRecon, r2
    blt       |$Loop7BandIsCoded|

    ldr       r2, [sp, #OFFptRandState]
    str       r7iRecon, [sp, #OFFiRecon]
    str       r8iPrior, [r2, #tagRandState_iPrior]
    str       r6uiRand, [r2, #tagRandState_uiRand]
    str       r10, [sp, #OFFpiCoefQ] 
    ldr       r11, [sp, #OFFiBarkX4] 
    ldr       r6, [sp, #OFFpffltSqrtBWRatio]  
    ldr       r8, [sp, #OFFrgbBandNotCoded_iBark]  

|$Loop4NoiseRegionEpilog|
    ldr       r0, [sp, #OFFiRecon]
    ldr       r1, [r3pau, #CAudioObject_m_cHighCutOff] 
    ;;;
    cmp       r0, r1
    blt       |$Loop4NoiseRegion|
|$Loop4NoiseRegionX|

; 532  :        }
; 533  :    }
; 534  : 
; 535  :    {   //Calculate from highCutOff to m_cSubband
; 536  :        UInt QuantStepXMaxWeightXWeightFactorXDither;
; 541  :        uiWeightFactor = rgiWeightFactor [pau->m_cHighCutOff - 1] << MORE_WF_FRAC_BITS;
; 542  :        cWFFracBits = WEIGHTFACTOR_FRACT_BITS+MORE_WF_FRAC_BITS;
; 543  :        //// Float WeightFactor = uiWeightFactor/(1024.0F*(1<<(cWFFracBits-10)))
; 544  :        NormUInt( &uiWeightFactor, &cWFFracBits, 0x3FFFFFFF );      

    mov       r4, #21
    ldr       r2, [sp, #OFFpWF]
    mov       r0, #21
    ldr       lr, [r3pau, #CAudioObject_m_cHighCutOff]
    mvn       r5, #0xE, 4                           ; 0x1fffffff
    add       r2, r2, lr, lsl #2
    ldr       r1, [r2, #-4]
    ;;;
    cmp       r1, #0
    beq       |$L4299| 
    cmp       r1, r5
    bcs       |$L4218| 
|$L4217|
    mov       r1, r1, lsl #2
    add       r0, r0, #2
    cmp       r1, r5
    bcc       |$L4217| 
|$L4218|
    mvn       r7, #3, 2                             ; 0x3fffffff
    cmp       r1, r7
    addcc     r0, r0, #1
    movcc     r1, r1, lsl #1
    mov       r4, r0
    b         |$L4212|
|$L4307|
|$L4299|
    mvn       r7, #3, 2                             ; 0x3fffffff
|$L4212|

; 546  :        QuantStepXMaxWeightXWeightFactor = MULT_HI(uiQuantStepXInvMaxWeight,uiWeightFactor);
; 547  :        cFracBits = cQSIMWFracBits+cWFFracBits-31;
; 548  :        //// Float QuantStep*WeightFactor/MaxWeight = QuantStepXMaxWeightXWeightFactor/(1024.0F*(1<<(cFracBits-10)))
; 549  :        NormUInt( (unsigned long *)&QuantStepXMaxWeightXWeightFactor, &cFracBits, 0x3FFFFFFF );

    ldr       r0, [sp, #OFFuiQuantStepXInvMaxWeight] 
    ldr       r2, [sp, #OFFcQSIMWFracBits]
    SMULL     r12, r0, r1, r0
    ;;; stall for issue = 2
    add       lr, r4, r2
    sub       r4, lr, #31
    MOV       r0,  r0, lsl #1
    cmp       r0, #0
    beq       |$L4222|
    cmp       r0, r5
    bcs       |$L4228| 
|$L4227|
    mov       r0, r0, lsl #2
    add       r4, r4, #2
    cmp       r0, r5
    bcc       |$L4227|
|$L4228|
    cmp       r0, r7
    movcc     r0, r0, lsl #1
    addcc     r4, r4, #1
|$L4222|

; 551  :        QuantStepXMaxWeightXWeightFactorXDither = MULT_HI(QuantStepXMaxWeightXWeightFactor,iDitherFactor);
; 552  :        cFracBits += (35-31);
; 553  :        //// Float QS * WF/MaxWF * Dither = QuantStepXMaxWeightXWeightFactorXDither/(1024.0F*(1<<(cFracBits-10)))
; 554  :        NormUInt( &QuantStepXMaxWeightXWeightFactorXDither, &cFracBits, 0x3FFFFFFF );

    ldr       r2, pciDitherFactor
    add       r1, r4, #4
    SMULL     r12, r6, r2, r0
    ;;; stall for issue = 2
    ;;;
    ;;;
    movs      r6,  r6, lsl #1
    mov       r0, r6
    beq       |$L4277|
    cmp       r6, r5
    bcs       |$L4238| 
|$L4237|
    mov       r0, r0, lsl #2
    add       r1, r1, #2
    cmp       r0, r5
    bcc       |$L4237|
|$L4238|
    cmp       r0, r7
    movcc     r0, r0, lsl #1
    addcc     r1, r1, #1
    mov       r6, r0
|$L4277|

; 555  : 
; 556  :        while (iRecon < pau->m_cSubband) {

;   ldrsh     r12, [r3pau, #PerChannelInfo_m_cSubFrameSampleHalf] 
    ldr       r12, [sp, #OFFcSubFrameSampleHalf]
    ldr       r10, [sp, #OFFiRecon]
    ;;;
    cmp       r10, r12
    bge       |$Loop6AboveHighCutOffX| 
    ldr       r9, pciRandA 
    sub       r5, r1, #2+TRANSFORM_FRACT_BITS
    ldr       r1, [sp, #OFFrgiCoefRecon]
    ldr       r8, pciRandC
    ldr       r4, [sp, #OFFptRandState]
    add       r7, r1, r10, lsl #2
    ldr       r11, [r4, #tagRandState_uiRand]
    ldr       r2, [r4, #tagRandState_iPrior]
|$Loop6AboveHighCutOff|

; 557  :            Int iCoefScaled;
; 558  :            Int qrand = quickRand (&(pau->m_tRandState));

    mul       r11, r9, r11
    mov       lr, r2
    cmp       r5, #0
    add       r11, r11, r8
    mov       r2, r11, asr #4
    add       r2, r2, r11, asr #2
    sub       r1, r2, lr

; 559  :            Int iCoefRecon = MULT_HI(QuantStepXMaxWeightXWeightFactorXDither,qrand);
; 560  :            cFracBits2 = cFracBits + 29 - 31;
; 561  :            //// Float CoefRecon = iCoefRecon/(1.0F*(1<<cFracBits2))

    SMULL     r12, r0, r1, r6
    ;;; stall for issue = 2
    rsbmi     r1, r5, #0
    add       r10, r10, #1
    MOV       r0,  r0, lsl #1

; 563  :            iCoefScaled = Align2FracBits( iCoefRecon, cFracBits2, TRANSFORM_FRACT_BITS );
; 565  :            //// Float CoefRecon = rgiCoefRecon [iRecon]/32.0F
; 564  :            rgiCoefRecon [iRecon] = iCoefScaled;                
; 570  :            iRecon++;

    movmi     r0, r0, lsl r1
    bmi       |$L4255|
    cmp       r5, #32
    mov       r0, r0, asr r5
    movge     r0, #0
|$L4255|
;   ldrsh     lr, [r3pau, #PerChannelInfo_m_cSubFrameSampleHalf] 
    ldr       lr, [sp, #OFFcSubFrameSampleHalf]
    str       r0, [r7], #+4
    cmp       r10, lr
    blt       |$Loop6AboveHighCutOff|

    str       r11, [r4, #tagRandState_uiRand]
    str       r2, [r4, #tagRandState_iPrior]

|$Loop6AboveHighCutOffX|

; IF  PAD2X_TRANSFORM = 1 :LOR: HALF_TRANSFORM = 1 :LOR: INTERPOLATED_DOWNSAMPLE = 1
;   if ( pau->m_cSubbandAdjusted > pau->m_cSubband )
;       WMA_MEMCLR_ALIGNED (rgiCoefRecon + pau->m_cSubband, (pau->m_cSubbandAdjusted - pau->m_cSubband) * sizeof (Int));
;   ldr       lr,  [r3pau, #CAudioObject_m_cSubband] 
;   ldr       r11, [r3pau, #CAudioObject_m_cSubbandAdjusted] 
;   mov       r0, r7                        ; rgiCoefRecon + pau->m_cSubband
;   subs      r1, r11, lr
;   mov       r1, r1, LSL #2
;   blgt      |prvWMAMemClrAligned|         ; prvWMAMemClrAligned( void* pDest, int cBytes2Clear )
; ENDIF ; PAD2X_TRANSFORM = 1 :LOR: HALF_TRANSFORM = 1 :LOR: INTERPOLATED_DOWNSAMPLE = 1

; 571  :        }
; 572  :    }
; 579  :    FUNCTION_PROFILE_STOP(&fp);
; 580  :    return WMA_OK;

	    
    mov       r0, #0
	b         gExit

gBrokenFrame
    ldr       r0, pWMA_E_BROKEN_FRAME

; 581  : }

gExit
    add       sp, sp, #OFFLocalSize  
;    ldmia     sp, {r4 - r12, sp, pc}  ; ldmfd	
    ldmfd     sp!, {r4 - r12, pc}  ; ldmfd
    ENTRY_END prvInverseQuantizeLowRate


    ENDIF ; WMA_OPT_INVERSQUAN_LOWRATE_ARM


    END