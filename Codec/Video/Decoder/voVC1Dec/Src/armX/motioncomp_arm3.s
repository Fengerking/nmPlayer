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
            
     EXPORT  MotionCompMixedHQHI   
          
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
  
  
    WMV_LEAF_ENTRY MotionCompMixedHQHI

;at_even_1=r4
;at_even_2=r5
;at_even_3=r6
;at_even_4=r7
;at_odd_1=r5
;at_odd_2=r6
;at_odd_3=r7
;at_odd_4=r8
;t_even_1=r6
;t_even_2=r7
;t_even_3=r8
;t_even_4=r9
;t_odd_1=r6
;t_odd_2=r7
;t_odd_3=r8
;t_odd_4=r9
;u2=r4
;u3=r5
;err_even=r6
;err_odd=r7
;y0=r4
;overflow=r12
;err_overflow=r10
;0x00080008=r11
;iy=r2
;ppxlcPredMB=r0
;ppxlcRefMB=r1
;pErrorBuf2=r3
;pErrorBuf=r2
;r14 scratch

;stack saved area
;   err_overflow
;   iy
;   iWidthPrev
;   at_even_2,3,4
;   at_odd_2,3,4

    STMFD   sp!, {r4 - r12, r14} ; //r0-r3 are preserved
    FRAME_PROFILE_COUNT

    

;;  r10 is pBlock 
    LDR r10, [ sp, #40 ]; 

;// U32 overflow = 0;

    MOV r12, #0
    

;//U32 mask = 0x00ff00ff;

;//0x00080008
    MOV r11,#0x00080000
    ORR r11, r11, #0x8

    STR r2, [sp, #-16]!; save r2
    MOV r4, r2

    SUB sp, sp, #24 ; make room for t_even/odd_2,3,4

;//const PixelC* pLine  =  ppxlcRefMB + 2*iWidthPrev+1;

    IF DYNAMIC_EDGEPAD=1
    ADD r1, r1, r4, LSR #16
    ELSE
    ADD r1, r1, r4, LSL #1
    ENDIF
    ADD r1, r1, #1

;// U32 err_overflow = 0;
    STR r12, [ sp , #32 ]; 

;//for(Int iz=0;iz<2;iz++)

MCMHQHI_Loop0

; initially pLine=r1=ppxlcRefMB2+2*iWidthPrev+1, so odd first
; r4 = iWidthPrev

;//at_odd_4 = pLine[1] | pLine[3] << 16;
;//pLine   -=  iWidthPrev;
    IF DYNAMIC_EDGEPAD=1
    LDRB r8, [r1, -r4, LSR #17 ]!   
    ELSE
    LDRB r8, [r1, -r4 ]!
    ENDIF   
    LDRB r14, [r1, #2]  
;;relo0 ORR r8, r8, r14, lsl #16

;//at_odd_3 = pLine[1] | pLine[3] << 16;
;//pLine   -=  iWidthPrev;
    IF DYNAMIC_EDGEPAD=1
    LDRB r7, [r1, -r4, LSR #17 ]!       
    ELSE
    LDRB r7, [r1, -r4 ]!    
    ENDIF
    ORR r8, r8, r14, lsl #16 ;;relo0
    LDRB r14, [r1, #2]  
;;relo1 ORR r7, r7, r14, lsl #16
        
;//at_odd_2 = pLine[1] | pLine[3] << 16;
;//pLine   -=  iWidthPrev;
    IF DYNAMIC_EDGEPAD=1
    LDRB r6, [r1, -r4, LSR #17]!
    ELSE
    LDRB r6, [r1, -r4 ]!
    ENDIF       
    
    ORR r7, r7, r14, lsl #16 ;;relo1    
    LDRB r14, [r1, #2]  
    SUB r1, r1, #1;;relo2
    ORR r6, r6, r14, lsl #16

; save at_odd_2,3,4 now

    STMIA sp, { r6 - r8 }

; now deal with the at_even_2,3,4

;;relo2 SUB r1, r1, #1

;//at_even_2 = pLine[0] | pLine[2] << 16;
;//pLine   +=  iWidthPrev;

    LDRB r6, [r1, #2]

    IF DYNAMIC_EDGEPAD=1
    LDRB r5, [r1], r4, LSR #17
    ELSE
    LDRB r5, [r1], r4
    ENDIF
    
;;relo3 ORR r5, r5, r6, lsl #16

;//at_even_3 = pLine[0] | pLine[2] << 16;
;//pLine   +=  iWidthPrev;

        
    LDRB r7, [r1, #2]
    ORR r5, r5, r6, lsl #16;;relo3
    IF DYNAMIC_EDGEPAD=1
    LDRB r6, [r1], r4, LSR #17
    ELSE
    LDRB r6, [r1], r4
    ENDIF
;;relo4 ORR r6, r6, r7, lsl #16
    

;//at_even_4 = pLine[0] | pLine[2] << 16;
;//pLine   +=  iWidthPrev;

        
    LDRB r14, [r1, #2]
    ORR r6, r6, r7, lsl #16 ;;relo4
    IF DYNAMIC_EDGEPAD=1
    LDRB r7, [r1], r4, LSR #17
    ELSE
    LDRB r7, [r1], r4
    ENDIF
    ADD r8, sp, #12 ;;relo5
    ORR r7, r7, r14, lsl #16

;;relo5 ADD r8, sp, #12
    STMIA r8, { r5 - r7 }

;//for (Int iy  =  0; iy < 8; iy ++ ) 

    MOVS r2, #7

MCMHQHI_Loop1

; r1 starts offset by 0, so load even regs first, r8= sp+12

;;relo6 LDMIA r8, { r4 - r6 }

;//at_even_4 = pLine[0] | pLine[2] << 16;

    LDRB r7, [r1]

    LDRB r14, [r1, #2]
    LDMIA r8, { r4 - r6 } ;;relo6
    ORR r7, r7, r14, lsl #16

;// u2 = ((at_even_2 + at_even_3)*9-(at_even_1 + at_even_4) + 0x00080008);

;;macro; CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r4, r4, r5, r6, r7, r11, r14

; now save at_even_2,3,4

    STMIA r8, { r5 - r7 }

;//overflow  |= u2; 
    ORR r12, r12, r4

;// u2 = u2>>4;
    MOV r4, r4, LSR #4


; now load at_odd_2,3,4

;;relo7 LDMIA sp, { r5 - r7 }
    
;//at_odd_4 = pLine[1] | pLine[3] << 16;

    LDRB r8, [r1, #1]
    LDRB r14, [r1, #3]
    LDMIA sp, { r5 - r7 };;relo7
    ORR r8, r8, r14, lsl #16
    
    STMIA sp, { r6 - r8 };;relo10
;//u3 = ((at_odd_2 + at_odd_3)*9-(at_odd_1 + at_odd_4) + 0x00080008);

;;macro; CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r5, r5, r6, r7, r8, r11, r14

; now save t_odd_2,3,4

;;relo10    STMIA sp, { r6 - r8 }

;//overflow  |= u3; 
;;relo9 ORR r12, r12, r5

;// u3 = u3>>4;
;;relo8 MOV r5, r5, LSR #4

    
; loading t_even_1-4 now
    LDMIA r10!, { r6 - r8 }
    LDR r9, [ r10 ], #32

    ORR r12, r12, r5 ;;relo9
    MOV r5, r5, LSR #4;relo8

;// u0 = ((t_even_2 + t_even_3)*9-(t_even_1 + t_even_4) + 0x00080008);
;;macro; CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r6, r6, r7, r8, r9, r11, r14

;// overflow |= u0; 
    ORR r12, r12, r6

;// u0 = u0>>4;
;//u0 = ((u0 & mask) + u2 + 0x00010001)>>1;
    BIC r6, r6, #0xff000
    ADD r4, r6, r4, LSL #4
;;relo11    ADD r4, r4, r11, LSL #1
;;relo12    MOV r4, r4, LSR #5

; loading t_odd_1-4 now

   LDMIA r10!, { r6 - r8}
; update pBlockU32 now
   LDR r9, [ r10 ], #-52

   ADD r4, r4, r11, LSL #1;;relo11 
   MOV r4, r4, LSR #5;;relo12 


;// u1 = ((t_odd_2 + t_odd_3)*9-(t_odd_1 + t_odd_4) + 0x00080008);
;;macro; CubicFilter $u0, $t0, $t1, $t2, $t3, $const, $scratch

    CubicFilter r6, r6, r7, r8, r9, r11, r14
            
;//overflow |= u1;
    ORR r12, r12, r6
            
;//u1   >>= 4;
;//u1 = ((u1 & mask) + u3 + 0x00010001)>>1;
    BIC r6, r6, #0xff000
    ADD r5, r6, r5, LSL #4
    ADD r5, r5, r11, LSL #1
    MOV r5, r5, LSR #5

;// u0  &= mask;
    BIC r4, r4, #0xff00

;// u1  &= mask;
    BIC r5, r5, #0xff00

; //   if(pErrorBuf2 != NULL)

    CMP r3, #4

    BLE MCMHQHI_L0   

    STR r10, [ sp, #80 ];;

    LDR r10, [ sp, #32 ]; restore err_overflow 
;;;Macro: AndAddError $pErrorBuf, $err_even, $err_odd, $u0, $u1, $err_overflow, $scratch

    AndAddError r3, r6, r7, r4, r5, r10, r14

    STR r10, [ sp, #32 ]; save err_overflow 

    LDR r10, [ sp, #80 ];;

MCMHQHI_L0
         

;//y0 = (u0) | ((u1) << 8);

    ORR r5, r4, r5, LSL #8

;//* (U32 *) ppxlcPredMB= y0;
;// ppxlcPredMB   +=  iWidthPrev;

    LDR r4, [sp, #24] ; get the iWidthPrev now

    ADD r8, sp, #12 ;;relo14
;;relo13    STR r5, [ r0 ], r4 

    IF DYNAMIC_EDGEPAD=1
    AND r7, r4, #0x3fc ;r7=iWidthPrev, r4>>17=iWidthPrefRef
    ADD r1, r1, r4, LSR #17  ;;relo10 ;r1 still even
    ELSE
;// pLine  +=  iWidthPrev;
    ADD r1, r1, r4 ;;relo10 ;r1 still even
    ENDIF ;IF DYNAMIC_EDGEPAD=1

; // } //for (Int iy  =  0; iy < 8; iy ++ ) 

;  prepare r8 for the loop start
;;relo14 ADD r8,    sp, #12

    IF DYNAMIC_EDGEPAD=1
    STR r5, [ r0 ], r7 ;;relo13
    ELSE
    STR r5, [ r0 ], r4 ;;relo13
    ENDIF

    SUBS r2, r2, #1
    BGE MCMHQHI_Loop1

;//     ppxlcRefMB=ppxlcRefMB2+4;

    IF DYNAMIC_EDGEPAD=1
    SUB r1, r1, r4, LSR #14  ; this brings r1 back to ppxlcRefMB2+2*iWidthPrev
    ELSE
    SUB r1, r1, r4, LSL #3  ; this brings r1 back to ppxlcRefMB2+2*iWidthPrev
    ENDIF
    ADD r1, r1, #5  ; 4+1 make sure r1 is odd now

;//     ppxlcPredMB=ppxlcPredMB2+4;
    IF DYNAMIC_EDGEPAD=1
    SUB r0, r0, r7, LSL #3
    ELSE
    SUB r0, r0, r4, LSL #3
    ENDIF
    ADD r0, r0, #4

;//pBlockU32   +=  (3+11);

    ADD r10, r10, #56 

    CMP r3, #4
    SUBGT r3, r3, #128

    TST r3, #4

;//     pErrorBuf=pErrorBuf2+1;

    
    ADD r3, r3, #4


    BEQ MCMHQHI_Loop0

;//   } //for(Int iz=0;iz<2;iz++)


    LDR r11, [ sp, #32 ]
    ADD sp, sp, #40 ; adjust the stack now

;//    if((err_overflow & 0xff00ff00) || (overflow & 0xf000f000))

    TST r11, #0xff000000
    TSTEQ r11, #0xff00
    TSTEQ r12, #0xf0000000
    TSTEQ r12, #0xf000
    LDMEQFD   sp!, {r4 - r12, PC}
    

;//MotionCompMixedHQHIComplete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2);
    
    SUB r0, r0, #8
    SUB r1, r1, #9
    IF DYNAMIC_EDGEPAD=1
    SUB r1, r1, r4, LSR #16
    ELSE
    SUB r1, r1, r4, LSL #1
    ENDIF
    MOV r2, r4
    SUB r3, r3, #8

    SUB r14, r10, #176

;    STR r14, [sp, #-4]
    STR r14, [sp, #-8]
    SUB sp, sp, #8
    BL MotionCompMixedHQHIComplete

    ADD sp, sp, #8

    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

	ENDIF  ;    IF WMV_OPT_MOTIONCOMP_ARM = 1

    END 