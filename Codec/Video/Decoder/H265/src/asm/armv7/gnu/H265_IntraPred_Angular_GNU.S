;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
;@VO_VOID  H265_IntraPred_Angular(VO_U8 *p_dst, VO_U8 *top, VO_U8 *left,VO_S32 dst_stride,
;@                                VO_S32 size, VO_S32 c_idx, VO_S32 mode)
;@
;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		    		
    .text 
    .align 4    
        
intra_pred_angle:
   .byte     32, 26, 21, 17, 13, 9, 5, 2, 0, -2, -5, -9, -13, -17, -21, -26, -32
	 .byte     -26, -21, -17, -13, -9, -5, -2, 0, 2, 5, 9, 13, 17, 21, 26, 32 
	 .byte     0, 0, 0 @; fill byte for 4B align
	 
	 
inv_angle:	 
	 .word     -4096, -1638, -910, -630, -482, -390, -315, -256, -315, -390, -482
	 .word     -630, -910, -1638, -4096
	 
		     #include     "h265dec_ASM_config.h"
        .section .text
        .align 4
        .if MC_ASM_ENABLED==1

        .global H265_IntraPred_Angular_mode_GE18_ASM
        .global H265_IntraPred_Angular_mode_LT18_ASM
        
H265_IntraPred_Angular_mode_GE18_ASM:   
        stmfd sp!, {r4-r11, lr}
        add   r12, sp, #36
        ldmia r12, {r4 - r6} 			;@ size，c_idx, mode
        sub   sp, sp ,#100 				;@ ref_array[3*32+1]
        ldr    r12, =intra_pred_angle
        sub   r8, r6, #2 					;@ mode-2
        ldrsb  r8, [r12,r8] 				;@ angle = intra_pred_angle[mode-2];
       @;cmp    r6, #18 						;@ if (mode >= 18)    
        mul    r9, r8, r4 				;@ (angle * size)    
       @;blt    H265_IntraPred_Angular_mode_LT18
        
       @; H265_IntraPred_Angular_mode_GE_18   
        mov    r12, #1
        adds   r9, r12, r9, asr #5 		;@ last+1   
        sub    r7, r1, #1 				;@ ref = top - 1;
        sub    r1, r1, #1 					;@ p_orig_top = top - 1;
        sub    r2, r2, #1 						;@ p_orig_left = left - 1;
        bge    H265_IntraPred_Angular_Loop
       @; if (/*angle < 0 &&*/ last < -1) 
        add    r7, sp, r4 				;@ ref = ref_array + size;
        ldr    r12, =inv_angle 		;@ inv_angle
        sub    r10, r6, #11 				;@ mode-11
        
        ldr    r10, [r12, r10, lsl #2] 	;@ inv_angle_mode_value = inv_angle[mode-11];
        sub    r9, r9, #1 					;@ last, 必定<-1
        mov    r11, #128
H265_IntraPred_Angular_Last_LT_Loop_1:    @; for (x = last; x <= -1; x++)   
        mla    r12,  r9, r10, r11 				;@ x * inv_angle_mode_value + 128
        ldrb   r12,  [r2, r12, asr #8]  @; p_orig_left[((x * inv_angle_mode_value + 128) >> 8)];
        
        strb   r12,  [r7, r9] 						;@ ref[x]        
        adds   r9, r9, #1        
        blt   H265_IntraPred_Angular_Last_LT_Loop_1
       @; for (x = 0; x <= size; x++)
        cmp   r4, #32
        beq   H265_IntraPred_Angular_Last_LT_Loop_1_w32
        cmp   r4, #16
        beq   H265_IntraPred_Angular_Last_LT_Loop_1_w16
        cmp   r4, #8
        beq   H265_IntraPred_Angular_Last_LT_Loop_1_w8
        
        vld1.32 {d0[0]}, [r1]
        ldrb    r12, [r1, #4]
        vst1.32 {d0[0]}, [r7]        
        strb    r12, [r7, #4]
        b H265_IntraPred_Angular_Loop
H265_IntraPred_Angular_Last_LT_Loop_1_w8:        
        
        vld1.8  {d0}, [r1]
        ldrb    r12, [r1, #8]
        vst1.8  {d0}, [r7]        
        strb    r12, [r7, #8]
        b H265_IntraPred_Angular_Loop
H265_IntraPred_Angular_Last_LT_Loop_1_w16:        
        cmp   r4, #16
        vld1.8  {d0, d1}, [r1]
        ldrb    r12, [r1, #16]
        vst1.8  {d0, d1}, [r7]        
        strb    r12, [r7, #16]
        b H265_IntraPred_Angular_Loop
H265_IntraPred_Angular_Last_LT_Loop_1_w32:       
        
        vld1.8  {d0, d1, d2, d3}, [r1]
        ldrb    r12, [r1, #32]
        vst1.8  {d0, d1, d2, d3}, [r7]        
        strb    r12, [r7, #32]
                
H265_IntraPred_Angular_Loop:        
        subs  r6, r6, #26
        movne r5, #0xff 					;@ 此时c_idx表示判断mode == 26 && c_idx == 0的结果，0表示为真，否则为假
        mov   r6, r8 							;@ tmp_media_value = angle;
        cmp   r4, #4
        beq  H265_IntraPred_Angular_Loop_size_4
       @; size = 8,16,32
        mov   r10, r4 						;@ y = size
        mov   r14, r0 						;@ p_curr_dst = p_dst;
        sub   r3, r3, r4 					;@ 修改后的跨度
H265_IntraPred_Angular_Loop_for_y:        
        add   r9, r7, r6, asr #5 	; @ ref+idx
        add   r9, r9, #1 					; @ p_curr_ref = ref + idx + 1;        
        ands  r12, r6, #31 				;@ fact
        beq   H265_IntraPred_Angular_Loop_for_y_fact_EQ
        rsb   r11, r12, #32 			;@ 32-fact
        vdup.8  d0, r11 						;@ d0=(32-fact) 
        vdup.8  d1, r12						;@ d1= fact
        mov   r11, r4 						;@ x =size;
        mov   r12, #8
H265_IntraPred_Angular_Loop_for_y_fact_NE:  @; for (x = 0; x < size; x++)     
        vld1.8 {d2,d3}, [r9], r12 		;@ d2: p_curr_ref[x] x=0~7
        vext.8 d4, d2, d3, #1 		;@ d4: p_curr_ref[x+1] x=0~7
        vmull.u8 q3, d2, d0 		;@ (32 - fact) * p_curr_ref[x]
        vmlal.u8 q3, d4, d1 		;@ fact * p_curr_ref[x + 1]        
        vqrshrn.u16 d6, q3, #5 	;@ p_curr_dst[x]
        vst1.8 {d6}, [r14]!
        subs  r11, r11, #8 				;@ x-=8
        bgt   H265_IntraPred_Angular_Loop_for_y_fact_NE
        
        subs  r10, r10,#1   			;@ y--
        add   r14, r14, r3 				;@ p_curr_dst += dst_stride;
        add   r6, r6, r8 					;@ tmp_media_value += angle;
        bgt  H265_IntraPred_Angular_Loop_for_y
        b    H265_IntraPred_Angular_END_Loop
        
H265_IntraPred_Angular_Loop_for_y_fact_EQ:
        cmp   r4, #32
        beq  H265_IntraPred_Angular_Loop_for_y_fact_EQ_w32
        cmp   r4, #16
        beq  H265_IntraPred_Angular_Loop_for_y_fact_EQ_w16
        vld1.8  {d0}, [r9]
        vst1.8  {d0}, [r14]!
        b    H265_IntraPred_Angular_Loop_for_y_fact_EQ_END
H265_IntraPred_Angular_Loop_for_y_fact_EQ_w16:        
        vld1.8  {d0, d1}, [r9]
        vst1.8  {d0, d1}, [r14]!
        b    H265_IntraPred_Angular_Loop_for_y_fact_EQ_END
H265_IntraPred_Angular_Loop_for_y_fact_EQ_w32:        
        vld1.8  {d0, d1, d2, d3}, [r9]
        vst1.8  {d0, d1, d2, d3}, [r14]!
        
H265_IntraPred_Angular_Loop_for_y_fact_EQ_END:        
        subs  r10, r10,#1   			;@ y--
        add   r14, r14, r3 				;@ p_curr_dst += dst_stride;
        add   r6, r6, r8 					;@ tmp_media_value += angle;
        bgt  H265_IntraPred_Angular_Loop_for_y
        
H265_IntraPred_Angular_END_Loop:        
        add  sp, sp, #100
        cmp  r5, #0
        ldmnefd sp!, {r4-r11, pc}
        cmp  r4, #32
        ldmeqfd sp!, {r4-r11, pc}
       @;  if (mode == 26 && c_idx == 0 && size < 32)       
        mov   r10, r4 						;@ y = size
        mov   r14, r0 						;@ p_curr_dst = p_dst;
        ldrb   r6, [r2], #1 						;@ left[-1], 此时r2=left
        ldrb   r5, [r1, #1]! 			;@ top[0]
        add  r3, r3, r4 				;@ dst_stride
H265_IntraPred_Angular_mode_26_Loop_for_y:        
        ldrb  r7, [r2], #1
        sub   r7, r7, r6 					;@ (left[y] - left[-1])
        add   r7, r5, r7, asr #1 @; top[0] + ((left[y] - first_left) >> 1)
        usat  r7, #8, r7 			;@ clip()
        strb  r7, [r14], r3
        subs  r10, r10,#1   			;@ y--        
        bgt  H265_IntraPred_Angular_mode_26_Loop_for_y
        ldmfd sp!, {r4-r11, pc}
        
        
H265_IntraPred_Angular_Loop_size_4:        
       @; mov   r6, r8 							;@ tmp_media_value = angle;
        mov   r10, r4 						;@ y = size
        mov   r14, r0 						;@ p_curr_dst = p_dst;
H265_IntraPred_Angular_Loop_size_4_for_y:        
        add   r9, r7, r6, asr #5 	; @ ref+idx
        add   r9, r9, #1 					; @ p_curr_ref = ref + idx + 1;        
        ands  r12, r6, #31 				;@ fact
        bne   H265_IntraPred_Angular_Loop_size_4_for_y_fact_NE
        vld1.32  {d6[0]}, [r9] @; fact==0时,p_curr_dst[x] = p_curr_ref[x];
        beq H265_IntraPred_Angular_Loop_size_4_for_y_end
H265_IntraPred_Angular_Loop_size_4_for_y_fact_NE:        
        rsb   r11, r12, #32 			;@ 32-fact
        vdup.8  d0, r11 						;@ d0=(32-fact) 
        vdup.8  d1, r12						;@ d1= fact
		  @; for (x = 0; x < 4; x++)     
        vld1.8 {d2}, [r9] 		;@ d2: p_curr_ref[x] x=0~3, 前32bit有效
        vext.8 d4, d2, d2, #1 		;@ d4: p_curr_ref[x+1] x=0~3， 前32bit有效
        vmull.u8 q3, d2, d0 		;@ (32 - fact) * p_curr_ref[x]
        vmlal.u8 q3, d4, d1 		;@ fact * p_curr_ref[x + 1]        
        vqrshrn.u16 d6, q3, #5 	;@ p_curr_dst[x]
H265_IntraPred_Angular_Loop_size_4_for_y_end:        
        vst1.32 {d6[0]}, [r14],r3
        subs  r10, r10,#1   			;@ y--
        add   r6, r6, r8 					;@ tmp_media_value += angle;
        bgt  H265_IntraPred_Angular_Loop_size_4_for_y
        sub   r3, r3, r4 					;@ 修改跨度
        b  H265_IntraPred_Angular_END_Loop
        
        
H265_IntraPred_Angular_mode_LT18_ASM:   
        stmfd sp!, {r4-r11, lr}
        add   r12, sp, #36
        ldmia r12, {r4 - r6} 			;@ size，c_idx, mode
        sub   sp, sp, #1024  @; 32*32
        sub   sp, sp, #100 				;@ ref_array[3*32+1]
        stmfd  sp!, {r0, r3}
        
        
        ldr    r12, =intra_pred_angle
        sub   r8, r6, #2 					;@ mode-2
        ldrsb  r8, [r12,r8] 				;@ angle = intra_pred_angle[mode-2];
         
        mul    r9, r8, r4 				;@ (angle * size) 
       @;cmp    r6, #18 						;@ if (mode >= 18)      
       @; blt    H265_IntraPred_Angular_mode_LT18_mode_LT18
       @; swap the top and left address
        mov  r12, r1
        mov  r1, r2
        mov  r2, r12
        add  r0, sp, #108 		;@ p_tmp_dst
        mov  r3, r4 
        
       @; H265_IntraPred_Angular_mode_LT18_mode_GE_18   
        mov    r12, #1
        adds   r9, r12, r9, asr #5 		;@ last+1   
        sub    r7, r1, #1 				;@ ref = top - 1;
        sub    r1, r1, #1 					;@ p_orig_top = top - 1;
        sub    r2, r2, #1 						;@ p_orig_left = left - 1;
        bge    H265_IntraPred_Angular_mode_LT18_Loop
       @; if (/*angle < 0 &&*/ last < -1) 
        add    r7, sp, r4 				;@ ref = ref_array + size;
        add    r7, r7, #8
        ldr    r12, =inv_angle 		;@ inv_angle
        sub    r10, r6, #11 				;@ mode-11
        
        ldr    r10, [r12, r10, lsl #2] 	;@ inv_angle_mode_value = inv_angle[mode-11];
        sub    r9, r9, #1 					;@ last, 必定<-1
        mov    r11, #128
H265_IntraPred_Angular_mode_LT18_Last_LT_Loop_1:    @; for (x = last; x <= -1; x++)   
        mla    r12,  r9, r10, r11 				;@ x * inv_angle_mode_value + 128
        ldrb   r12,  [r2, r12, asr #8]  @; p_orig_left[((x * inv_angle_mode_value + 128) >> 8)];
        
        strb   r12,  [r7, r9] 						;@ ref[x]        
        adds   r9, r9, #1        
        blt   H265_IntraPred_Angular_mode_LT18_Last_LT_Loop_1
       @; for (x = 0; x <= size; x++)
        cmp   r4, #32
        beq   H265_IntraPred_Angular_mode_LT18_Last_LT_Loop_1_w32
        cmp   r4, #16
        beq   H265_IntraPred_Angular_mode_LT18_Last_LT_Loop_1_w16
        cmp   r4, #8
        beq   H265_IntraPred_Angular_mode_LT18_Last_LT_Loop_1_w8
        
        vld1.32 {d0[0]}, [r1]
        ldrb    r12, [r1, #4]
        vst1.32 {d0[0]}, [r7]        
        strb    r12, [r7, #4]
        b H265_IntraPred_Angular_mode_LT18_Loop
H265_IntraPred_Angular_mode_LT18_Last_LT_Loop_1_w8:        
        
        vld1.8  {d0}, [r1]
        ldrb    r12, [r1, #8]
        vst1.8  {d0}, [r7]        
        strb    r12, [r7, #8]
        b H265_IntraPred_Angular_mode_LT18_Loop
H265_IntraPred_Angular_mode_LT18_Last_LT_Loop_1_w16:        
        cmp   r4, #16
        vld1.8  {d0, d1}, [r1]
        ldrb    r12, [r1, #16]
        vst1.8  {d0, d1}, [r7]        
        strb    r12, [r7, #16]
        b H265_IntraPred_Angular_mode_LT18_Loop
H265_IntraPred_Angular_mode_LT18_Last_LT_Loop_1_w32:        
        
        vld1.8  {d0, d1, d2, d3}, [r1]
        ldrb    r12, [r1, #32]
        vst1.8  {d0, d1, d2, d3}, [r7]        
        strb    r12, [r7, #32]
                
H265_IntraPred_Angular_mode_LT18_Loop:        
        
        sub r12, r6, #10
        orr  r5, r5, r6, lsl #8 @; 含有两个变量，0~7表示mode == 10 && c_idx == 0，8~15bit表示mode
        
H265_IntraPred_Angular_mode_LT18_Loop_COMMOM:        
        mov   r6, r8 							;@ tmp_media_value = angle;
        cmp   r4, #4
        beq  H265_IntraPred_Angular_mode_LT18_Loop_size_4
       @; size = 8,16,32
        mov   r10, r4 						;@ y = size
        mov   r14, r0 						;@ p_curr_dst = p_dst;
        sub   r3, r3, r4 					;@ 修改后的跨度
H265_IntraPred_Angular_mode_LT18_Loop_for_y:        
        add   r9, r7, r6, asr #5 	; @ ref+idx
        add   r9, r9, #1 					; @ p_curr_ref = ref + idx + 1;        
        ands  r12, r6, #31 				;@ fact
        beq   H265_IntraPred_Angular_mode_LT18_Loop_for_y_fact_EQ
        rsb   r11, r12, #32 			;@ 32-fact
        vdup.8  d0, r11 						;@ d0=(32-fact) 
        vdup.8  d1, r12						;@ d1= fact
        mov   r11, r4 						;@ x =size;
        mov   r12, #8
H265_IntraPred_Angular_mode_LT18_Loop_for_y_fact_NE:  @; for (x = 0; x < size; x++)     
        vld1.8 {d2,d3}, [r9], r12 		;@ d2: p_curr_ref[x] x=0~7
        vext.8 d4, d2, d3, #1 		;@ d4: p_curr_ref[x+1] x=0~7
        vmull.u8 q3, d2, d0 		;@ (32 - fact) * p_curr_ref[x]
        vmlal.u8 q3, d4, d1 		;@ fact * p_curr_ref[x + 1]        
        vqrshrn.u16 d6, q3, #5 	;@ p_curr_dst[x]
        vst1.8 {d6}, [r14]!
        subs  r11, r11, #8 				;@ x-=8
        bgt   H265_IntraPred_Angular_mode_LT18_Loop_for_y_fact_NE
        
        subs  r10, r10,#1   			;@ y--
        add   r14, r14, r3 				;@ p_curr_dst += dst_stride;
        add   r6, r6, r8 					;@ tmp_media_value += angle;
        bgt  H265_IntraPred_Angular_mode_LT18_Loop_for_y
        b    H265_IntraPred_Angular_mode_LT18_END_Loop_LT_18
        
H265_IntraPred_Angular_mode_LT18_Loop_for_y_fact_EQ:
        cmp   r4, #32
        beq  H265_IntraPred_Angular_mode_LT18_Loop_for_y_fact_EQ_w32
        cmp   r4, #16
        beq  H265_IntraPred_Angular_mode_LT18_Loop_for_y_fact_EQ_w16
        vld1.8  {d0}, [r9]
        vst1.8  {d0}, [r14]!
        b    H265_IntraPred_Angular_mode_LT18_Loop_for_y_fact_EQ_END
H265_IntraPred_Angular_mode_LT18_Loop_for_y_fact_EQ_w16:        
        vld1.8  {d0, d1}, [r9]
        vst1.8  {d0, d1}, [r14]!
        b    H265_IntraPred_Angular_mode_LT18_Loop_for_y_fact_EQ_END
H265_IntraPred_Angular_mode_LT18_Loop_for_y_fact_EQ_w32:        
        vld1.8  {d0, d1, d2, d3}, [r9]
        vst1.8  {d0, d1, d2, d3}, [r14]!
        
H265_IntraPred_Angular_mode_LT18_Loop_for_y_fact_EQ_END:        
        subs  r10, r10,#1   			;@ y--
        add   r14, r14, r3 				;@ p_curr_dst += dst_stride;
        add   r6, r6, r8 					;@ tmp_media_value += angle;
        bgt  H265_IntraPred_Angular_mode_LT18_Loop_for_y
        b   H265_IntraPred_Angular_mode_LT18_END_Loop_LT_18
        
        
        
H265_IntraPred_Angular_mode_LT18_Loop_size_4:        
       @; mov   r6, r8 							;@ tmp_media_value = angle;
        mov   r10, r4 						;@ y = size
        mov   r14, r0 						;@ p_curr_dst = p_dst;
H265_IntraPred_Angular_mode_LT18_Loop_size_4_for_y:        
        add   r9, r7, r6, asr #5 	; @ ref+idx
        add   r9, r9, #1 					; @ p_curr_ref = ref + idx + 1;        
        ands  r12, r6, #31 				;@ fact
        bne   H265_IntraPred_Angular_mode_LT18_Loop_size_4_for_y_fact_NE
        vld1.32  {d6[0]}, [r9] @; fact==0时,p_curr_dst[x] = p_curr_ref[x];
        beq H265_IntraPred_Angular_mode_LT18_Loop_size_4_for_y_end
H265_IntraPred_Angular_mode_LT18_Loop_size_4_for_y_fact_NE:        
        rsb   r11, r12, #32 			;@ 32-fact
        vdup.8  d0, r11 						;@ d0=(32-fact) 
        vdup.8  d1, r12						;@ d1= fact
		  @; for (x = 0; x < 4; x++)     
        vld1.8 {d2}, [r9] 		;@ d2: p_curr_ref[x] x=0~4, 前32bit有效
        vext.8 d4, d2, d2, #1 		;@ d4: p_curr_ref[x+1] x=0~3， 前32bit有效
        vmull.u8 q3, d2, d0 		;@ (32 - fact) * p_curr_ref[x]
        vmlal.u8 q3, d4, d1 		;@ fact * p_curr_ref[x + 1]        
        vqrshrn.u16 d6, q3, #5 	;@ p_curr_dst[x]
H265_IntraPred_Angular_mode_LT18_Loop_size_4_for_y_end:        
        vst1.32 {d6[0]}, [r14],r3
        subs  r10, r10,#1   			;@ y--
        add   r6, r6, r8 					;@ tmp_media_value += angle;
        bgt  H265_IntraPred_Angular_mode_LT18_Loop_size_4_for_y
        sub   r3, r3, r4 					;@ 修改跨度
       @; b  H265_IntraPred_Angular_mode_LT18_END_Loop_LT_18
        
        
H265_IntraPred_Angular_mode_LT18_END_Loop_LT_18:        
        
        ldmia sp!, {r0, r3}
       @;根据不同大小进行转置
        add  r6, sp, #100 	;@p_dst_tmp
        mov  r8, r0
        cmp  r4, #32
        beq  H265_IntraPred_Angular_mode_LT18_END_Loop_LT_18_RVS_32
        cmp  r4, #16
        beq  H265_IntraPred_Angular_mode_LT18_END_Loop_LT_18_RVS_16
        cmp  r4, #8
        beq  H265_IntraPred_Angular_mode_LT18_END_Loop_LT_18_RVS_8
       @; cmp  r4, #4
        vld4.8 {d0, d1, d2, d3}, [r6]        
        vst1.32 {d0[0]}, [r8], r3
        vst1.32 {d1[0]}, [r8], r3
        vst1.32 {d2[0]}, [r8], r3
        vst1.32 {d3[0]}, [r8], r3
        b    H265_IntraPred_Angular_mode_LT18_END_Loop_LT_18_RVS_END
H265_IntraPred_Angular_mode_LT18_END_Loop_LT_18_RVS_8:        
        vld4.32 {d0,d2,d4,d6}, [r6]!
        vld4.32 {d1,d3,d5,d7}, [r6]!
        vzip.8   q0,  q2
        vzip.16  d0,  d1
        vzip.16  d4,  d5
        vzip.32  q0,  q2
        vzip.8   q1,  q3
        vzip.16  d2,  d3
        vzip.16  d6,  d7
        vzip.32  q1,  q3
        vst1.8 {d0}, [r8], r3
        vst1.8 {d1}, [r8], r3        
        vst1.8 {d4}, [r8], r3
        vst1.8 {d5}, [r8], r3
        vst1.8 {d2}, [r8], r3
        vst1.8 {d3}, [r8], r3        
        vst1.8 {d6}, [r8], r3
        vst1.8 {d7}, [r8], r3
        b    H265_IntraPred_Angular_mode_LT18_END_Loop_LT_18_RVS_END
H265_IntraPred_Angular_mode_LT18_END_Loop_LT_18_RVS_16:        
        
       @; 两个8*8子块
        vld1.8 {d0,d1,d2,d3}, [r6]!
        vld1.8 {d4,d5,d6,d7}, [r6]!
        vld1.8 {d8,d9,d10,d11}, [r6]!
        vld1.8 {d12,d13,d14,d15}, [r6]!
        vtrn.8   q0, q1
        vtrn.8   q2, q3
        vtrn.8   q4, q5
        vtrn.8   q6, q7                        
        vtrn.16   q0, q2
        vtrn.16   q1, q3
        vtrn.16   q4, q6
        vtrn.16   q5, q7  
        vtrn.32   q0, q4
        vtrn.32   q1, q5
        vtrn.32   q2, q6
        vtrn.32   q3, q7       
        vst1.8 {d0}, [r8], r3
        vst1.8 {d2}, [r8], r3
        vst1.8 {d4}, [r8], r3
        vst1.8 {d6}, [r8], r3
        vst1.8 {d8}, [r8], r3
        vst1.8 {d10}, [r8], r3
        vst1.8 {d12}, [r8], r3
        vst1.8 {d14}, [r8], r3
        vst1.8 {d1}, [r8], r3        
        vst1.8 {d3}, [r8], r3        
        vst1.8 {d5}, [r8], r3        
        vst1.8 {d7}, [r8], r3        
        vst1.8 {d9}, [r8], r3        
        vst1.8 {d11}, [r8], r3        
        vst1.8 {d13}, [r8], r3        
        vst1.8 {d15}, [r8], r3
        
        sub   r8, r8, r3, lsl #4 	;@ 还原到初始p_dst
        add   r8, r8, #8
        
       @; 剩下两个8*8子块
        vld1.8 {d0,d1,d2,d3}, [r6]!
        vld1.8 {d4,d5,d6,d7}, [r6]!
        vld1.8 {d8,d9,d10,d11}, [r6]!
        vld1.8 {d12,d13,d14,d15}, [r6]!
        vtrn.8   q0, q1
        vtrn.8   q2, q3
        vtrn.8   q4, q5
        vtrn.8   q6, q7                        
        vtrn.16   q0, q2
        vtrn.16   q1, q3
        vtrn.16   q4, q6
        vtrn.16   q5, q7  
        vtrn.32   q0, q4
        vtrn.32   q1, q5
        vtrn.32   q2, q6
        vtrn.32   q3, q7       
        vst1.8 {d0}, [r8], r3
        vst1.8 {d2}, [r8], r3
        vst1.8 {d4}, [r8], r3
        vst1.8 {d6}, [r8], r3
        vst1.8 {d8}, [r8], r3
        vst1.8 {d10}, [r8], r3
        vst1.8 {d12}, [r8], r3
        vst1.8 {d14}, [r8], r3
        vst1.8 {d1}, [r8], r3        
        vst1.8 {d3}, [r8], r3        
        vst1.8 {d5}, [r8], r3        
        vst1.8 {d7}, [r8], r3        
        vst1.8 {d9}, [r8], r3        
        vst1.8 {d11}, [r8], r3        
        vst1.8 {d13}, [r8], r3        
        vst1.8 {d15}, [r8], r3              
        
        b    H265_IntraPred_Angular_mode_LT18_END_Loop_LT_18_RVS_END
H265_IntraPred_Angular_mode_LT18_END_Loop_LT_18_RVS_32:        
        mov  r7, #16
H265_IntraPred_Angular_mode_LT18_END_Loop_LT_18_RVS_32_Block4:        
       @; 4个8*8子块
        vld1.8 {d0,d1,d2,d3}, [r6]!
        vld1.8 {d4,d5,d6,d7}, [r6]!
        vld1.8 {d8,d9,d10,d11}, [r6]!
        vld1.8 {d12,d13,d14,d15}, [r6]!
        vld1.8 {d16,d17,d18,d19}, [r6]!
        vld1.8 {d20,d21,d22,d23}, [r6]!
        vld1.8 {d24,d25,d26,d27}, [r6]!
        vld1.8 {d28,d29,d30,d31}, [r6]!
        vtrn.8   q0, q2
        vtrn.8   q4, q6
        vtrn.8   q8, q10
        vtrn.8   q12, q14                        
        vtrn.16   q0, q4
        vtrn.16   q2, q6
        vtrn.16   q8, q12
        vtrn.16   q10, q14  
        vtrn.32   q0, q8
        vtrn.32   q2, q10
        vtrn.32   q4, q12
        vtrn.32   q6, q14      
        vst1.8 {d0}, [r8], r3
        vst1.8 {d4}, [r8], r3
        vst1.8 {d8}, [r8], r3
        vst1.8 {d12}, [r8], r3
        vst1.8 {d16}, [r8], r3
        vst1.8 {d20}, [r8], r3
        vst1.8 {d24}, [r8], r3
        vst1.8 {d28}, [r8], r3
        
        vst1.8 {d1}, [r8], r3
        vst1.8 {d5}, [r8], r3
        vst1.8 {d9}, [r8], r3
        vst1.8 {d13}, [r8], r3
        vst1.8 {d17}, [r8], r3
        vst1.8 {d21}, [r8], r3
        vst1.8 {d25}, [r8], r3
        vst1.8 {d29}, [r8], r3
        
        vtrn.8   q1, q3
        vtrn.8   q5, q7
        vtrn.8   q9, q11
        vtrn.8   q13, q15                        
        vtrn.16   q1, q5
        vtrn.16   q3, q7
        vtrn.16   q9, q13
        vtrn.16   q11, q15 
        vtrn.32   q1, q9
        vtrn.32   q3, q11
        vtrn.32   q5, q13
        vtrn.32   q7, q15
              
        vst1.8 {d2}, [r8], r3
        vst1.8 {d6}, [r8], r3
        vst1.8 {d10}, [r8], r3
        vst1.8 {d14}, [r8], r3
        vst1.8 {d18}, [r8], r3
        vst1.8 {d22}, [r8], r3
        vst1.8 {d26}, [r8], r3
        vst1.8 {d30}, [r8], r3
        
        vst1.8 {d3}, [r8], r3
        vst1.8 {d7}, [r8], r3
        vst1.8 {d11}, [r8], r3
        vst1.8 {d15}, [r8], r3
        vst1.8 {d19}, [r8], r3
        vst1.8 {d23}, [r8], r3
        vst1.8 {d27}, [r8], r3
        vst1.8 {d31}, [r8], r3
        
        sub   r8, r8, r3, lsl #5 	;@ 还原到初始p_dst
        add   r8, r8, #8
        subs  r7, r7, #4
        bgt H265_IntraPred_Angular_mode_LT18_END_Loop_LT_18_RVS_32_Block4
               
        
H265_IntraPred_Angular_mode_LT18_END_Loop_LT_18_RVS_END:        
        add  sp, sp, #1024        
        add  sp, sp, #100
        mov  r6, r5, asr #8 	;@ mode
        cmp   r6, #10
        ldmnefd sp!, {r4-r11, pc} 
        ands  r5, #0xff
        ldmnefd sp!, {r4-r11, pc}
        cmp  r4, #32
        ldmeqfd sp!, {r4-r11, pc}
       @;  if (mode == 10 && c_idx == 0 && size < 32)       
        
        mov   r14, r0 						;@ p_curr_dst = p_dst;
        ldrb   r6, [r2], #1 						;@ left[-1], 此时r2=left
        ldrb   r5, [r1, #1]! 			;@ top[0]
        vdup.8 d0, r6
        vdup.16 q1, r5
        cmp  r4, #4 				;@ dst_stride
        beq  H265_IntraPred_Angular_mode_LT18_mode_10_Loop_for_x_w4
H265_IntraPred_Angular_mode_LT18_mode_10_Loop_for_x_wOther:        
        vld1.8 {d4}, [r2]! 		;@ 8B
       @;vhsubu8.  d6, d4, d0 @; ((left[y] - first_left) >> 1) 结果不一定为负数，那么就是错的********
        vsubl.u8  q3, d4, d0 @; (left[y] - first_left)
        vshr.s16  q3, q3, #1
        vadd.s16  q3, q3, q1 @; clip(top[0] + ((left[y] - first_left) >> 1))
        vqmovun.s16 d6, q3
        vst1.8   {d6}, [r14]!
        subs  r4, r4,#8   			;@ x-=8        
        bgt H265_IntraPred_Angular_mode_LT18_mode_10_Loop_for_x_wOther         
        ldmfd sp!, {r4-r11, pc}
        
H265_IntraPred_Angular_mode_LT18_mode_10_Loop_for_x_w4:       
        vld1.32   {d4[0]}, [r2] 		;@ 4B
       @;vhsubu8.  d6, d4, d0 @; ((left[y] - first_left) >> 1) 结果不一定为负数，那么就是错的********
        vsubl.u8  q3, d4, d0 @; (left[y] - first_left)
        vshr.s16  q3, q3, #1
        vadd.s16  q3, q3, q1 @; clip(top[0] + ((left[y] - first_left) >> 1))
        vqmovun.s16 d6, q3
        vst1.32   {d6[0]}, [r14]               
        ldmfd sp!, {r4-r11, pc}

	
	 
	 .endif											;@ if IDCT_ASM_ENABLED==1
	 .end
	 
        
	 
	 
	 
	 		 