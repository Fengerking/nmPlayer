    INCLUDE wmvdec_member_arm.inc
    INCLUDE xplatform_arm_asm.h 
    IF UNDER_CE != 0
    INCLUDE kxarm.h
    ENDIF
    
    IF WMV_OPT_MOTIONCOMP_ARM = 1
    
	AREA	|.rdata|, DATA, READONLY
    IMPORT  MotionCompMixedHQHIComplete
    IMPORT  MotionCompMixedAlignBlock
    IMPORT  MotionCompMixed000Complete
    IMPORT  MotionCompWAddError00Complete
            
    EXPORT  MotionCompMixed000    
    EXPORT  MotionCompWAddError00    
      
;PRESERVE8
	AREA	|.text|, CODE, READONLY

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;//Void MotionCompMixed010(PixelC*   ppxlcPredMB, const PixelC*  ppxlcRefMB, Int iWidthPrev, I32 * pErrorBuf )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


    IF ARCH_V3 = 1
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        MACRO             
        LOADONE16bitsLo $srcRN, $offset, $dstRN
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        LDR     $dstRN, [$srcRN, $offset]
        MOV     $dstRN, $dstRN, LSL #16
        MOV     $dstRN, $dstRN, ASR #16 
        MEND
    ENDIF ; //ARCH_V3

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    MACRO
    AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch

;// err_even = pErrorBuf[0];

    LDR $err_even, [$pErrorBuf], #0x10

;// err_odd  = pErrorBuf[0 + 32];

    LDR $err_odd, [$pErrorBuf, #0x70]


;//pErrorBuf += 4;



;//u0 = u0 + err_even-((err_even & 0x8000) << 1);

    AND $scratch, $err_even, #0x8000
    SUB $scratch, $err_even, $scratch, LSL #1
    ADD $u0, $u0, $scratch

;//err_overflow  |= u0;
    ORR $err_overflow, $err_overflow, $u0

;//u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);

    AND $scratch,$err_odd,#0x8000
    SUB $scratch,$err_odd,$scratch, LSL #1
    ADD $u1, $u1, $scratch

;//err_overflow  |= u1;
    ORR $err_overflow, $err_overflow, $u1

    MEND

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    MACRO
    AndAddErrorPartial $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch
;//u0 = u0 + err_even-((err_even & 0x8000) << 1);

    AND $scratch, $err_even, #0x8000
    SUB $scratch, $err_even, $scratch, LSL #1
    ADD $u0, $u0, $scratch

;//err_overflow  |= u0;
    ORR $err_overflow, $err_overflow, $u0

;//u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);

    AND $scratch,$err_odd,#0x8000
    SUB $scratch,$err_odd,$scratch, LSL #1
    ADD $u1, $u1, $scratch

;//err_overflow  |= u1;
    ORR $err_overflow, $err_overflow, $u1

    MEND
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    MACRO
    CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch
;   //u0 = ((t1 + t2)*9-(t0 + t3) + 0x00080008);
    
    ADD $scratch, $t1, $t2  
    ADD $scratch, $scratch, $scratch, LSL #3
    SUB $u0, $scratch, $t0
    SUB $u0, $u0, $t3
    ADD $u0, $u0, $const

    MEND
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    MACRO
    CubicFilterShort $u0, $t0, $t1, $t2, $const, $scratch
;   //u0 = ((t1 + t2)*9-(t0 ) + 0x00080008);
    
    ADD $scratch, $t1, $t2  
    ADD $scratch, $scratch, $scratch, LSL #3
    SUB $u0, $scratch, $t0
    ADD $u0, $u0, $const

    MEND

    


    WMV_LEAF_ENTRY MotionCompMixed000

;t_even_1=r4
;t_even_2=r5
;t_even_3=r6
;t_even_4=r7
;t_odd_1=r5
;t_odd_2=r6
;t_odd_3=r7
;t_odd_4=r8
;u0=r4
;u1=r5
;err_even=r6
;err_odd=r7
;y0=r4
;overflow=r9
;err_overflow=r10
;0x00080008=r11
;iy=r12
;ppxlcPredMB=r0
;ppxlcRefMB=r1
;pBlockU32=r1
;pErrorBuf2=r3
;pErrorBuf=r2
;r14 scratch
;stack saved area
;   iWidthPrev


    STMFD   sp!, {r4 - r12, r14} ; 
    FRAME_PROFILE_COUNT

    SUB sp, sp, #176 ; sizeof(tempBlock)

    ;//iWidthPrev  &= 0xffff;

    BIC r2, r2, #0x10000

;    STR r1, [sp, #-4]!
;    STR r2, [sp, #-4]!

    STR r1, [sp, #-4]
    STR r2, [sp, #-8]
    sub	sp, sp, #8

    STMFD   sp!, {r0, r3} ; //r0-r3 are preserved

    ADD r0, sp, #16

    BL MotionCompMixedAlignBlock

    LDMFD   sp!, {r0, r3}

;   now sp+0 is iWidthPrev, sp+4 is original ppxlcRefMB

    ADD r1, sp, #8;  r1 = pBlockU32

    IF DYNAMIC_EDGEPAD=1
        IF ARCH_V3 = 1
            LOADONE16bitsLo sp, #0, r2 ; sp is always word aligned.
        ELSE
            LDRH r2, [sp];;relo2
        ENDIF
    ELSE
        LDR r2, [sp];;relo2 
    ENDIF

;// U32 err_overflow = 0;

    MOV r10, #0

;// U32 overflow = 0;

    MOV r9, #0

;//U32 mask = 0x00ff00ff;

;//0x00080008
    MOV r11,#0x00080000
    ORR r11, r11, #0x8

;//for(Int iz=0;iz<2;iz++)

MCM000_Loop0
    
    
;//for (Int iy  =  0; iy < 8; iy ++ ) 

    MOV r12, #8

MCM000_Loop1

; load t_even_1-4 now

    LDMIA r1!, { r4 - r6 }
    LDR r7, [ r1 ], #32

;// u0 = ((t_even_2 + t_even_3)*9-(t_even_1 + t_even_4) + 0x00080008);
 

;;macro; CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r4, r4, r5, r6, r7, r11, r14

    LDMIA r1!, { r5 - r7 };;relo1
    LDR r8, [ r1 ], #-52
;//overflow  |= u0; 
    ORR r9, r9, r4

;// u0 = u0>>4;
    MOV r4, r4, LSR #4

; load t_odd_1-4 now

;;relo0    ADD r1, r1, #44
;;relo1    LDMIA r1, { r5 - r8 }

;// u1 = ((t_odd_2 + t_odd_3)*9-(t_odd_1 + t_odd_4) + 0x00080008);

;;macro; CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r5, r5, r6, r7, r8, r11, r14
    


;//overflow  |= u1; 
    ORR r9, r9, r5

;// u1 = u1>>4;
    MOV r5, r5, LSR #4

;// u0  &= mask;

    BIC r4, r4, #0xff00


;// u1  &= mask;

    BIC r5, r5, #0xff00

; //   if(pErrorBuf2 != NULL)
    CMP r3, #0
	BLT AndAddError_Done
	CMP r3, #4
    BLE MCM000_L0    



;;;Macro: AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch
AndAddError_Done
    AndAddError r3, r6, r7, r4, r5, r10, r14



MCM000_L0
         

;//y0 = (u0) | ((u1) << 8);

    ORR r4, r4, r5, LSL #8

;//* (U32 *) ppxlcPredMB= y0;
;// ppxlcPredMB   +=  iWidthPrev;

;;relo2 LDR r8, [sp]

    STR r4, [ r0 ], r2 

            
;// ppxlcRefMB   +=  iWidthPrev;


; // } //for (Int iy  =  0; iy < 8; iy ++ ) 

    SUBS r12, r12, #1
    BGT MCM000_Loop1



;//     pBlockU32+=3+11;

    ADD r1, r1, #56

;//     ppxlcPredMB=ppxlcPredMB2+4;

    SUB r0, r0, r2, LSL #3
    ADD r0, r0, #4

		CMP 	r3, #0
    BLT		NEXT_POS_000
    CMP 	r3, #4
    BLE		TEST_NEXT_HALF_000

NEXT_POS_000
    SUB		r3, r3, #128

TEST_NEXT_HALF_000
    TST r3, #4

;//     pErrorBuf=pErrorBuf2+1;

    
    ADD r3, r3, #4


    BEQ MCM000_Loop0


;//   } //for(Int iz=0;iz<2;iz++)

    IF DYNAMIC_EDGEPAD=1
    LDR r2, [sp]
    ENDIF

    LDR r1, [ sp, #4 ]
    ADD sp, sp, #184

;//    if((err_overflow & 0xff00ff00) || (overflow & 0xf000f000))

    TST r10, #0xff000000
    TSTEQ r10, #0xff00
    TSTEQ r9, #0xf0000000
    TSTEQ r9, #0xf000
    LDMEQFD   sp!, {r4 - r12, PC}
    

;//MotionCompMixed000Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2);
    
    SUB r0, r0, #8
    SUB r3, r3, #8

    BL MotionCompMixed000Complete

    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END
        
        
          
    WMV_LEAF_ENTRY MotionCompWAddError00

;t_even_1=r4
;t_odd_1=r5
;ta1=r6
;t_even_2=r7
;t_odd_2=r8
;ta2=r9
;u0=r4
;u1=r5
;err_even=r8
;err_odd=r9
;rndCtrl=r11
;pErrorBuf=r2
;y0=r4
;err_overflow=r10
;iy=r12
;ppxlcPredMB=r0
;ppxlcRefMB=r1
;pLine=r1
;pErrorBuf2=r3
;r14 scratch
;stack save:
;  iWidthPrev
; t_even_2, t_odd_2, ta2

    STMFD   sp!, {r4 - r12, r14} ; 
    FRAME_PROFILE_COUNT

;// U32 err_overflow = 0;

    MOV r10, #0

;//   U32 rndCtrl = iWidthPrev>>16;
;   MOV r11, r2, LSR #16
    AND r11, r2, #0x10000

;// rndCtrl ++ ;
    ADD r11, r11, #0x10000

;// rndCtrl |= rndCtrl << 16;
    ORR r11, r11, r11, LSR #16

;//iWidthPrev  &= 0xffff;

    BIC r2, r2, #0x10000

    SUB sp, sp, #16 ; make room for t_even_2, t_odd_2, ta2

;//for(Int iz=0;iz<2;iz++)

MCWAE00_Loop0

;// assume r6=iWidthPrev

;// ta2 = pLine[2] | pLine[4] << 16;
    LDRB r9, [r1, #2]
    LDRB r14, [r1, #4]
;;relo0 ORR r9, r9, r14, lsl #16
    
;//     t_odd_2 = pLine[1] | pLine[3] << 16;
    LDRB r8, [r1, #1]
    ORR r9, r9, r14, lsl #16 ;;relo0
    LDRB r7, [r1, #3]
;;relo1 ORR r8, r8, r7, lsl #16

;// t_even_2 = pLine[0] | pLine[2] << 16;

;//pLine   +=  iWidthPrev;
    LDRB r14, [r1, #2]
    ORR r8, r8, r7, lsl #16 ;;relo1

    IF DYNAMIC_EDGEPAD=1
    LDRB r7, [r1], r2, LSR #17
    ELSE
    LDRB r7, [r1], r2
    ENDIF
    
    MOVS r12, #7 ;;relo2
    ORR r7, r7, r14, lsl #16

    STMIA sp, { r7 - r9}
    
;//for (Int iy  =  0; iy < 8; iy ++ ) 

;;relo2 MOV r12, #8

MCWAE00_Loop1


;// t_even_2 = pLine[0] | pLine[2] << 16;
    
    LDRB r7, [r1]

    LDRB r14, [r1, #2]
    LDMIA sp, { r4 - r6};;relo3
    ORR r7, r7, r14, lsl #16

;//     t_odd_2 = pLine[1] | pLine[3] << 16;
    LDRB r8, [r1, #1]
    LDRB r14, [r1, #3]
;;relo4 ORR r8, r8, r14, lsl #16

;// ta2 = pLine[2] | pLine[4] << 16;
    LDRB r9, [r1, #2]
    ORR r8, r8, r14, lsl #16 ;;relo4
    LDRB r14, [r1, #4]
;;relo5 ORR r9, r9, r14, lsl #16

;//   u0 = (t_even_1 + t_even_2 + t_odd_1 + t_odd_2 + rndCtrl);
;//  u0 = u0>>2;

    ADD r4, r4, r5
    ADD r4, r4, r7
    ADD r4, r4, r8
    ADD r4, r4, r11
    MOV r4, r4, LSR #2

    ORR r9, r9, r14, lsl #16;;relo5

   STMIA sp, { r7 - r9 };;relo6 


;// u1 = (t_odd_1 + t_odd_2 + ta1 + ta2 + rndCtrl);
;//u1   >>= 2;
    ADD r5, r5, r6
    ADD r5, r5, r8
    ADD r5, r5, r9
    ADD r5, r5, r11
    MOV r5, r5, LSR #2

;// u0  &= mask;

    BIC r4, r4, #0xff00


;// u1  &= mask;

    BIC r5, r5, #0xff00

; //   if(pErrorBuf2 != NULL)
    CMP r3, #0
	BLT	VO_AndAddError
	CMP r3, #4
    BLE MCWAE00_L0

VO_AndAddError
;;;Macro: AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch
 ;AndAddError r3, r8, r9, r4, r5, r10, r14
    ;MACRO
    ;AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch

;// err_even = pErrorBuf[0];

    LDR r8, [r3], #0x10

;// err_odd  = pErrorBuf[0 + 32];

    LDR r9, [r3, #0x70]

;//pErrorBuf += 4;

;//u0 = u0 + err_even-((err_even & 0x8000) << 1);

    AND r14, r8, #0x8000
    SUB r14, r8, r14, LSL #1
    ADD r4,  r4, r14

;//err_overflow  |= u0;
    ORR r10, r10, r4

;//u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);

    AND r14,r9,#0x8000
    SUB r14,r9,r14, LSL #1
    ADD r5, r5, r14

;//err_overflow  |= u1;
    ORR r10, r10, r5

    ;MEND


MCWAE00_L0

;   picking up iWidthPrev
    
;//y0 = (u0) | ((u1) << 8);

    ORR r4, r4, r5, LSL #8

;//* (U32 *) ppxlcPredMB= y0;
;// ppxlcPredMB   +=  iWidthPrev;

    IF DYNAMIC_EDGEPAD=1
    AND r5, r2, #0x3fc ;r5=iWidthPrev, r8>>17=iWidthPrefRef
    STR r4, [ r0 ], r5 
    ADD r1, r1, r2, LSR #17 ;;relo7
    ELSE
    STR r4, [ r0 ], r2 
    ADD r1, r1, r2 ;;relo7
    ENDIF ;IF DYNAMIC_EDGEPAD=1
    

;//pLine   +=  iWidthPrev;

;;relo7   ADD r1, r1, r6


; // } //for (Int iy  =  0; iy < 8; iy ++ ) 

    SUBS r12, r12, #1
    BGE MCWAE00_Loop1       
      
;//     ppxlcRefMB=ppxlcRefMB2+4;

    IF DYNAMIC_EDGEPAD=1
    SUB r1, r1, r2, LSR #14
    SUB r1, r1, r2, LSR #17
    ELSE
    SUB r1, r1, r2, LSL #3
    SUB r1, r1, r2
    ENDIF
    ADD r1, r1, #4

;//     ppxlcPredMB=ppxlcPredMB2+4;

    IF DYNAMIC_EDGEPAD=1
    SUB r0, r0, r5, LSL #3
    ELSE
    SUB r0, r0, r2, LSL #3
    ENDIF

    ADD r0, r0, #4

    CMP 	r3, #0
    BLT		NEXT_POS_00
    CMP 	r3, #4
    BLE		TEST_NEXT_HALF_00

NEXT_POS_00
    SUB		r3, r3, #128

TEST_NEXT_HALF_00
    TST 	r3, #4
	;//pErrorBuf=pErrorBuf2+1;
    ADD 	r3, r3, #4
    
    BEQ 	MCWAE00_Loop0
;//    if((err_overflow & 0xff00ff00) )

    ADD sp, sp, #16

    TST r10, #0xff000000
    TSTEQ r10, #0xff00
    LDMEQFD   sp!, {r4 - r12, PC}
    

;//MotionCompMixed010Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2);
    
    SUB r0, r0, #8
    SUB r1, r1, #8

    SUB r11, r11, #1
    ORR r2, r2, r11, LSL #16
    SUB r3, r3, #8

    BL MotionCompWAddError00Complete

    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

	ENDIF   ;    IF WMV_OPT_MOTIONCOMP_ARM = 1
            
    END 