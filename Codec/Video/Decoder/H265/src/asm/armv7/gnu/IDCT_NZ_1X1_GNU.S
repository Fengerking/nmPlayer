@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@;VO_VOID IDCT_NZ_1X1(const VO_S16 invtransformValue,
@;                   const VO_U32 tuWidth,
@;                   const VO_U8 *p_prediction_buff,
@;                   VO_U8 *p_reconstruction_buff,
@;                   const VO_U32 reconstruction_stride)
@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

				@;include		h265dec_ASM_config.h   
        @;include h265dec_idct_macro.inc
        @;area |.text|, code, readonly 
        @;align 4
        @;if IDCT_ASM_ENABLED==1  
        @;export IDCT_NZ_1X1_ASMV7
        @;
        #include     "h265dec_ASM_config.h"
        @#include "../../../h265dec_config.h"
        .section .text
        .align 4
        .if IDCT_ASM_ENABLED==1
        .global IDCT_NZ_1X1_ASMV7     
        
IDCT_NZ_1X1_ASMV7:  
        stmfd   sp!, {r4, r5, r6, r7, lr}
        ldrsh    r0, [r0] 										@; invtransformValue
        ldr     r4, [sp, #20] 							@; prediction_stride
       @; mov     r5, #PRED_CACHE_STRIDE 			@; prediction_stride 
        ldr     r5, [sp, #24] 							@; *type_tuWidth   
        add     r0, r0, #65 								@; (invtransformValue + 1 + add)
        ldrh    r5, [r5] 										@; width
        mov     r0, r0, asr #7 							@; resiValue
        vdup.16 q0, r0 										  @; 8个resiValue        
        cmp     r5, #4
        beq     IDCT_NZ_1X1_ASMV7_CASE_WIDTH4
        sub     r3, r3, r5
        sub     r4, r4, r5
        mov     r7, r5
IDCT_NZ_1X1_ASMV7_For_j:       
        mov     r6, r5
IDCT_NZ_1X1_ASMV7_For_k:        
        vld1.8     {d2}, [r1]! 							@; 8个p_prediction[k]
        vaddw.u8 q2, q0, d2 						  @;p_reconstruction[k]
        vqmovun.s16  d4, q2
        vst1.8     {d4}, [r2]!
        subs       r6, r6, #8 									@; tuWidth
        bgt        IDCT_NZ_1X1_ASMV7_For_k
        add        r2, r2, r3
        add        r1, r1, r4
        subs       r7, r7, #1 									@; j--
        bgt        IDCT_NZ_1X1_ASMV7_For_j
        
        ldmfd  sp!, {r4, r5, r6, r7, pc}
        
IDCT_NZ_1X1_ASMV7_CASE_WIDTH4:       
        @; one row
        pld      [r1]
        vld1.32     {d2[0]}, [r1], r4 							@; 4个p_prediction[k]
        vaddw.u8 	q2, q0, d2 						  @;p_reconstruction[k]
        vqmovun.s16  d4, q2
        vst1.32     {d4[0]}, [r2], r3
        @; two row
        vld1.32     {d2[0]}, [r1], r4 							@; 4个p_prediction[k]
        vaddw.u8 q2, q0, d2 						  @;p_reconstruction[k]
        vqmovun.s16  d4, q2
        vst1.32     {d4[0]}, [r2], r3
        @; three row
        vld1.32     {d2[0]}, [r1], r4 							@; 4个p_prediction[k]
        vaddw.u8 q2, q0, d2 						  @;p_reconstruction[k]
        vqmovun.s16  d4, q2
        vst1.32     {d4[0]}, [r2], r3
        @; four row
        vld1.32     {d2[0]}, [r1], r4 							@; 4个p_prediction[k]
        vaddw.u8 q2, q0, d2 						  @;p_reconstruction[k]
        vqmovun.s16  d4, q2
        vst1.32     {d4[0]}, [r2], r3
        
        ldmfd  sp!, {r4, r5, r6, r7, pc}
        
        .endif											@@;if IDCT_ASM_ENABLED==1
        .end
        
        