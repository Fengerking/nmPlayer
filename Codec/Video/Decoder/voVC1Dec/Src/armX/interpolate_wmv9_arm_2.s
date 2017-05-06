    INCLUDE wmvdec_member_arm.inc
    INCLUDE xplatform_arm_asm.h 
    IF UNDER_CE != 0
    INCLUDE kxarm.h
    ENDIF
    
    IF WMV_OPT_MOTIONCOMP_ARM = 1
    
	AREA	|.text|, CODE, READONLY
	
	IMPORT	|g_AddErrorB_EMB_Overflow_C|
    IMPORT  g_AddError_EMB_Overflow_C	
	EXPORT	g_AddErrorB_SSIMD_ARMV4 
	EXPORT	g_AddError_SSIMD_ARMV4

	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;Void_WMV g_AddErrorB_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef0 ,  U32_WMV* pRef1 , 
;                        I32_WMV* pErrorBuf, I32_WMV iPitch, U32_WMV iOffset)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    WMV_LEAF_ENTRY g_AddErrorB_SSIMD_ARMV4
;r0 = ppxlcDst
;r1 = pRef0
;r2 = pRef1
;r3 = pErrorBuf
;r11 = iPitch
;r4-r7 = u0-u7
;r14 = iy
;r8=err_even
;r9=err_odd
;r10=tmp
;r12=err_overflow

     stmdb     sp!, {r4 - r12, r14}
    FRAME_PROFILE_COUNT

   ; I32_WMV iy;
   ; U32_WMV u0,u1,u2,u3, y0,y1;
   ; U32_WMV err_even, err_odd;

AEBS_OffsetRegSaving   EQU       0x28
AEBS_Offset_iPitch          EQU       AEBS_OffsetRegSaving + 0
AEBS_Offset_iOffset        EQU       AEBS_OffsetRegSaving +  4

    ldr r4, [sp, #AEBS_Offset_iOffset];
  
    mov r11, #1
    mov r14, #8
    orr   r11, r11, r11, lsl #16
    mov r12, #0

   add r1, r1, r4, lsl #2
   add r2, r2, r4, lsl #2

   

   
   

AEBS_Loop
    ;for (iy = 0; iy < BLOCK_SIZE; iy++) 
    
        ;err_even = pErrorBuf[1];
        ;err_odd  = pErrorBuf[1 + 16];

        ldr r8, [ r3, #4   ]
        ldr r6, [ r1, #4   ] ;; relo 0

        ; v2 = pRef1[1];
        ldr r10, [r2, #4 ];

        ldr r9, [ r3, #17*4]

        ;u2 = pRef[1];
        ;u3 = pRef[1 + 5];
        ;relo0 ldr r6, [ r1, #4   ]

        ;u2 = (u2 + v2 + 0x00010001) >>1;
        ;u2 = u2 & ~0x8000;
        add r6, r6, r11
        add r6, r6, r10
        bic r6, r6, #0x10000

        

        ;u2 = u2 + err_even-((err_even & 0x8000) << 1);
        ;err_overflow  |= u2;

        and r10, r8, #0x8000
        ldr r7, [ r1, #6*4]
        sub r8, r8, r10, lsl #1
        ldr r10, [r2, #6*4 ]; //v1 = pRef1[0 + 5];
        add r6,  r8, r6, lsr #1

        ;u3 = (u3 + v1 + 0x00010001) >>1;
        ;u3 = u3 & ~0x8000;
        add r7, r7, r11
        add r7, r7, r10
        bic r7, r7, #0x10000
        

        ;u3 = u3 + err_odd -((err_odd  & 0x8000) << 1);
        ;err_overflow  |= u3;

        and r10, r9, #0x8000
        orr r12, r12, r6
        sub r9, r9, r10, lsl #1
        add r7,  r9, r7, lsr #1
        orr r12, r12, r7


        ;err_even = pErrorBuf[0];
        ;err_odd  = pErrorBuf[0 + 16];
        
        ldr r5, [ r1, #5*4] ;relo1
        ; v1 = pRef1[5];
        ldr r10, [r2, #5*4 ];
        ldr r9, [ r3, #16*4]
        ldr r8, [ r3 ], #8

        ;u0 = pRef[0];
        ;u1 = pRef[0 + 5];
        ;relo1 ldr r5, [ r1, #5*4]

        ;u1 = (u1 + v1 + 0x00010001) >>1;
        ;u1 = u1 & ~0x8000;
        add r5, r5, r11
        add r5, r5, r10
        bic r5, r5, #0x10000
        
        ;u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
        ;err_overflow  |= u1;

        and r10, r9, #0x8000
        ldr r4, [ r1 ], #40
        sub r9, r9, r10, lsl #1
        ;v0 = pRef1[0];
        ldr r10, [r2 ], #40;
        add r5, r9, r5, lsr #1

        ;u0 = (u0 + v0 + 0x00010001) >>1;
        ;u0 = u0 & ~0x8000;

        add r4, r4, r11
        add r4, r4, r10
        bic r4, r4, #0x10000
        
        ;u0 = u0 + err_even-((err_even & 0x8000) << 1);
        ;err_overflow  |= u0;
        
        and r10, r8, #0x8000
        orr r12, r12, r5
        sub r8, r8, r10, lsl #1

        ldr r10, [sp, #AEBS_Offset_iPitch]

        add r4,  r8, r4, lsr #1
        orr r12, r12, r4
       
            
        ;pErrorBuf += 2;
        ;pRef += 10;


        ;y0 = (u0) | ((u1) << 8);
        ;y1 = (u2) | ((u3) << 8);
        orr r4, r4, r5, lsl #8
        orr r6, r6, r7, lsl #8

        ;*(U32_WMV *)ppxlcDst = y0;
        ;*(U32_WMV *)(ppxlcDst + 4)= y1;

        str r6, [r0, #4]
        str r4, [r0], r10

        ;ppxlcDst += iPitch;

        subs r14, r14, #1
        bgt AEBS_Loop

    ;}

    ;DEBUG_STATEMENT(g_AddErr_cnt++;)

    mov r14, #0xff00
    orr r14, r14, r14, lsl #16

    ;if(err_overflow & 0xff00ff00)
    ;{

        tst r14, r12
        ldmeqia     sp!, {r4 - r12, pc}

        ldr r10, [sp, #AEBS_Offset_iPitch]
        ldr r4, [sp, #AEBS_Offset_iOffset];

        ;pRef -= 10*8;
        sub r1, r1, #80*4
        sub r2, r2, #80*4

        sub r1, r1, r4, lsl #2
        sub r2, r2, r4, lsl #2
         
        ;pErrorBuf -= 16;
        sub r3, r3, #16*4

        ;ppxlcDst -= 8*iPitch;
        sub r0, r0, r10, lsl #3
        
        str r10, [ sp, #-8]!
        str r4, [sp, #4]

        ;g_AddErrorB_EMB_Overflow(ppxlcDst, pRef0 , pRef1, pErrorBuf, iPitch, iOffset);

        
        bl g_AddErrorB_EMB_Overflow_C
        
        add  sp, sp, #8

        ldmia     sp!, {r4 - r12, pc}
        WMV_ENTRY_END

    
;  }


;Void_WMV g_AddError_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   WMV_LEAF_ENTRY g_AddError_SSIMD_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;r0 = ppxlcDst
;r1 = pRef
;r2 = pErrorBuf
;r3 = iPitch
;r12=err_overflow
;r14=tmp

	
    stmdb     sp!, {r4 - r10, r14}
    FRAME_PROFILE_COUNT
    
;    I32_WMV iy;
;    U32_WMV err_overflow = 0;
;    U32_WMV u0,u1,u2,u3, y0,y1;
;    U32_WMV err_even, err_odd;
;    U32_WMV err_even1, err_odd1;   

	mov r12, #0
	mov r10, #8
AEE_Loop2


;        u1 = pRef[0 + 5];
;        u3 = pRef[1 + 5];  
;        err_odd  = pErrorBuf[0 + 16];
;        err_odd1  = pErrorBuf[1 + 16];   

        ldr r6, [ r2, #64] ;  
		ldr	r7, [r2, #68]    
        ldr r4, [ r1, #20] ;
		ldr r5, [ r1, #24] ;
		
;        u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
;        err_overflow  |= u1;
;        u3 = u3 + err_odd1 -((err_odd1  & 0x8000) << 1);
;        err_overflow  |= u3;

        ands r14, r6, #0x8000 
        and r9, r7, #0x8000
        subne r6, r6, r14, lsl #1
        add r8, r4, r6 
        orr r12, r12, r8                     
        sub r7, r7, r9, lsl #1
        add r9, r5, r7 
        
;        u0 = pRef[0];
;        u2 = pRef[1]; 
;        pRef += 10;            
;        err_even = pErrorBuf[0];
;        err_even1 = pErrorBuf[1];   
;        pErrorBuf += 2; 
     
        ldr r6, [ r2], #4; 
        ldr r4, [ r1], #4; 
		ldr r7, [ r2], #4;		      
		ldr r5, [ r1], #36;		  
                                                 
;        u0 = u0 + err_even-((err_even & 0x8000) << 1);
;        err_overflow  |= u0;
;        u2 = u2 + err_even1-((err_even1 & 0x8000) << 1);
;        err_overflow  |= u2;

        ands r14, r6, #0x8000
        orr r12, r12, r9    
        subne r6, r6, r14, lsl #1
        add r4, r4, r6 
        ands r14, r7, #0x8000
        orr r12, r12, r4   
                        
        subne r7, r7, r14, lsl #1
        add r5, r5, r7 
        orr r12, r12, r5    

;        y0 = (u0) | ((u1) << 8);
;        y1 = (u2) | ((u3) << 8);
        orr r4, r4, r8, lsl #8
        orr r5, r5, r9, lsl #8
		
;        *(U32_WMV *)ppxlcDst = y0;
;        *(U32_WMV *)(ppxlcDst + 4)= y1;
;        ppxlcDst += iPitch; 

        subs	r10, r10, #1          
        str  r5, [r0, #4]   
        str  r4, [r0], r3       

        bgt AEE_Loop2
    ;}

    ;DEBUG_STATEMENT(g_AddErr_cnt++;)

    mov r14, #0xff00
    orr r14, r14, r14, lsl #16

    ;if(err_overflow & 0xff00ff00)
    ;{

    tst r14, r12
    ldmeqia     sp!, {r4 - r10, pc}

    

        ;ppxlcDst -= 8*iPitch;
        sub r0, r0, r3, lsl #3

        ;pRef -= 10*8;
        sub r1, r1, #80*4
        
        ;pErrorBuf -= 16;
        sub r2, r2, #16*4

        ;g_AddError_EMB_Overflow(ppxlcDst, pRef , pErrorBuf, iPitch);

        
        bl g_AddError_EMB_Overflow_C
        
        ldmia     sp!, {r4 - r10, pc}
    
;  }
	
    WMV_ENTRY_END
    
	ENDIF  ;    IF WMV_OPT_MOTIONCOMP_ARM = 1
    
	END
    