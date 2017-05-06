;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
;@VO_VOID IDST4X4_1X1(const VO_S16 invtransformValue, 
;@             const VO_U8 *p_perdiction_buff,        
;@             VO_U8 *p_reconstruction_buff,          
;@             const VO_U32 reconstruction_stride)    
;@
;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
;@V5 =V4+adjust pipeline
        include		h265dec_ASM_config.h   
        ;@include h265dec_idct_macro.inc
        area |.text|, code, readonly 
        align 4
        if IDCT_ASM_ENABLED==1  
        export IDST4X4_1X1_ASMV7
        
IDST4X4_1X1_ASMV7  
        stmfd   sp!, {r4, r5, lr}
        ldrsh    r0, [r0]
        vdup.16 d1, r0 										  ;@ 4个invtransformValue
        ldr     r4, = 0x0037001d 						;@ 55 | 29
        ldr     r5, = 0x0054004a 						;@ 84 | 74
        ;@mov     r0, #PRED_CACHE_STRIDE 			;@ prediction_stride
        ldr    r0, [sp, #12]
        vmov    d0, r4, r5 									;@ 84 | 74 | 55 | 29
        vmull.s16 q1, d0, d1
        vqrshrn.s32 d1, q1, # 7 						;@ tmp_block[0~3]
      
        ;@ one row
        vmull.s16   q2, d0, d1[0]
        vqrshrn.s32 d4, q2, #12
        vld1.32     {d2[0]}, [r1], r0 			;@ 4个p_prediction[k]
        vaddw.u8 	q2, q2, d2 						  ;@p_reconstruction[k]
        vqmovun.s16  d4, q2 								;@ p_target_block[0~3]
        vst1.32     {d4[0]}, [r2], r3
        
        
        
        ;@ two row
        vmull.s16   q2, d0, d1[1]
        vqrshrn.s32 d4, q2, #12
        vld1.32     {d2[0]}, [r1], r0 			;@ 4个p_prediction[k]
        vaddw.u8 	q2, q2, d2 						  ;@p_reconstruction[k]
        vqmovun.s16  d4, q2 								;@ p_target_block[0~3]
        vst1.32     {d4[0]}, [r2], r3
        
        ;@ three row
        vmull.s16   q2, d0, d1[2]
        vqrshrn.s32 d4, q2, #12
        vld1.32     {d2[0]}, [r1], r0 			;@ 4个p_prediction[k]
        vaddw.u8 	q2, q2, d2 						  ;@p_reconstruction[k]
        vqmovun.s16  d4, q2 								;@ p_target_block[0~3]
        vst1.32     {d4[0]}, [r2], r3
        
        ;@ four row
        vmull.s16   q2, d0, d1[3]
        vqrshrn.s32 d4, q2, #12
        vld1.32     {d2[0]}, [r1], r0 			;@ 4个p_prediction[k]
        vaddw.u8 	q2, q2, d2 						  ;@p_reconstruction[k]
        vqmovun.s16  d4, q2 								;@ p_target_block[0~3]
        vst1.32     {d4[0]}, [r2], r3
        
        ldmfd  sp!, {r4, r5, pc}
        
        endif											;if IDCT_ASM_ENABLED==1
        end
        
        