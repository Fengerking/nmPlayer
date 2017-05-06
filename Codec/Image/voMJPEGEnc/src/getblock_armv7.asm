;************************************************************************
;									                                    *
;	VisualOn, Inc. Confidential and Proprietary, 2010		            *
;	written by zouzh							 	                                    *
;***********************************************************************/
	AREA    |.text|, CODE, READONLY
	EXPORT |vogetblock_armv7|

;prepare_block_data_v7(short* workspaceptr,unsigned char* elemptr);
|vogetblock_armv7| PROC
    ;stmdb	sp!, {r4-r11, lr}		; save regs used
		vld1.64 {q6}, [r0]	; elemptr[0]~elemptr[7] 
		vmov.s16 q7, #128   ; 
		vsub.s16 q6, q6, q7	;elemptr[0~7] - 128
    vst1.64 {q6},[r0]						
		;ldmia	sp!, {r4-r11, pc}		; restore and return 
	  ENDP  ;