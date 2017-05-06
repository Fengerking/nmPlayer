;/************************************************************************
; VisualOn Proprietary
; Copyright (c) 2012, VisualOn Incorporated. All rights Reserved
;
; VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
;
; All data and information contained in or disclosed by this document are
; confidential and proprietary information of VisualOn, and all rights
; therein are expressly reserved. By accepting this material, the
; recipient agrees that this material and the information contained
; therein are held in confidence and in trust. The material may only be
; used and/or disclosed as authorized in a license agreement controlling
; such use and disclosure.
;************************************************************************/
%include "x86util.asm"
%include "h265dec_ASM_config.asm"

%include "h265dec_ID.asm"    ;include it to hide your symbol names


cextern CONST_U8_0;

cextern CONST_S16_NEG1;
cextern CONST_S16_POS1;
cextern CONST_S16_POS4;
cextern CONST_S16_NEG10;
cextern CONST_S16_POS58;
cextern CONST_S16_POS17;
cextern CONST_S16_NEG5;
cextern CONST_S16_POS11;
cextern CONST_S16_NEG11;
cextern CONST_S16_POS40;
cextern CONST_U16_POS32;
cextern CONST_U16_POS64;

cextern CONST_S8_F1_01;
cextern CONST_S8_F1_23;
cextern CONST_S8_F1_45;
cextern CONST_S8_F1_67;

cextern CONST_S8_F2_01;
cextern CONST_S8_F2_23;
cextern CONST_S8_F2_45;
cextern CONST_S8_F2_67;

cextern CONST_S8_F3_01;
cextern CONST_S8_F3_23;
cextern CONST_S8_F3_45;
cextern CONST_S8_F3_67;
cextern g_chromaFilter_x86;
cextern g_chromaFilter_x86_8;

cextern CONST_S32_POS4;
cextern CONST_S32_NEG10;
cextern CONST_S32_POS58;
cextern CONST_S32_POS17;
cextern CONST_S32_NEG5;
cextern CONST_S32_POS11;
cextern CONST_S32_NEG11;
cextern CONST_S32_POS40;
cextern CONST_U32_POS32;
cextern CONST_U32_POS2048;



%define block_temp_stride 16

section  .text
align    4  

stub:
    nop
    nop
    ret

%macro x_3y_3 0
    sub                 pSrcq, nSrcStrideq
    sub                 pSrcq, nSrcStrideq
    sub                 pSrcq, nSrcStrideq
    sub                 pSrcq, 3
%endmacro

%macro x_2y_3 0
    sub                 pSrcq, nSrcStrideq
    sub                 pSrcq, nSrcStrideq
    sub                 pSrcq, nSrcStrideq			
    sub                 pSrcq, 2
%endmacro

%macro x_3y_2 0
    sub                 pSrcq, nSrcStrideq
    sub                 pSrcq, nSrcStrideq
    sub                 pSrcq, 3
%endmacro

%macro x_2y_2 0
    sub                 pSrcq, nSrcStrideq
    sub                 pSrcq, nSrcStrideq
    sub                 pSrcq, 2
%endmacro

%macro x_0y_3 0
    sub                 pSrcq, nSrcStrideq
    sub                 pSrcq, nSrcStrideq
    sub                 pSrcq, nSrcStrideq
%endmacro

%macro x_0y_2 0
    sub                 pSrcq, nSrcStrideq
    sub                 pSrcq, nSrcStrideq
%endmacro

%macro x_3y_0 0
    sub                 pSrcq, 3	
%endmacro

%macro x_2y_0 0
    sub                 pSrcq, 2
%endmacro

;input:   regs, must xmm or mm
;output: 
;args:
%macro round_7bits       1
    paddsw               %1, %1, [CONST_U16_POS64]
    psraw               %1, %1, 7
%endmacro

;input: 
;output: 
;args:
;role: set HxVx_H_result = pDst2(asm_mc0_buff/asm_mc1_buff) + 20180  , only for H!=0&V!=0 case 
%macro  set_HxVx_H_result 0     
    mov                 nDstStrideq, pDst2mp
%if ARCH_X86_64
    and                 nDstStrideq, 0xfffffffffffffff0 ;align to asm_mc0_buff/asm_mc1_buff
%else
    and                 nDstStrideq, 0xfffffff0 
%endif
    add                 nDstStrideq, 8192  ;jump original (71*2*71*2+16) bytes
%if ARCH_X86_64
    and                 nDstStrideq, 0xfffffffffffffff0 ;align @16byte
%else
    and                 nDstStrideq, 0xfffffff0 
%endif
    mov                 HxVx_H_resultmp, nDstStrideq
    mov                 nDstStrideq, nDstStridemp
%endmacro

;input: mm0 ~ mm7, 16 bit(from 8 bit yuv then zero extend)
;output: mm6
;args:  %narrow  
%macro  v1_4 1 %narrow   
    psubw               mm6, mm6, mm0      ; (A[6]-A[0])
    movq                mm7, mm3             ; NOT change mm1, mm2, mm3
    movq                mm0, mm2             ; NOT change mm1, mm2, mm3
 
    psllw               mm1, mm1,  2    ; 4*A[1], NOT change mm1, mm2, mm3
    pmullw              mm0, mm0,  [CONST_S16_NEG10]    ; -10*A[2]
    pmullw              mm7, mm7,  [CONST_S16_POS58]    ; 58*A[3]
    pmullw              mm4, mm4,  [CONST_S16_POS17]    ; 17*A[4]
    pmullw              mm5, mm5,  [CONST_S16_NEG5]    ; -5*A[5]

    paddsw              mm0, mm0, mm1
    paddsw              mm7, mm7, mm4
    paddsw              mm6, mm6, mm5
    paddsw              mm0, mm0, mm7
    paddsw              mm6, mm6, mm0

    psrlw               mm1, mm1,  2    ; NOT change mm1, mm2, mm3

    %ifidn %1, true
        paddsw          mm6, mm6, [CONST_U16_POS32]
        psraw           mm6, mm6, 6
        packuswb        mm6, mm6, mm2
    %endif
 
%endmacro

;input: mm0 ~ mm7, 16 bit(from 8 bit yuv then zero extend)
;output: mm6
;args:  %narrow  
%macro  v2_4 1 %narrow   

    paddw               mm7, mm7, mm0      ; (A[0]+A[7])
    paddw               mm6, mm6, mm1   
    paddw               mm5, mm5, mm2   
    paddw               mm4, mm4, mm3   

    pmullw              mm6, mm6,  [CONST_S16_POS4]    ; 4*(A[1]+A[6])
    pmullw              mm5, mm5,  [CONST_S16_POS11]    ; 11*(A[2]+A[5])
    pmullw              mm4, mm4,  [CONST_S16_POS40]    ; 40*(A[3]+A[4])

    paddsw              mm6, mm6, mm4
    paddsw              mm7, mm7, mm5
    psubsw              mm6, mm6, mm7

    %ifidn %1, true
        paddsw          mm6, mm6, [CONST_U16_POS32]
        psraw           mm6, mm6, 6
        packuswb        mm6, mm6, mm2
    %endif
%endmacro

;input: mm0 ~ mm7, 16 bit(from 8 bit yuv then zero extend)
;output: mm6
;args:  %narrow  
%macro  v3_4 1 %narrow   
    movq                mm0, mm1             ; NOT change mm1, mm2, mm3
    psubw               mm0, mm0, mm7      ; (A[1]-A[7])

    movq                mm7, mm2             ; NOT change mm1, mm2, mm3
  
    pmullw              mm7, mm7,  [CONST_S16_NEG5]    ; -5*A[2]
    pmullw              mm3, mm3,  [CONST_S16_POS17]    ; 17*A[3]
    pmullw              mm4, mm4,  [CONST_S16_POS58]    ; 58*A[4]
    pmullw              mm5, mm5,  [CONST_S16_NEG10]    ; -10*A[5]
    psllw               mm6, mm6,  2                    ; 4*A[6]
   
   
    paddsw              mm4, mm4, mm0
    paddsw              mm5, mm5, mm7
    paddsw              mm6, mm6, mm3
    paddsw              mm4, mm4, mm5
    paddsw              mm6, mm6, mm4   

    %ifidn %1, true
        paddsw          mm6, mm6, [CONST_U16_POS32]
        psraw           mm6, mm6, 6
        packuswb        mm6, mm6, mm2
    %endif
 
%endmacro

;input: pSrcq, is 16 bit(from 16 bit H filter)
;output: V_tempmx, 32 bit. (16*16->32)
;args:  %offset 
%macro  v1_4_2_mul 1 
    pmullw              xmm0, [pSrcq], [CONST_S16_NEG1]
    pmulhw              xmm1, [pSrcq], [CONST_S16_NEG1]
    add                 pSrcq, block_temp_stride
    pmullw              xmm2, [pSrcq], [CONST_S16_POS4]
    pmulhw              xmm3, [pSrcq], [CONST_S16_POS4]
    add                 pSrcq, block_temp_stride
    pmullw              xmm4, [pSrcq], [CONST_S16_NEG10]
    pmulhw              xmm5, [pSrcq], [CONST_S16_NEG10]
    add                 pSrcq, block_temp_stride
    pmullw              xmm6, [pSrcq], [CONST_S16_POS58]
    pmulhw              xmm7, [pSrcq], [CONST_S16_POS58]
    punpcklwd           xmm0, xmm0, xmm1   
    punpcklwd           xmm2, xmm2, xmm3
    punpcklwd           xmm4, xmm4, xmm5 
    punpcklwd           xmm6, xmm6, xmm7   
    movdqa              [V_tempmx+16*0], xmm0
    movdqa              [V_tempmx+16*2], xmm2
    movdqa              [V_tempmx+16*4], xmm4
    movdqa              [V_tempmx+16*6], xmm6
    add                 pSrcq, block_temp_stride  

    pmullw              xmm0, [pSrcq], [CONST_S16_POS17]
    pmulhw              xmm1, [pSrcq], [CONST_S16_POS17]
    add                 pSrcq, block_temp_stride
    pmullw              xmm2, [pSrcq], [CONST_S16_NEG5]
    pmulhw              xmm3, [pSrcq], [CONST_S16_NEG5]
    add                 pSrcq, block_temp_stride
    pmullw              xmm4, [pSrcq], [CONST_S16_POS1]
    pmulhw              xmm5, [pSrcq], [CONST_S16_POS1]
    add                 pSrcq, block_temp_stride
    punpcklwd           xmm0, xmm0, xmm1
    punpcklwd           xmm2, xmm2, xmm3
    punpcklwd           xmm4, xmm4, xmm5  
    pxor                xmm6, xmm6
    movdqa              [V_tempmx+16*8], xmm0 
    movdqa              [V_tempmx+16*10], xmm2
    movdqa              [V_tempmx+16*12], xmm4  
    movdqa              [V_tempmx+16*14], xmm6

%endmacro

;input: pSrcq, is 16 bit(from 16 bit H filter)
;output: V_tempmx, 32 bit. (16*16->32)
;args:  %offset 
%macro  v2_4_2_mul 1 

    pmullw              xmm0, [pSrcq], [CONST_S16_NEG1]
    pmulhw              xmm1, [pSrcq], [CONST_S16_NEG1]
    add                 pSrcq, block_temp_stride
    pmullw              xmm2, [pSrcq], [CONST_S16_POS4]
    pmulhw              xmm3, [pSrcq], [CONST_S16_POS4]
    add                 pSrcq, block_temp_stride
    pmullw              xmm4, [pSrcq], [CONST_S16_NEG11]
    pmulhw              xmm5, [pSrcq], [CONST_S16_NEG11]
    add                 pSrcq, block_temp_stride
    pmullw              xmm6, [pSrcq], [CONST_S16_POS40]
    pmulhw              xmm7, [pSrcq], [CONST_S16_POS40]
    punpcklwd           xmm0, xmm0, xmm1   
    punpcklwd           xmm2, xmm2, xmm3
    punpcklwd           xmm4, xmm4, xmm5 
    punpcklwd           xmm6, xmm6, xmm7   
    movdqa              [V_tempmx+16*0], xmm0
    movdqa              [V_tempmx+16*2], xmm2
    movdqa              [V_tempmx+16*4], xmm4
    movdqa              [V_tempmx+16*6], xmm6
    add                 pSrcq, block_temp_stride  

    pmullw              xmm0, [pSrcq], [CONST_S16_POS40]
    pmulhw              xmm1, [pSrcq], [CONST_S16_POS40]
    add                 pSrcq, block_temp_stride
    pmullw              xmm2, [pSrcq], [CONST_S16_NEG11]
    pmulhw              xmm3, [pSrcq], [CONST_S16_NEG11]
    add                 pSrcq, block_temp_stride
    pmullw              xmm4, [pSrcq], [CONST_S16_POS4]
    pmulhw              xmm5, [pSrcq], [CONST_S16_POS4]
    add                 pSrcq, block_temp_stride
    pmullw              xmm6, [pSrcq], [CONST_S16_NEG1]
    pmulhw              xmm7, [pSrcq], [CONST_S16_NEG1]
    punpcklwd           xmm0, xmm0, xmm1
    punpcklwd           xmm2, xmm2, xmm3
    punpcklwd           xmm4, xmm4, xmm5
    punpcklwd           xmm6, xmm6, xmm7 
    movdqa              [V_tempmx+16*8], xmm0 
    movdqa              [V_tempmx+16*10], xmm2
    movdqa              [V_tempmx+16*12], xmm4  
    movdqa              [V_tempmx+16*14], xmm6
%endmacro

;input: pSrcq, is 16 bit(from 16 bit H filter)
;output: V_tempmx, 32 bit. (16*16->32)
;args:  %offset 
%macro  v3_4_2_mul 1 
    pmullw              xmm2, [pSrcq], [CONST_S16_POS1]
    pmulhw              xmm3, [pSrcq], [CONST_S16_POS1]
    add                 pSrcq, block_temp_stride
    pmullw              xmm4, [pSrcq], [CONST_S16_NEG5]
    pmulhw              xmm5, [pSrcq], [CONST_S16_NEG5]
    add                 pSrcq, block_temp_stride
    pmullw              xmm6, [pSrcq], [CONST_S16_POS17]
    pmulhw              xmm7, [pSrcq], [CONST_S16_POS17]
    pxor                 xmm0, xmm0   
    punpcklwd           xmm2, xmm2, xmm3
    punpcklwd           xmm4, xmm4, xmm5 
    punpcklwd           xmm6, xmm6, xmm7   
    movdqa              [V_tempmx+16*0], xmm0
    movdqa              [V_tempmx+16*2], xmm2
    movdqa              [V_tempmx+16*4], xmm4
    movdqa              [V_tempmx+16*6], xmm6
    add                 pSrcq, block_temp_stride  

    pmullw              xmm0, [pSrcq], [CONST_S16_POS58]
    pmulhw              xmm1, [pSrcq], [CONST_S16_POS58]
    add                 pSrcq, block_temp_stride
    pmullw              xmm2, [pSrcq], [CONST_S16_NEG10]
    pmulhw              xmm3, [pSrcq], [CONST_S16_NEG10]
    add                 pSrcq, block_temp_stride
    pmullw              xmm4, [pSrcq], [CONST_S16_POS4]
    pmulhw              xmm5, [pSrcq], [CONST_S16_POS4]
    add                 pSrcq, block_temp_stride
    pmullw              xmm6, [pSrcq], [CONST_S16_NEG1]
    pmulhw              xmm7, [pSrcq], [CONST_S16_NEG1]
    punpcklwd           xmm0, xmm0, xmm1
    punpcklwd           xmm2, xmm2, xmm3
    punpcklwd           xmm4, xmm4, xmm5
    punpcklwd           xmm6, xmm6, xmm7 
    movdqa              [V_tempmx+16*8], xmm0 
    movdqa              [V_tempmx+16*10], xmm2
    movdqa              [V_tempmx+16*12], xmm4  
    movdqa              [V_tempmx+16*14], xmm6

%endmacro

;input:  xmm0
;output: xmm1
;args:  %narrow 
%macro  h1_8 1

    psrldq              xmm1, xmm0, 6
    punpcklbw           xmm1, xmm1, [CONST_U8_0] 
    punpcklbw           xmm2, xmm0, [CONST_U8_0] 
    psubw               xmm1, xmm1, xmm2  ; xmm1 = A[6], xmm2 = A[0]

    psrldq              xmm3, xmm0, 1
    punpcklbw           xmm3, xmm3, [CONST_U8_0]
    pmullw              xmm3, xmm3,  [CONST_S16_POS4]

    psrldq              xmm4, xmm0, 2
    punpcklbw           xmm4, xmm4, [CONST_U8_0]
    pmullw              xmm4, xmm4,  [CONST_S16_NEG10]

    psrldq              xmm5, xmm0, 3
    punpcklbw           xmm5, xmm5, [CONST_U8_0]
    pmullw              xmm5, xmm5,  [CONST_S16_POS58]

    psrldq              xmm6, xmm0, 4
    punpcklbw           xmm6, xmm6, [CONST_U8_0]
    pmullw              xmm6, xmm6,  [CONST_S16_POS17]

    psrldq              xmm7, xmm0, 5
    punpcklbw           xmm7, xmm7, [CONST_U8_0]
    pmullw              xmm7, xmm7,  [CONST_S16_NEG5]

    paddsw              xmm1, xmm1, xmm7
    paddsw              xmm3, xmm3, xmm4
    paddsw              xmm5, xmm5, xmm6
    paddsw              xmm1, xmm1, xmm3
    paddsw              xmm1, xmm1, xmm5

     %ifidn %1, true
        paddsw          xmm1, xmm1, [CONST_U16_POS32]
        psraw           xmm1, xmm1, 6
        packuswb        xmm1, xmm1, xmm2
    %endif
%endmacro

;input:  xmm0
;output: xmm1
;args:  %narrow 
%macro  h2_8 1 

    psrldq              xmm1, xmm0, 1
    psrldq              xmm2, xmm0, 6
    punpcklbw           xmm1, xmm1, [CONST_U8_0] 
    punpcklbw           xmm2, xmm2, [CONST_U8_0] 
    paddw               xmm1, xmm1, xmm2   
    psllw               xmm1, xmm1, 2      ;(A[6]+A[1])*4

    psrldq              xmm2, xmm0, 7
    psrldq              xmm4, xmm0, 2
    psrldq              xmm5, xmm0, 5
    psrldq              xmm6, xmm0, 3
    psrldq              xmm7, xmm0, 4
    punpcklbw           xmm2, xmm2, [CONST_U8_0] 
    punpcklbw           xmm3, xmm0, [CONST_U8_0]
    punpcklbw           xmm4, xmm4, [CONST_U8_0]
    punpcklbw           xmm5, xmm5, [CONST_U8_0]
    punpcklbw           xmm6, xmm6, [CONST_U8_0]
    punpcklbw           xmm7, xmm7, [CONST_U8_0]
    paddw               xmm4, xmm4, xmm5
    paddw               xmm6, xmm6, xmm7    
    pmullw              xmm4, xmm4,  [CONST_S16_NEG11]    ; -11*(A[2]+A[5])   
    pmullw              xmm6, xmm6,  [CONST_S16_POS40]    ; 40*(A[3]+A[4])

    paddw               xmm2, xmm2, xmm3   ;(A[7]+A[0])
    psubw               xmm1, xmm1, xmm2   ;(A[6]+A[1])*4-(A[7]+A[0])
    paddsw              xmm4, xmm4, xmm6
    paddsw              xmm1, xmm1, xmm4

    %ifidn %1, true
        paddsw          xmm1, xmm1, [CONST_U16_POS32]
        psraw           xmm1, xmm1, 6
        packuswb        xmm1, xmm1, xmm2
    %endif
%endmacro

;input:  xmm0
;output: xmm1
;args:  %narrow 
%macro  h3_8 1
    psrldq              xmm1, xmm0, 1
    psrldq              xmm2, xmm0, 7
    punpcklbw           xmm1, xmm1, [CONST_U8_0] 
    punpcklbw           xmm2, xmm2, [CONST_U8_0] 
    psubw               xmm1, xmm1, xmm2  ; xmm1 = A[1], xmm2 = A[7]

    psrldq              xmm3, xmm0, 2
    punpcklbw           xmm3, xmm3, [CONST_U8_0]
    pmullw              xmm3, xmm3,  [CONST_S16_NEG5]

    psrldq              xmm4, xmm0, 3
    punpcklbw           xmm4, xmm4, [CONST_U8_0]
    pmullw              xmm4, xmm4,  [CONST_S16_POS17]

    psrldq              xmm5, xmm0, 4
    punpcklbw           xmm5, xmm5, [CONST_U8_0]
    pmullw              xmm5, xmm5,  [CONST_S16_POS58]

    psrldq              xmm6, xmm0, 5
    punpcklbw           xmm6, xmm6, [CONST_U8_0]
    pmullw              xmm6, xmm6,  [CONST_S16_NEG10]

    psrldq              xmm7, xmm0, 6
    punpcklbw           xmm7, xmm7, [CONST_U8_0]
    pmullw              xmm7, xmm7,  [CONST_S16_POS4]

    paddsw              xmm1, xmm1, xmm7
    paddsw              xmm3, xmm3, xmm4
    paddsw              xmm5, xmm5, xmm6
    paddsw              xmm1, xmm1, xmm3
    paddsw              xmm1, xmm1, xmm5

     %ifidn %1, true
        paddsw          xmm1, xmm1, [CONST_U16_POS32]
        psraw           xmm1, xmm1, 6
        packuswb        xmm1, xmm1, xmm2
    %endif

%endmacro

%macro  h123_8_filter 1 
    movdqa          xmm3,  [CONST_U16_POS32]
    movdqu          xmm4,  [CONST_S8_F%1_01]
	movdqu          xmm5,  [CONST_S8_F%1_23]
	movdqu          xmm6,  [CONST_S8_F%1_45]
	movdqu          xmm7,  [CONST_S8_F%1_67]
%endmacro

;input:  xmm0
;output: xmm1
;args:  %narrow 
%macro  h123_8 1

;    pshufb              xmm1, xmm0, [CONST_U8_01]
;    pmaddubsw           xmm1, xmm4
;    pshufb              xmm2, xmm0, [CONST_U8_23]
;    pmaddubsw           xmm2, xmm5
;    paddw               xmm1, xmm2
;    pshufb              xmm2, xmm0, [CONST_U8_45]
;    pmaddubsw           xmm2, xmm6
;    paddw               xmm1, xmm2
;    pshufb              xmm2, xmm0, [CONST_U8_67]
;    pmaddubsw           xmm2, xmm7
;    paddw               xmm1, xmm2

;pshufb              xmm1, xmm0, [CONST_U8_01]
	movdqa              xmm1, xmm0
	psrldq              xmm0, xmm0, 1
	punpcklbw           xmm1, xmm0
    pmaddubsw           xmm1, xmm4
	;pshufb              xmm2, xmm0, [CONST_U8_23]
	psrldq              xmm2, xmm0, 1
	psrldq              xmm0, xmm2, 1
	punpcklbw           xmm2, xmm0
	pmaddubsw           xmm2, xmm5
	paddw               xmm1, xmm2
	;pshufb              xmm2, xmm0, [CONST_U8_45]
	psrldq              xmm2, xmm0, 1
	psrldq              xmm0, xmm2, 1
	punpcklbw           xmm2, xmm0
	pmaddubsw           xmm2, xmm6
	paddw              xmm1, xmm2
	;pshufb              xmm2, xmm0, [CONST_U8_67]
	psrldq              xmm2, xmm0, 1
	psrldq              xmm0, xmm2, 1
	punpcklbw           xmm2, xmm0
	pmaddubsw           xmm2, xmm7
	paddw              xmm1, xmm2
	
     %ifidn %1, true
        paddsw          xmm1, xmm1, xmm3
        psraw           xmm1, xmm1, 6
        packuswb        xmm1, xmm1, xmm2
    %endif
%endmacro

;input: xmm0 ~ xmm7, 16 bit(from 8 bit yuv then zero extend)
;output: xmm6
;args:  %narrow  
%macro  v1_8 1
    psubw               xmm6, xmm6, xmm0      ; (A[6]-A[0])
    movdqa              xmm7, xmm3             ; NOT change xmm1, xmm2, xmm3
    movdqa              xmm0, xmm2             ; NOT change xmm1, xmm2, xmm3
 
    psllw               xmm1, xmm1,  2    ; 4*A[1], NOT change xmm1, xmm2, xmm3
    pmullw              xmm0, xmm0,  [CONST_S16_NEG10]    ; -10*A[2]
    pmullw              xmm7, xmm7,  [CONST_S16_POS58]    ; 58*A[3]
    pmullw              xmm4, xmm4,  [CONST_S16_POS17]    ; 17*A[4]
    pmullw              xmm5, xmm5,  [CONST_S16_NEG5]    ; -5*A[5]

    paddsw              xmm0, xmm0, xmm1
    paddsw              xmm7, xmm7, xmm4
    paddsw              xmm6, xmm6, xmm5
    paddsw              xmm0, xmm0, xmm7
    paddsw              xmm6, xmm6, xmm0

    psrlw               xmm1, xmm1,  2    ; NOT change xmm1, xmm2, xmm3

    %ifidn %1, true
        paddsw          xmm6, xmm6, [CONST_U16_POS32]
        psraw           xmm6, xmm6, 6
        packuswb        xmm6, xmm6, xmm2
    %endif
   

%endmacro

;input: xmm0 ~ xmm7, 16 bit(from 8 bit yuv then zero extend)
;output: xmm6
;args:  %narrow  
%macro  v2_8 1 
    paddw               xmm7, xmm7, xmm0      ; (A[0]+A[7])
    paddw               xmm6, xmm6, xmm1   
    paddw               xmm5, xmm5, xmm2   
    paddw               xmm4, xmm4, xmm3   

    pmullw              xmm6, xmm6,  [CONST_S16_POS4]    ; 4*(A[1]+A[6])
    pmullw              xmm5, xmm5,  [CONST_S16_POS11]    ; 11*(A[2]+A[5])
    pmullw              xmm4, xmm4,  [CONST_S16_POS40]    ; 40*(A[3]+A[4])

    paddsw              xmm6, xmm6, xmm4
    paddsw              xmm7, xmm7, xmm5
    psubsw              xmm6, xmm6, xmm7

    %ifidn %1, true
        paddsw          xmm6, xmm6, [CONST_U16_POS32]
        psraw           xmm6, xmm6, 6
        packuswb        xmm6, xmm6, xmm2
    %endif
%endmacro

;input: xmm0 ~ xmm7, 16 bit(from 8 bit yuv then zero extend)
;output: xmm6
;args:  %narrow  
%macro  v3_8 1
    movdqa              xmm0, xmm1             ; NOT change xmm1, xmm2, xmm3
    psubw               xmm0, xmm0, xmm7      ; (A[1]-A[7])

    movdqa              xmm7, xmm2             ; NOT change xmm1, xmm2, xmm3
  
    pmullw              xmm7, xmm7,  [CONST_S16_NEG5]    ; -5*A[2]
    pmullw              xmm3, xmm3,  [CONST_S16_POS17]    ; 17*A[3]
    pmullw              xmm4, xmm4,  [CONST_S16_POS58]    ; 58*A[4]
    pmullw              xmm5, xmm5,  [CONST_S16_NEG10]    ; -10*A[5]
    psllw               xmm6, xmm6,  2                    ; 4*A[6]
   
   
    paddsw              xmm4, xmm4, xmm0
    paddsw              xmm5, xmm5, xmm7
    paddsw              xmm6, xmm6, xmm3
    paddsw              xmm4, xmm4, xmm5
    paddsw              xmm6, xmm6, xmm4   

    %ifidn %1, true
        paddsw          xmm6, xmm6, [CONST_U16_POS32]
        psraw           xmm6, xmm6, 6
        packuswb        xmm6, xmm6, xmm2
    %endif

%endmacro

;input: pSrcq, is 16 bit(from 16 bit H filter)
;output: V_tempmx, 32 bit. (16*16->32)
;args:  %offset 
%macro  v1_8_2_mul 1 
    pmullw              xmm0, [pSrcq], [CONST_S16_NEG1]
    pmulhw              xmm1, [pSrcq], [CONST_S16_NEG1]
    add                 pSrcq, block_temp_stride
    pmullw              xmm4, [pSrcq], [CONST_S16_POS4]
    pmulhw              xmm5, [pSrcq], [CONST_S16_POS4]
    punpcklwd           xmm2, xmm0, xmm1
    punpckhwd           xmm3, xmm0, xmm1
    punpcklwd           xmm6, xmm4, xmm5
    punpckhwd           xmm4, xmm4, xmm5
    movdqa              [V_tempmx+16*0], xmm2
    movdqa              [V_tempmx+16*1], xmm3 
    movdqa              [V_tempmx+16*2], xmm6
    movdqa              [V_tempmx+16*3], xmm4
    add                 pSrcq, block_temp_stride

    pmullw              xmm0, [pSrcq], [CONST_S16_NEG10]
    pmulhw              xmm1, [pSrcq], [CONST_S16_NEG10]
    add                 pSrcq, block_temp_stride
    pmullw              xmm4, [pSrcq], [CONST_S16_POS58]
    pmulhw              xmm5, [pSrcq], [CONST_S16_POS58]
    punpcklwd           xmm2, xmm0, xmm1
    punpckhwd           xmm3, xmm0, xmm1
    punpcklwd           xmm6, xmm4, xmm5
    punpckhwd           xmm4, xmm4, xmm5
    movdqa              [V_tempmx+16*4], xmm2
    movdqa              [V_tempmx+16*5], xmm3 
    movdqa              [V_tempmx+16*6], xmm6
    movdqa              [V_tempmx+16*7], xmm4
    add                 pSrcq, block_temp_stride

    pmullw              xmm0, [pSrcq], [CONST_S16_POS17]
    pmulhw              xmm1, [pSrcq], [CONST_S16_POS17]
    add                 pSrcq, block_temp_stride
    pmullw              xmm4, [pSrcq], [CONST_S16_NEG5]
    pmulhw              xmm5, [pSrcq], [CONST_S16_NEG5]
    punpcklwd           xmm2, xmm0, xmm1
    punpckhwd           xmm3, xmm0, xmm1
    punpcklwd           xmm6, xmm4, xmm5
    punpckhwd           xmm4, xmm4, xmm5
    movdqa              [V_tempmx+16*8], xmm2
    movdqa              [V_tempmx+16*9], xmm3 
    movdqa              [V_tempmx+16*10], xmm6
    movdqa              [V_tempmx+16*11], xmm4
    add                 pSrcq, block_temp_stride

    pmullw              xmm0, [pSrcq], [CONST_S16_POS1]
    pmulhw              xmm1, [pSrcq], [CONST_S16_POS1]
    add                 pSrcq, block_temp_stride
    punpcklwd           xmm2, xmm0, xmm1
    punpckhwd           xmm3, xmm0, xmm1
    pxor                xmm4, xmm4
    pxor                xmm6, xmm6
    movdqa              [V_tempmx+16*12], xmm2
    movdqa              [V_tempmx+16*13], xmm3 
    movdqa              [V_tempmx+16*14], xmm6
    movdqa              [V_tempmx+16*15], xmm4

%endmacro

;input: pSrcq, is 16 bit(from 16 bit H filter)
;output: V_tempmx, 32 bit. (16*16->32)
;args:  %offset 
%macro  v2_8_2_mul 1 

    pmullw              xmm0, [pSrcq], [CONST_S16_NEG1]
    pmulhw              xmm1, [pSrcq], [CONST_S16_NEG1]
    add                 pSrcq, block_temp_stride
    pmullw              xmm4, [pSrcq], [CONST_S16_POS4]
    pmulhw              xmm5, [pSrcq], [CONST_S16_POS4]
    punpcklwd           xmm2, xmm0, xmm1
    punpckhwd           xmm3, xmm0, xmm1
    punpcklwd           xmm6, xmm4, xmm5
    punpckhwd           xmm4, xmm4, xmm5
    movdqa              [V_tempmx+16*0], xmm2
    movdqa              [V_tempmx+16*1], xmm3 
    movdqa              [V_tempmx+16*2], xmm6
    movdqa              [V_tempmx+16*3], xmm4
    add                 pSrcq, block_temp_stride

    pmullw              xmm0, [pSrcq], [CONST_S16_NEG11]
    pmulhw              xmm1, [pSrcq], [CONST_S16_NEG11]
    add                 pSrcq, block_temp_stride
    pmullw              xmm4, [pSrcq], [CONST_S16_POS40]
    pmulhw              xmm5, [pSrcq], [CONST_S16_POS40]
    punpcklwd           xmm2, xmm0, xmm1
    punpckhwd           xmm3, xmm0, xmm1
    punpcklwd           xmm6, xmm4, xmm5
    punpckhwd           xmm4, xmm4, xmm5
    movdqa              [V_tempmx+16*4], xmm2
    movdqa              [V_tempmx+16*5], xmm3 
    movdqa              [V_tempmx+16*6], xmm6
    movdqa              [V_tempmx+16*7], xmm4
    add                 pSrcq, block_temp_stride

    pmullw              xmm0, [pSrcq], [CONST_S16_POS40]
    pmulhw              xmm1, [pSrcq], [CONST_S16_POS40]
    add                 pSrcq, block_temp_stride
    pmullw              xmm4, [pSrcq], [CONST_S16_NEG11]
    pmulhw              xmm5, [pSrcq], [CONST_S16_NEG11]
    punpcklwd           xmm2, xmm0, xmm1
    punpckhwd           xmm3, xmm0, xmm1
    punpcklwd           xmm6, xmm4, xmm5
    punpckhwd           xmm4, xmm4, xmm5
    movdqa              [V_tempmx+16*8], xmm2
    movdqa              [V_tempmx+16*9], xmm3 
    movdqa              [V_tempmx+16*10], xmm6
    movdqa              [V_tempmx+16*11], xmm4
    add                 pSrcq, block_temp_stride

    pmullw              xmm0, [pSrcq], [CONST_S16_POS4]
    pmulhw              xmm1, [pSrcq], [CONST_S16_POS4]
    add                 pSrcq, block_temp_stride
    pmullw              xmm4, [pSrcq], [CONST_S16_NEG1]
    pmulhw              xmm5, [pSrcq], [CONST_S16_NEG1]
    punpcklwd           xmm2, xmm0, xmm1
    punpckhwd           xmm3, xmm0, xmm1
    punpcklwd           xmm6, xmm4, xmm5
    punpckhwd           xmm4, xmm4, xmm5
    movdqa              [V_tempmx+16*12], xmm2
    movdqa              [V_tempmx+16*13], xmm3 
    movdqa              [V_tempmx+16*14], xmm6
    movdqa              [V_tempmx+16*15], xmm4

%endmacro

;input: pSrcq, is 16 bit(from 16 bit H filter)
;output: V_tempmx, 32 bit. (16*16->32)
;args:  %offset 
%macro  v3_8_2_mul 1   
    pmullw              xmm4, [pSrcq], [CONST_S16_POS1]
    pmulhw              xmm5, [pSrcq], [CONST_S16_POS1]
    pxor                xmm2, xmm2
    pxor                xmm3, xmm3
    punpcklwd           xmm6, xmm4, xmm5
    punpckhwd           xmm4, xmm4, xmm5
    movdqa              [V_tempmx+16*0], xmm2
    movdqa              [V_tempmx+16*1], xmm3 
    movdqa              [V_tempmx+16*2], xmm6
    movdqa              [V_tempmx+16*3], xmm4
    add                 pSrcq, block_temp_stride

    pmullw              xmm0, [pSrcq], [CONST_S16_NEG5]
    pmulhw              xmm1, [pSrcq], [CONST_S16_NEG5]
    add                 pSrcq, block_temp_stride
    pmullw              xmm4, [pSrcq], [CONST_S16_POS17]
    pmulhw              xmm5, [pSrcq], [CONST_S16_POS17]
    punpcklwd           xmm2, xmm0, xmm1
    punpckhwd           xmm3, xmm0, xmm1
    punpcklwd           xmm6, xmm4, xmm5
    punpckhwd           xmm4, xmm4, xmm5
    movdqa              [V_tempmx+16*4], xmm2
    movdqa              [V_tempmx+16*5], xmm3 
    movdqa              [V_tempmx+16*6], xmm6
    movdqa              [V_tempmx+16*7], xmm4
    add                 pSrcq, block_temp_stride

    pmullw              xmm0, [pSrcq], [CONST_S16_POS58]
    pmulhw              xmm1, [pSrcq], [CONST_S16_POS58]
    add                 pSrcq, block_temp_stride
    pmullw              xmm4, [pSrcq], [CONST_S16_NEG10]
    pmulhw              xmm5, [pSrcq], [CONST_S16_NEG10]
    punpcklwd           xmm2, xmm0, xmm1
    punpckhwd           xmm3, xmm0, xmm1
    punpcklwd           xmm6, xmm4, xmm5
    punpckhwd           xmm4, xmm4, xmm5
    movdqa              [V_tempmx+16*8], xmm2
    movdqa              [V_tempmx+16*9], xmm3 
    movdqa              [V_tempmx+16*10], xmm6
    movdqa              [V_tempmx+16*11], xmm4
    add                 pSrcq, block_temp_stride

    pmullw              xmm0, [pSrcq], [CONST_S16_POS4]
    pmulhw              xmm1, [pSrcq], [CONST_S16_POS4]
    add                 pSrcq, block_temp_stride
    pmullw              xmm4, [pSrcq], [CONST_S16_NEG1]
    pmulhw              xmm5, [pSrcq], [CONST_S16_NEG1]
    punpcklwd           xmm2, xmm0, xmm1
    punpckhwd           xmm3, xmm0, xmm1
    punpcklwd           xmm6, xmm4, xmm5
    punpckhwd           xmm4, xmm4, xmm5
    movdqa              [V_tempmx+16*12], xmm2
    movdqa              [V_tempmx+16*13], xmm3 
    movdqa              [V_tempmx+16*14], xmm6
    movdqa              [V_tempmx+16*15], xmm4

%endmacro

;input: V_tempmx, is 32 bit(from V filter)
;output: xmm0
;args:  %offset, %narrow 
%macro  v2_8_2_add 1-2 true  

    movdqa              xmm0, [V_tempmx+16*0+%1]
    movdqa              xmm1, [V_tempmx+16*2+%1]
    movdqa              xmm2, [V_tempmx+16*4+%1]
    movdqa              xmm3, [V_tempmx+16*6+%1]
    movdqa              xmm4, [V_tempmx+16*8+%1]
    movdqa              xmm5, [V_tempmx+16*10+%1]
    movdqa              xmm6, [V_tempmx+16*12+%1]
    movdqa              xmm7, [V_tempmx+16*14+%1]

    paddd               xmm0, xmm0, xmm1
    paddd               xmm2, xmm2, xmm3
    paddd               xmm4, xmm4, xmm5
    paddd               xmm6, xmm6, xmm7

    paddd               xmm0, xmm0, xmm2
    paddd               xmm4, xmm4, xmm6
    paddd               xmm0, xmm0, xmm4

    %ifidn %2, true
        paddd           xmm0, xmm0, [CONST_U32_POS2048]
        psrad           xmm0, xmm0, 12
        packssdw        xmm0, xmm0, xmm2
        packuswb        xmm0, xmm0, xmm2
    %else      
        psrad           xmm0, xmm0, 6
        packssdw        xmm0, xmm0, xmm2
    %endif
%endmacro


%macro  h265_qpel2_h0v0_neon 1 ;%type 
%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_qpel2_h0v0_y_loop:

    movd                mm0,  [pSrcq]

%ifidn %1, put	                ;put case
    movd                nSrcStrided, mm0  ;@reuse nSrcStrideq
    mov                 [pDstq], nSrcStridew
%elifidn %1, avg_nornd	        ;avg_nornd case, , shift 6 left
    punpcklbw           mm0, mm0, [CONST_U8_0]
    psllw               mm0, mm0, 6
    movd                [pDstq], mm0
%elifidn %1, avg	            ;avg case, shift  7 bits right 
    punpcklbw           mm0, mm0, [CONST_U8_0]
    psllw               mm0, mm0, 6
    movq                mm1,  [nWidthq]
    paddsw               mm0, mm0, mm1
    round_7bits         mm0         
    packuswb            mm0, mm0, mm1
    movd                nSrcStrided, mm0  ;@reuse nSrcStrideq
    mov                 [pDstq], nSrcStridew
    add                 nWidthq, nDstStride2mp    
%endif
    
    mov                 nSrcStrideq, nSrcStridemp  ;@reuse nSrcStrideq
    add                 pSrcq, nSrcStrideq
    add                 pDstq, nDstStrideq
    sub                 nHeightd,  1
    jg                  %%h265_qpel2_h0v0_y_loop 

%ifidn %1, avg	        ;move pDst2 2 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 4
    mov                 [pDst2mx], nWidthq
%endif 

%endmacro

%macro  h265_qpel4_h0v0_neon 1 ;%type 

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_qpel4_h0v0_y_loop:

    movd                xmm0,  [pSrcq]

%ifidn %1, put	                ;put case
    movd                [pDstq], xmm0
%elifidn %1, avg_nornd	        ;avg_nornd case, , shift 6 left
    punpcklbw           xmm0, xmm0, [CONST_U8_0]
    psllw               xmm0, xmm0, 6
    movq                [pDstq], xmm0
%elifidn %1, avg	            ;avg case, shift  7 bits right 
    punpcklbw           xmm0, xmm0, [CONST_U8_0]
    psllw               xmm0, xmm0, 6
    movq                xmm1,  [nWidthq]
    paddsw             xmm0, xmm0, xmm1
    round_7bits         xmm0         
    packuswb            xmm0, xmm0, xmm1
    movd                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp    
%endif

    add                 pSrcq, nSrcStrideq
    add                 pDstq, nDstStrideq
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h0v0_y_loop 

%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif 

%endmacro


%macro  h265_qpel4_h1_neon 1 ;%type 

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif
    ;load filter parameter
    h123_8_filter 1
%%h265_qpel4_h1_y_loop:
    movdqu              xmm0,  [pSrcq]
%ifidn %1, put	                ;put case, shift 6 right and get final result
    ;h1_8                true
    h123_8  true
    movd                [pDstq], xmm1
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    ;h1_8                false
    h123_8  false
    movq                [pDstq], xmm1
%elifidn %1, avg	            ;avg case, shift  7 bits right 
    ;h1_8                false
    h123_8   false
    movdqu              xmm0,  [nWidthq]
    paddsw             xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pSrcq, nSrcStrideq
    add                 pDstq, nDstStrideq
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h1_y_loop 
    
%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif  


%endmacro





%macro  h265_qpel4_h2_neon 1 ;%type 

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif
    h123_8_filter 2
%%h265_qpel4_h2_y_loop:
    movdqu              xmm0,  [pSrcq]
%ifidn %1, put	                ;put case, shift 6 right and get final result
    h123_8                true
    movd                [pDstq], xmm1
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    h123_8                false
    movq                [pDstq], xmm1
%elifidn %1, avg	            ;avg case, shift  7 bits right 
    h123_8                false
    movdqu              xmm0,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pSrcq, nSrcStrideq
    add                 pDstq, nDstStrideq
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h2_y_loop 
    
%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif  
 
%endmacro

%macro  h265_qpel4_h3_neon 1 ;%type 

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif
    h123_8_filter 3
%%h265_qpel4_h3_y_loop:
    movdqu              xmm0,  [pSrcq]
%ifidn %1, put	                ;put case, shift 6 right and get final result
    h123_8                true
    movd                [pDstq], xmm1
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    h123_8                false
    movq                [pDstq], xmm1
%elifidn %1, avg	            ;avg case, shift  7 bits right 
    h123_8                false
    movdqu              xmm0,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pSrcq, nSrcStrideq
    add                 pDstq, nDstStrideq
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h3_y_loop 
    
%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif 

%endmacro

%macro  h265_qpel4_v1_neon 1 ;%type 
    movd                mm0,  [pSrcq]
    punpcklbw           mm0, mm0, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movd                mm1,  [pSrcq]
    punpcklbw           mm1, mm1, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movd                mm2,  [pSrcq]
    punpcklbw           mm2, mm2, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif
    
%%h265_qpel4_v1_y_loop:
    mov                 nDstStrideq, pSrcq      ;back pSrcq for next y loop
    movd                mm3,  [pSrcq]
    punpcklbw           mm3, mm3, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movd                mm4,  [pSrcq]
    punpcklbw           mm4, mm4, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movd                mm5,  [pSrcq]
    punpcklbw           mm5, mm5, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movd                mm6,  [pSrcq]
    punpcklbw           mm6, mm6, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movd                mm7,  [pSrcq]
    punpcklbw           mm7, mm7, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq

%ifidn %1, put	                ;put case, shift 6 bits right and get final result
    v1_4                true
    movd                [pDstq], mm6
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    v1_4                false
    movq                [pDstq], mm6
%elifidn %1, avg	            ;avg case, shift 7 bits right
    v1_4                false
    movq                mm0,  [nWidthq]
    paddsw               mm0, mm0, mm6
    round_7bits         mm0
    packuswb            mm0, mm0, mm2  ;mm2 can be any REG
    movd                [pDstq], mm0
    add                 nWidthq, nDstStride2mp
%endif

    movq                mm0, mm1
    movq                mm1, mm2
    movq                mm2, mm3

    mov                 pSrcq, nDstStrideq          ;back pSrcq for next y loop
    add                 pSrcq, nSrcStrideq

    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    add                 pDstq, nDstStrideq

    sub                 nHeightd,  1
    jg                  %%h265_qpel4_v1_y_loop    

%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif 


%endmacro

%macro  h265_qpel4_v2_neon 1 ;%type 
    movd                mm0,  [pSrcq]
    punpcklbw           mm0, mm0, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movd                mm1,  [pSrcq]
    punpcklbw           mm1, mm1, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movd                mm2,  [pSrcq]
    punpcklbw           mm2, mm2, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif
    
%%h265_qpel4_v2_y_loop:
    mov                 nDstStrideq, pSrcq      ;back pSrcq for next y loop
    movd                mm3,  [pSrcq]
    punpcklbw           mm3, mm3, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movd                mm4,  [pSrcq]
    punpcklbw           mm4, mm4, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movd                mm5,  [pSrcq]
    punpcklbw           mm5, mm5, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movd                mm6,  [pSrcq]
    punpcklbw           mm6, mm6, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movd                mm7,  [pSrcq]
    punpcklbw           mm7, mm7, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq

%ifidn %1, put	                ;put case, shift 6 bits right and get final result
    v2_4                true
    movd                [pDstq], mm6
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    v2_4                false
    movq                [pDstq], mm6
%elifidn %1, avg	            ;avg case, shift 7 bits right
    v2_4                false
    movq                mm0,  [nWidthq]
    paddsw               mm0, mm0, mm6
    round_7bits         mm0
    packuswb            mm0, mm0, mm2  ;mm2 can be any REG
    movd                [pDstq], mm0
    add                 nWidthq, nDstStride2mp
%endif

    movq                mm0, mm1
    movq                mm1, mm2
    movq                mm2, mm3

    mov                 pSrcq, nDstStrideq          ;back pSrcq for next y loop
    add                 pSrcq, nSrcStrideq

    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    add                 pDstq, nDstStrideq

    sub                 nHeightd,  1
    jg                  %%h265_qpel4_v2_y_loop    

%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif 

%endmacro

%macro  h265_qpel4_v3_neon 1 ;%type

    movd                mm1,  [pSrcq]
    punpcklbw           mm1, mm1, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
  

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif
    
%%h265_qpel4_v3_y_loop:
    mov                 nDstStrideq, pSrcq      ;back pSrcq for next y loop
    movd                mm2,  [pSrcq]
    punpcklbw           mm2, mm2, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movd                mm3,  [pSrcq]
    punpcklbw           mm3, mm3, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movd                mm4,  [pSrcq]
    punpcklbw           mm4, mm4, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movd                mm5,  [pSrcq]
    punpcklbw           mm5, mm5, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movd                mm6,  [pSrcq]
    punpcklbw           mm6, mm6, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movd                mm7,  [pSrcq]
    punpcklbw           mm7, mm7, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq

%ifidn %1, put	                ;put case, shift 6 bits right and get final result
    v3_4                true
    movd                [pDstq], mm6
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    v3_4                false
    movq                [pDstq], mm6
%elifidn %1, avg	            ;avg case, shift 7 bits right
    v3_4                false
    movq                mm0,  [nWidthq]
    paddsw               mm0, mm0, mm6
    round_7bits         mm0
    packuswb            mm0, mm0, mm2  ;mm2 can be any REG
    movd                [pDstq], mm0
    add                 nWidthq, nDstStride2mp
%endif

    movq                mm0, mm1
    movq                mm1, mm2
 

    mov                 pSrcq, nDstStrideq          ;back pSrcq for next y loop
    add                 pSrcq, nSrcStrideq

    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    add                 pDstq, nDstStrideq

    sub                 nHeightd,  1
    jg                  %%h265_qpel4_v3_y_loop    

%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif 

%endmacro

%macro  h265_qpel4_h1v1_neon 1 ;%type 
    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7       ; 7 = filter_size-1
    h123_8_filter 1
%%h265_qpel4_h1v1_H_loop:
    movdqu              xmm0,  [pSrcq]
    ;h1_8                false
    h123_8      false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h1v1_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm        ; nHeightd is modified in H loop

    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp
%ifidn %1, avg	        ;@reuse. avg case, using nWidth register to keep pDst2
    mov                 nWidthq, [pDst2mx]  
%endif

%%h265_qpel4_h1v1_V_loop:
    v1_4_2_mul          0   
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0   
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0    
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1 
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*6 ;dealing 1 row behind, in v2_4_2_mul, move 7 rows.  6 = 7 - 1
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h1v1_V_loop

%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif


%endmacro

%macro  h265_qpel4_h1v2_neon 1 ;%type 
    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7       ; 7 = filter_size-1
    h123_8_filter 1
%%h265_qpel4_h1v2_H_loop:
    movdqu              xmm0,  [pSrcq]
    ;h1_8                false
    h123_8      false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h1v2_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm        ; nHeightd is modified in H loop

    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp
%ifidn %1, avg	        ;@reuse. avg case, using nWidth register to keep pDst2
    mov                 nWidthq, [pDst2mx]  
%endif

%%h265_qpel4_h1v2_V_loop:
    v2_4_2_mul          0   
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0   
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0    
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1 
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*6 ;dealing 1 row behind, in v2_4_2_mul, move 7 rows.  6 = 7 - 1
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h1v2_V_loop

%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif


%endmacro

%macro  h265_qpel4_h1v3_neon 1 ;%type 

    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7       ; 7 = filter_size-1
    h123_8_filter 1
%%h265_qpel4_h1v3_H_loop:
    movdqu              xmm0,  [pSrcq]
    ;h1_8                false
    h123_8      false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h1v3_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm        ; nHeightd is modified in H loop

    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp
%ifidn %1, avg	        ;@reuse. avg case, using nWidth register to keep pDst2
    mov                 nWidthq, [pDst2mx]  
%endif

%%h265_qpel4_h1v3_V_loop:
    v3_4_2_mul          0   
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0   
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0    
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1 
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*5 ;dealing 1 row behind, in v3_4_2_mul, move 6 rows.  5 = 6 - 1
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h1v3_V_loop

%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif

%endmacro

%macro  h265_qpel4_h2v1_neon 1 ;%type 
    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7       ; 7 = filter_size-1
    h123_8_filter 2
%%h265_qpel4_h2v1_H_loop:
    movdqu              xmm0,  [pSrcq]
    h123_8                false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h2v1_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm        ; nHeightd is modified in H loop

    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp
%ifidn %1, avg	        ;@reuse. avg case, using nWidth register to keep pDst2
    mov                 nWidthq, [pDst2mx]  
%endif

%%h265_qpel4_h2v1_V_loop:
    v1_4_2_mul          0   
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0   
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0    
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1 
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*6 ;dealing 1 row behind, in v2_4_2_mul, move 7 rows.  6 = 7 - 1
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h2v1_V_loop

%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif


%endmacro

%macro  h265_qpel4_h2v2_neon 1 ;%type 
    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7       ; 7 = filter_size-1
    h123_8_filter 2
%%h265_qpel4_h2v2_H_loop:
    movdqu              xmm0,  [pSrcq]
    h123_8                false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h2v2_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm        ; nHeightd is modified in H loop

    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp
%ifidn %1, avg	        ;@reuse. avg case, using nWidth register to keep pDst2
    mov                 nWidthq, [pDst2mx]  
%endif

%%h265_qpel4_h2v2_V_loop:
    v2_4_2_mul          0   
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0   
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0    
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1 
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*6 ;dealing 1 row behind, in v2_4_2_mul, move 7 rows.  6 = 7 - 1
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h2v2_V_loop

%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif

%endmacro

%macro  h265_qpel4_h2v3_neon 1 ;%type 
    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7       ; 7 = filter_size-1
    h123_8_filter 2
%%h265_qpel4_h2v3_H_loop:
    movdqu              xmm0,  [pSrcq]
    h123_8                false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h2v3_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm        ; nHeightd is modified in H loop

    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp
%ifidn %1, avg	        ;@reuse. avg case, using nWidth register to keep pDst2
    mov                 nWidthq, [pDst2mx]  
%endif

%%h265_qpel4_h2v3_V_loop:
    v3_4_2_mul          0   
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0   
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0    
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1 
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*5 ;dealing 1 row behind, in v2_4_2_mul, move 7 rows.  6 = 7 - 1
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h2v3_V_loop

%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif


%endmacro

%macro  h265_qpel4_h3v1_neon 1 ;%type 

    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7       ; 7 = filter_size-1
    h123_8_filter 3
%%h265_qpel4_h3v1_H_loop:
    movdqu              xmm0,  [pSrcq]
    h123_8                false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h3v1_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm        ; nHeightd is modified in H loop

    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp
%ifidn %1, avg	        ;@reuse. avg case, using nWidth register to keep pDst2
    mov                 nWidthq, [pDst2mx]  
%endif

%%h265_qpel4_h3v1_V_loop:
    v1_4_2_mul          0   
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0   
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0    
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1 
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*6 ;dealing 1 row behind, in v2_4_2_mul, move 7 rows.  6 = 7 - 1
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h3v1_V_loop

%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif

%endmacro

%macro  h265_qpel4_h3v2_neon 1 ;%type 

    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7       ; 7 = filter_size-1
    h123_8_filter 3
%%h265_qpel4_h3v2_H_loop:
    movdqu              xmm0,  [pSrcq]
    h123_8                false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h3v2_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm        ; nHeightd is modified in H loop

    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp
%ifidn %1, avg	        ;@reuse. avg case, using nWidth register to keep pDst2
    mov                 nWidthq, [pDst2mx]  
%endif

%%h265_qpel4_h3v2_V_loop:
    v2_4_2_mul          0   
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0   
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0    
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1 
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*6 ;dealing 1 row behind, in v2_4_2_mul, move 7 rows.  6 = 7 - 1
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h3v2_V_loop

%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif

%endmacro

%macro  h265_qpel4_h3v3_neon 1 ;%type 

    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7       ; 7 = filter_size-1
    h123_8_filter 3
%%h265_qpel4_h3v3_H_loop:
    movdqu              xmm0,  [pSrcq]
    h123_8                false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h3v3_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm        ; nHeightd is modified in H loop

    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp
%ifidn %1, avg	        ;@reuse. avg case, using nWidth register to keep pDst2
    mov                 nWidthq, [pDst2mx]  
%endif

%%h265_qpel4_h3v3_V_loop:
    v3_4_2_mul          0   
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0   
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0    
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1 
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*5 ;dealing 1 row behind, in v2_4_2_mul, move 6 rows.  5 = 6 - 1
    sub                 nHeightd,  1
    jg                  %%h265_qpel4_h3v3_V_loop

%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif

%endmacro



%macro  h265_qpel8_h0v0_neon 1 ;%type 

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_qpel8_h0v0_y_loop:

    movq                xmm0,  [pSrcq]

%ifidn %1, put	                ;put case
    movq                [pDstq], xmm0
%elifidn %1, avg_nornd	        ;avg_nornd case, , shift 6 left
    punpcklbw           xmm0, xmm0, [CONST_U8_0]
    psllw               xmm0, xmm0, 6
    movdqu              [pDstq], xmm0
%elifidn %1, avg	            ;avg case, shift  7 bits right 
    punpcklbw           xmm0, xmm0, [CONST_U8_0]
    psllw               xmm0, xmm0, 6
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm1
    movq                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp    
%endif

    add                 pSrcq, nSrcStrideq
    add                 pDstq, nDstStrideq
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h0v0_y_loop 

%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif 

%endmacro


%macro  h265_qpel8_h1_neon 1 ;%type 

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif
    h123_8_filter 1
%%h265_qpel8_h1_y_loop:
    movdqu              xmm0,  [pSrcq]
%ifidn %1, put	                ;put case, shift 6 right and get final result
    ;h1_8                true
    h123_8            true
    movq                [pDstq], xmm1
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    ;h1_8                false
    h123_8            false
    movdqu              [pDstq], xmm1
%elifidn %1, avg	            ;avg case, shift  7 bits right 
    ;h1_8                false
    h123_8            false
    movdqu              xmm0,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movq                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pSrcq, nSrcStrideq
    add                 pDstq, nDstStrideq
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h1_y_loop 
    
%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif  


%endmacro

%macro  h265_qpel8_h2_neon 1 ;%type 

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif
    h123_8_filter 2
%%h265_qpel8_h2_y_loop:
    movdqu              xmm0,  [pSrcq]
%ifidn %1, put	                ;put case, shift 6 right and get final result
    h123_8                true
    movq                [pDstq], xmm1
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    h123_8                false
    movdqu              [pDstq], xmm1
%elifidn %1, avg	            ;avg case, shift  7 bits right 
    h123_8                false
    movdqu              xmm0,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movq                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pSrcq, nSrcStrideq
    add                 pDstq, nDstStrideq
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h2_y_loop 
    
%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif  
 
%endmacro

%macro  h265_qpel8_h3_neon 1 ;%type 

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif
    h123_8_filter 3
%%h265_qpel8_h3_y_loop:
    movdqu              xmm0,  [pSrcq]
%ifidn %1, put	                ;put case, shift 6 right and get final result
    h123_8                true
    movq                [pDstq], xmm1
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    h123_8                false
    movdqu              [pDstq], xmm1
%elifidn %1, avg	            ;avg case, shift  7 bits right 
    h123_8                false
    movdqu              xmm0,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movq                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pSrcq, nSrcStrideq
    add                 pDstq, nDstStrideq
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h3_y_loop 
    
%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif 

%endmacro

%macro  h265_qpel8_v1_neon 1 ;%type 
    movdqu              xmm0,  [pSrcq]
    punpcklbw           xmm0, xmm0, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm1,  [pSrcq]
    punpcklbw           xmm1, xmm1, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm2,  [pSrcq]
    punpcklbw           xmm2, xmm2, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif
    
%%h265_qpel8_v1_y_loop:
    mov                 nDstStrideq, pSrcq      ;back pSrcq for next y loop
    movdqu              xmm3,  [pSrcq]
    punpcklbw           xmm3, xmm3, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm4,  [pSrcq]
    punpcklbw           xmm4, xmm4, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm5,  [pSrcq]
    punpcklbw           xmm5, xmm5, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm6,  [pSrcq]
    punpcklbw           xmm6, xmm6, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm7,  [pSrcq]
    punpcklbw           xmm7, xmm7, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq

%ifidn %1, put	                ;put case, shift 6 bits right and get final result
    v1_8                true
    movq                [pDstq], xmm6
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    v1_8                false
    movdqu              [pDstq], xmm6
%elifidn %1, avg	            ;avg case, shift 7 bits right
    v1_8                false
    movdqu              xmm0,  [nWidthq]
    paddsw               xmm0, xmm0, xmm6
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2  ;xmm2 can be any REG
    movq                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif

    movdqa              xmm0, xmm1
    movdqa              xmm1, xmm2
    movdqa              xmm2, xmm3

    mov                 pSrcq, nDstStrideq          ;back pSrcq for next y loop
    add                 pSrcq, nSrcStrideq

    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    add                 pDstq, nDstStrideq

    sub                 nHeightd,  1
    jg                  %%h265_qpel8_v1_y_loop    

%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif 


%endmacro


%macro  h265_qpel8_v2_neon 1 ;%type 
    movdqu              xmm0,  [pSrcq]
    punpcklbw           xmm0, xmm0, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm1,  [pSrcq]
    punpcklbw           xmm1, xmm1, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm2,  [pSrcq]
    punpcklbw           xmm2, xmm2, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif
    
%%h265_qpel8_v2_y_loop:
    mov                 nDstStrideq, pSrcq      ;back pSrcq for next y loop
    movdqu              xmm3,  [pSrcq]
    punpcklbw           xmm3, xmm3, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm4,  [pSrcq]
    punpcklbw           xmm4, xmm4, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm5,  [pSrcq]
    punpcklbw           xmm5, xmm5, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm6,  [pSrcq]
    punpcklbw           xmm6, xmm6, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm7,  [pSrcq]
    punpcklbw           xmm7, xmm7, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq

%ifidn %1, put	                ;put case, shift 6 bits right and get final result
    v2_8                true
    movq                [pDstq], xmm6
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    v2_8                false
    movdqu              [pDstq], xmm6
%elifidn %1, avg	            ;avg case, shift 7 bits right
    v2_8                false
    movdqu              xmm0,  [nWidthq]
    paddsw               xmm0, xmm0, xmm6
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2  ;xmm2 can be any REG
    movq                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif

    movdqa              xmm0, xmm1
    movdqa              xmm1, xmm2
    movdqa              xmm2, xmm3

    mov                 pSrcq, nDstStrideq          ;back pSrcq for next y loop
    add                 pSrcq, nSrcStrideq

    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    add                 pDstq, nDstStrideq

    sub                 nHeightd,  1
    jg                  %%h265_qpel8_v2_y_loop    

%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif 

%endmacro

%macro  h265_qpel8_v3_neon 1 ;%type

    movdqu              xmm1,  [pSrcq]
    punpcklbw           xmm1, xmm1, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
  

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif
    
%%h265_qpel8_v3_y_loop:
    mov                 nDstStrideq, pSrcq      ;back pSrcq for next y loop
    movdqu              xmm2,  [pSrcq]
    punpcklbw           xmm2, xmm2, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm3,  [pSrcq]
    punpcklbw           xmm3, xmm3, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm4,  [pSrcq]
    punpcklbw           xmm4, xmm4, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm5,  [pSrcq]
    punpcklbw           xmm5, xmm5, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm6,  [pSrcq]
    punpcklbw           xmm6, xmm6, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm7,  [pSrcq]
    punpcklbw           xmm7, xmm7, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq

%ifidn %1, put	                ;put case, shift 6 bits right and get final result
    v3_8                true
    movq                [pDstq], xmm6
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    v3_8                false
    movdqu              [pDstq], xmm6
%elifidn %1, avg	            ;avg case, shift 7 bits right
    v3_8                false
    movdqu              xmm0,  [nWidthq]
    paddsw               xmm0, xmm0, xmm6
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2  ;xmm2 can be any REG
    movq                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif

    movdqa              xmm0, xmm1
    movdqa              xmm1, xmm2
 

    mov                 pSrcq, nDstStrideq          ;back pSrcq for next y loop
    add                 pSrcq, nSrcStrideq

    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    add                 pDstq, nDstStrideq

    sub                 nHeightd,  1
    jg                  %%h265_qpel8_v3_y_loop    

%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif 
 


%endmacro

%macro  h265_qpel8_h1v1_neon 1 ;%type 
    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7   ; 7 = filter_size-1
    h123_8_filter 1
%%h265_qpel8_h1v1_H_loop:
    movdqu              xmm0,  [pSrcq]

    ;h1_8                false
    h123_8            false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h1v1_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm  ;nHeightd is modified in H loop

     
    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_qpel8_h1v1_V_loop: 

    v1_8_2_mul          0 
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0
    v2_8_2_add          16
    movd                [pDstq+4], xmm0
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0
    v2_8_2_add          16, false
    movq                [pDstq+8], xmm0
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    
    v2_8_2_add          16, false
    movdqu              xmm1,  [nWidthq+8]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq+4], xmm0 
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*6
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h1v1_V_loop

%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif



%endmacro

%macro  h265_qpel8_h1v2_neon 1 ;%type 
    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7   ; 7 = filter_size-1
    h123_8_filter     1
%%h265_qpel8_h1v2_H_loop:
    movdqu              xmm0,  [pSrcq]

    ;h1_8                false
    h123_8            false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h1v2_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm  ;nHeightd is modified in H loop

     
    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_qpel8_h1v2_V_loop: 

    v2_8_2_mul          0 
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0
    v2_8_2_add          16
    movd                [pDstq+4], xmm0
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0
    v2_8_2_add          16, false
    movq                [pDstq+8], xmm0
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    
    v2_8_2_add          16, false
    movdqu              xmm1,  [nWidthq+8]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq+4], xmm0 
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*6
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h1v2_V_loop

%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif


%endmacro

%macro  h265_qpel8_h1v3_neon 1 ;%type 
    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7   ; 7 = filter_size-1
    h123_8_filter   1
%%h265_qpel8_h1v3_H_loop:
    movdqu              xmm0,  [pSrcq]

    ;h1_8                false
    h123_8            false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h1v3_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm  ;nHeightd is modified in H loop

     
    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_qpel8_h1v3_V_loop: 

    v3_8_2_mul          0 
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0
    v2_8_2_add          16
    movd                [pDstq+4], xmm0
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0
    v2_8_2_add          16, false
    movq                [pDstq+8], xmm0
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    
    v2_8_2_add          16, false
    movdqu              xmm1,  [nWidthq+8]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq+4], xmm0 
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*5
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h1v3_V_loop

%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif


%endmacro

%macro  h265_qpel8_h2v1_neon 1 ;%type 
    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7   ; 7 = filter_size-1
    h123_8_filter 2
%%h265_qpel8_h2v1_H_loop:
    movdqu              xmm0,  [pSrcq]

    h123_8                false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h2v1_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm  ;nHeightd is modified in H loop

     
    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_qpel8_h2v1_V_loop: 

    v1_8_2_mul          0 
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0
    v2_8_2_add          16
    movd                [pDstq+4], xmm0
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0
    v2_8_2_add          16, false
    movq                [pDstq+8], xmm0
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    
    v2_8_2_add          16, false
    movdqu              xmm1,  [nWidthq+8]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq+4], xmm0 
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*6
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h2v1_V_loop

%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif


%endmacro

%macro  h265_qpel8_h2v2_neon 1 ;%type 
    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7   ; 7 = filter_size-1
    h123_8_filter 2
%%h265_qpel8_h2v2_H_loop:
    movdqu              xmm0,  [pSrcq]

    h123_8                false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h2v2_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm  ;nHeightd is modified in H loop

     
    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_qpel8_h2v2_V_loop: 

    v2_8_2_mul          0 
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0
    v2_8_2_add          16
    movd                [pDstq+4], xmm0
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0
    v2_8_2_add          16, false
    movq                [pDstq+8], xmm0
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    
    v2_8_2_add          16, false
    movdqu              xmm1,  [nWidthq+8]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq+4], xmm0 
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*6
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h2v2_V_loop

%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif

%endmacro

%macro  h265_qpel8_h2v3_neon 1 ;%type 

    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7   ; 7 = filter_size-1
    h123_8_filter 2
%%h265_qpel8_h2v3_H_loop:
    movdqu              xmm0,  [pSrcq]

    h123_8                false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h2v3_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm  ;nHeightd is modified in H loop

     
    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_qpel8_h2v3_V_loop: 

    v3_8_2_mul          0 
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0
    v2_8_2_add          16
    movd                [pDstq+4], xmm0
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0
    v2_8_2_add          16, false
    movq                [pDstq+8], xmm0
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    
    v2_8_2_add          16, false
    movdqu              xmm1,  [nWidthq+8]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq+4], xmm0 
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*5
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h2v3_V_loop

%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif

%endmacro

%macro  h265_qpel8_h3v1_neon 1 ;%type 

    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7   ; 7 = filter_size-1
    h123_8_filter 3
%%h265_qpel8_h3v1_H_loop:
    movdqu              xmm0,  [pSrcq]

    h123_8                false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h3v1_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm  ;nHeightd is modified in H loop

     
    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_qpel8_h3v1_V_loop: 

    v1_8_2_mul          0 
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0
    v2_8_2_add           16
    movd                [pDstq+4], xmm0
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0
    v2_8_2_add          16, false
    movq                [pDstq+8], xmm0
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    
    v2_8_2_add          16, false
    movdqu              xmm1,  [nWidthq+8]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq+4], xmm0 
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*6
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h3v1_V_loop

%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif

%endmacro

%macro  h265_qpel8_h3v2_neon 1 ;%type 

    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7   ; 7 = filter_size-1
    h123_8_filter 3
%%h265_qpel8_h3v2_H_loop:
    movdqu              xmm0,  [pSrcq]

    h123_8                false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h3v2_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm  ;nHeightd is modified in H loop

     
    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_qpel8_h3v2_V_loop: 

    v2_8_2_mul          0 
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0
    v2_8_2_add          16
    movd                [pDstq+4], xmm0
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0
    v2_8_2_add          16, false
    movq                [pDstq+8], xmm0
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    
    v2_8_2_add          16, false
    movdqu              xmm1,  [nWidthq+8]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq+4], xmm0 
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*6
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h3v2_V_loop

%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif

%endmacro

%macro  h265_qpel8_h3v3_neon 1 ;%type 

    set_HxVx_H_result

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  7   ; 7 = filter_size-1
    h123_8_filter 3
%%h265_qpel8_h3v3_H_loop:
    movdqu              xmm0,  [pSrcq]

    h123_8                false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h3v3_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm  ;nHeightd is modified in H loop

     
    ;V part, load data from block_temp
    mov                 pSrcq, HxVx_H_resultmp

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_qpel8_h3v3_V_loop: 

    v3_8_2_mul          0 
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    v2_8_2_add          0
    movd                [pDstq], xmm0
    v2_8_2_add          16
    movd                [pDstq+4], xmm0
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    v2_8_2_add          0, false
    movq                [pDstq], xmm0
    v2_8_2_add          16, false
    movq                [pDstq+8], xmm0
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    v2_8_2_add          0, false
    movdqu              xmm1,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    
    v2_8_2_add          16, false
    movdqu              xmm1,  [nWidthq+8]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq+4], xmm0 
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*5
    sub                 nHeightd,  1
    jg                  %%h265_qpel8_h3v3_V_loop

%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif

%endmacro

%macro  h265_qpel16_h0v0_neon 1 ;%type 

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_qpel16_h0v0_y_loop:

    movdqu              xmm0,  [pSrcq]

%ifidn %1, put	                ;put case
    movdqu              [pDstq], xmm0
%elifidn %1, avg_nornd	        ;avg_nornd case, , shift 6 left
    movdqa              xmm2, xmm0     ;backup xmm0 into xmm2
    punpcklbw           xmm0, xmm0, [CONST_U8_0]
    punpckhbw           xmm2, xmm2, [CONST_U8_0]
    psllw               xmm0, xmm0, 6
    psllw               xmm2, xmm2, 6
    movdqu              [pDstq], xmm0
    movdqu              [pDstq+16], xmm2
%elifidn %1, avg	            ;avg case, shift  7 bits right 
    movdqa               xmm2, xmm0     ;backup xmm0 into xmm2
    punpcklbw            xmm0, xmm0, [CONST_U8_0]
    punpckhbw            xmm2, xmm2, [CONST_U8_0]
    psllw                xmm0, xmm0, 6
    psllw                xmm2, xmm2, 6
    movdqu               xmm1,  [nWidthq]
    movdqu               xmm3,  [nWidthq+16]
    paddsw                xmm0, xmm0, xmm1
    paddsw                xmm2, xmm2, xmm3
    round_7bits          xmm0
    round_7bits          xmm2
    packuswb             xmm0, xmm0, xmm1
    packuswb             xmm2, xmm2, xmm1
    movq                 [pDstq], xmm0
    movq                 [pDstq+8], xmm2
    add                  nWidthq, nDstStride2mp    
%endif

    add                  pSrcq, nSrcStrideq
    add                  pDstq, nDstStrideq
    sub                  nHeightd,  1
    jg                   %%h265_qpel16_h0v0_y_loop 

%ifidn %1, avg	        ;move pDst2 16 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 32
    mov                 [pDst2mx], nWidthq
%endif 

%endmacro

%macro  h265_qpel32_h0v0_neon 1 ;%type 

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_qpel32_h0v0_y_loop:

    movdqu              xmm0,  [pSrcq]
    movdqu              xmm4,  [pSrcq+16]

%ifidn %1, put	                ;put case
    movdqu              [pDstq], xmm0
    movdqu              [pDstq+16], xmm4
%elifidn %1, avg_nornd	        ;avg_nornd case, , shift 6 left
    movdqa              xmm2, xmm0     ;backup xmm0 into xmm2
    movdqa              xmm6, xmm4     ;backup xmm4 into xmm6
    punpcklbw           xmm0, xmm0, [CONST_U8_0]
    punpckhbw           xmm2, xmm2, [CONST_U8_0]
    punpcklbw           xmm4, xmm4, [CONST_U8_0]
    punpckhbw           xmm6, xmm6, [CONST_U8_0]
    psllw               xmm0, xmm0, 6
    psllw               xmm2, xmm2, 6
    psllw               xmm4, xmm4, 6
    psllw               xmm6, xmm6, 6
    movdqu              [pDstq], xmm0
    movdqu              [pDstq+16], xmm2
    movdqu              [pDstq+32], xmm4
    movdqu              [pDstq+48], xmm6
%elifidn %1, avg	            ;avg case, shift  7 bits right 
    movdqa              xmm2, xmm0     ;backup xmm0 into xmm2
    movdqa              xmm6, xmm4     ;backup xmm4 into xmm6
    punpcklbw           xmm0, xmm0, [CONST_U8_0]
    punpckhbw           xmm2, xmm2, [CONST_U8_0]
    punpcklbw           xmm4, xmm4, [CONST_U8_0]
    punpckhbw           xmm6, xmm6, [CONST_U8_0]
    psllw               xmm0, xmm0, 6
    psllw               xmm2, xmm2, 6
    psllw               xmm4, xmm4, 6
    psllw               xmm6, xmm6, 6
    movdqu              xmm1,  [nWidthq]
    movdqu              xmm3,  [nWidthq+16]
    movdqu              xmm5,  [nWidthq+32]
    movdqu              xmm7,  [nWidthq+48]
    paddsw               xmm0, xmm0, xmm1
    paddsw               xmm2, xmm2, xmm3
    paddsw               xmm4, xmm4, xmm5
    paddsw               xmm6, xmm6, xmm7
    round_7bits         xmm0
    round_7bits         xmm2
    round_7bits         xmm4
    round_7bits         xmm6
    packuswb            xmm0, xmm0, xmm1
    packuswb            xmm2, xmm2, xmm1
    packuswb            xmm4, xmm4, xmm1
    packuswb            xmm6, xmm6, xmm1
    movq                [pDstq], xmm0
    movq                [pDstq+8], xmm2
    movq                [pDstq+16], xmm4
    movq                [pDstq+24], xmm6
    add                 nWidthq, nDstStride2mp
%endif

    add                 pSrcq, nSrcStrideq
    add                 pDstq, nDstStrideq
    sub                 nHeightd,  1
    jg                  %%h265_qpel32_h0v0_y_loop 

%ifidn %1, avg	        ;move pDst2 32 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 64
    mov                 [pDst2mx], nWidthq
%endif 
;
%endmacro

;input: all
;output: 
;args: %type
%macro MC_InterLuma_PutPixels  1 
    %if ARCH_X86_64
        ; write Regs into stack for accessing later  , only x64 
        mov		        pSrcmp,   pSrcq
        mov             pDstmp, pDstq
        mov             nSrcStridemp, nSrcStrideq
        mov             nDstStridemp, nDstStrideq     

        ; extend sign, data is in stack, d->q, then save to stack   
        mov             nDstStrided, nDstStride2m  ;@reuse  nDstStride
        mov             nDstStride2mp, nDstStrideq 
        mov             nDstStrideq, nDstStridemp
    %endif

     %ifidn %1, avg_nornd
       ;set pDst and pDstStride as pDst2 and nDstStride2
        mov             pDstq, pDst2mp
        mov             nDstStrideq, nDstStride2mp       
        mov             pDstmp, pDstq
        mov             nDstStridemp, nDstStrideq  
     %endif	


%%MC_InterLuma_H0V0_width_loop:
    mov                 nHeightd, nHeightm  ;nHeight is modified in Y loop. resotre it from stack
    cmp                 nWidthd,     32
    jl                  %%MC_InterLuma_H0V0_width_lt_32 


    h265_qpel32_h0v0_neon %1 ;%type
    mov                 pSrcq,  pSrcmp  ;get from stack
    mov                 pDstq,  pDstmp	
    add                 pSrcq,  32
 %ifidn %1, avg_nornd	
    add                 pDstq,  64
%else
    add                 pDstq,  32
%endif
%ifidn %1, avg	        ;avg case,  retrive nWidthd from nWidthm
    mov                 nWidthd, nWidthm
%endif
    sub                 nWidthd,     32
%ifidn %1, avg	        ;avg case, save nWidthd into nWidthm
    mov                 nWidthm, nWidthd
%endif 
    mov                 pSrcmp,  pSrcq ;write into stack
    mov                 pDstmp,  pDstq  
    jg                  %%MC_InterLuma_H0V0_width_loop
    jmp                 %%MC_InterLuma_H0V0_end
 

%%MC_InterLuma_H0V0_width_lt_32:
    cmp                 nWidthd,     16
    jl                  %%MC_InterLuma_H0V0_width_lt_16 
 

    h265_qpel16_h0v0_neon %1 ;%type
    mov                 pSrcq,  pSrcmp  ;get from stack
    mov                 pDstq,  pDstmp	
    add                 pSrcq,  16
%ifidn %1, avg_nornd	
    add                 pDstq,  32
%else
    add                 pDstq,  16
%endif
%ifidn %1, avg	        ;avg case,  retrive nWidthd from nWidthm
    mov                 nWidthd, nWidthm
%endif
    sub                 nWidthd,     16
%ifidn %1, avg	        ;avg case, save nWidthd into nWidthm
    mov                 nWidthm, nWidthd
%endif 
    mov                 pSrcmp,  pSrcq ;write into stack
    mov                 pDstmp,  pDstq 
    jg                  %%MC_InterLuma_H0V0_width_loop
    jmp                 %%MC_InterLuma_H0V0_end

%%MC_InterLuma_H0V0_width_lt_16:
    cmp                 nWidthd,     8
    jl                  %%MC_InterLuma_H0V0_width_lt_8 


    h265_qpel8_h0v0_neon %1 ;%type
    mov                 pSrcq,  pSrcmp  ;get from stack
    mov                 pDstq,  pDstmp	
    add                 pSrcq,  8
 %ifidn %1, avg_nornd	
    add                 pDstq,  16
%else
    add                 pDstq,  8
%endif
%ifidn %1, avg	        ;avg case,  retrive nWidthd from nWidthm
    mov                 nWidthd, nWidthm
%endif
    sub                 nWidthd,     8
%ifidn %1, avg	        ;avg case, save nWidthd into nWidthm
    mov                 nWidthm, nWidthd
%endif 
    mov                 pSrcmp,  pSrcq ;write into stack
    mov                 pDstmp,  pDstq 
    jg                  %%MC_InterLuma_H0V0_width_loop
    jmp                 %%MC_InterLuma_H0V0_end

%%MC_InterLuma_H0V0_width_lt_8:
    cmp                 nWidthd,     4
    jl                  %%MC_InterLuma_H0V0_width_lt_4 
    

    h265_qpel4_h0v0_neon %1 ;%type
    mov                 pSrcq,  pSrcmp  ;get from stack
    mov                 pDstq,  pDstmp	
    add                 pSrcq, 4
%ifidn %1, avg_nornd	
    add                 pDstq,  8
%else
    add                 pDstq,  4
%endif
%ifidn %1, avg	        ;avg case,  retrive nWidthd from nWidthm
    mov                 nWidthd, nWidthm
%endif
    sub                 nWidthd,     4
%ifidn %1, avg	        ;avg case, save nWidthd into nWidthm
    mov                 nWidthm, nWidthd
%endif
    mov                 pSrcmp,  pSrcq ;write into stack
    mov                 pDstmp,  pDstq 
    jg                  %%MC_InterLuma_H0V0_width_loop
    jmp                 %%MC_InterLuma_H0V0_end

%%MC_InterLuma_H0V0_width_lt_4:
    h265_qpel2_h0v0_neon %1 ;%type

    mov                 pSrcq,  pSrcmp  ;get from stack
    mov                 pDstq,  pDstmp	
    add                 pSrcq,  2
%ifidn %1, avg_nornd	
    add                 pDstq,  4
%else
    add                 pDstq,  2
%endif
%ifidn %1, avg	        ;avg case,  retrive nWidthd from nWidthm
    mov                 nWidthd, nWidthm
%endif
    sub                 nWidthd,     2
%ifidn %1, avg	        ;avg case, save nWidthd into nWidthm
    mov                 nWidthm, nWidthd
%endif 
    mov                 pSrcmp,  pSrcq ;write into stack
    mov                 pDstmp,  pDstq 
    jg                  %%MC_InterLuma_H0V0_width_loop
    jmp                 %%MC_InterLuma_H0V0_end
    

%%MC_InterLuma_H0V0_end:

%endmacro

;input: all
;output: 
;args: %type, %name, %src_pos,  %tap  
%macro MC_InterLuma_HxVx  4  

    %if ARCH_X86_64
        ; write Regs into stack for accessing later  , only x64 
        mov		        pSrcmp,   pSrcq
        mov             pDstmp, pDstq   
        mov             nDstStridemp, nDstStrideq

        ; extend sign, data is in stack, d->q, then save to stack       
        mov             nDstStrided, nDstStride2m   ;@reuse  nDstStride
        mov             nDstStride2mp, nDstStrideq 
        mov             nDstStrideq, nDstStridemp
    %endif
    ;
     %ifidn %1, avg_nornd
       ;set pDst and pDstStride as pDst2 and nDstStride2
        mov             pDstq, pDst2mp
        mov             nDstStrideq, nDstStride2mp        
        mov             pDstmp, pDstq
        mov             nDstStridemp, nDstStrideq  
     %endif	

    %3                  ;%src_pos	
    mov                 pSrcmp,  pSrcq  ;write into stack for next width loop

%%MC_InterLuma_HxVx_width_loop:
    mov                 nHeightd, nHeightm
    cmp                 nWidthd,     8    
    jl                  %%MC_InterLuma_HxVx_width_case4	
    h265_qpel8_%2_neon  %1 ;%type
			 			 

    mov                 pSrcq,  pSrcmp
    mov                 pDstq,  pDstmp
    add                 pSrcq,  8
%ifidn %1, avg_nornd	
    add                 pDstq,  16
%else
    add                 pDstq,  8
%endif
    mov                 pSrcmp,  pSrcq
    mov                 pDstmp,  pDstq
%ifidn %1, avg	        ;avg case,  retrive nWidthd from nWidthm
    mov                 nWidthd, nWidthm
%endif
    sub                 nWidthd,     8
%ifidn %1, avg	        ;avg case, save nWidthd into nWidthm
    mov                 nWidthm, nWidthd
%endif
    jg                  %%MC_InterLuma_HxVx_width_loop  
    jmp                 %%MC_InterLuma_HxVx_End
    
%%MC_InterLuma_HxVx_width_case4:  

    h265_qpel4_%2_neon  %1  ;%type 
    mov                 pSrcq,  pSrcmp
    mov                 pDstq,  pDstmp			 
    add                 pSrcq,  4
%ifidn %1, avg_nornd	
    add                 pDstq,  8
%else
    add                 pDstq,  4
%endif
    mov                 pSrcmp,  pSrcq
    mov                 pDstmp,  pDstq
%ifidn %1, avg	        ;avg case,  retrive nWidthd from nWidthm
    mov                 nWidthd, nWidthm
%endif
    sub                 nWidthd,     4
%ifidn %1, avg	        ;avg case, save nWidthd into nWidthm
    mov                 nWidthm, nWidthd
%endif
    jg                  %%MC_InterLuma_HxVx_width_loop  
%%MC_InterLuma_HxVx_End:


%endmacro

%macro MC_HxVx_prologue 1 ; args: hasStack
    %ifidn %1, true	     
        DECLARE_ALIGNED_LOCAL_BUF V_temp, 0  ;V_temp size = 8 S32 * 8 = 32*8 = 256
        DECLARE_ALIGNED_LOCAL_BUF HxVx_H_result, 256  ;HxVx_H_result size =  8 + 8(pad) = 16
    %endif
%endmacro



%macro MC_InterLuma 1-2+ ; args: type, args

    cglobal VO_%1_MC_InterLuma_H0V0_neon, %2
        MC_InterLuma_PutPixels %1
        emms
    RET
    cglobal VO_%1_MC_InterLuma_H1V0_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx %1, h1, x_3y_0, tap1_const
        emms 
    RET	
    cglobal VO_%1_MC_InterLuma_H2V0_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx %1, h2, x_3y_0, tap2_const
        emms 
    RET	
    cglobal VO_%1_MC_InterLuma_H3V0_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx %1, h3, x_3y_0, tap3_const
        emms 
    RET	
    cglobal VO_%1_MC_InterLuma_H0V1_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx %1, v1, x_0y_3, tap8_1_const
        emms 
    RET	
    cglobal VO_%1_MC_InterLuma_H0V2_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx %1, v2, x_0y_3, tap8_2_const
        emms 
    RET	
    cglobal VO_%1_MC_InterLuma_H0V3_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx %1, v3, x_0y_2, tap8_3_const
        emms 
    RET	
    cglobal VO_%1_MC_InterLuma_H1V1_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx %1, h1v1, x_3y_3, tap123_const
        emms 
    RET	
    cglobal VO_%1_MC_InterLuma_H1V2_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx %1, h1v2, x_3y_3, tap123_const
        emms 
    RET	
    cglobal VO_%1_MC_InterLuma_H1V3_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx %1, h1v3, x_3y_2, tap123_const
        emms 
    RET	
    cglobal VO_%1_MC_InterLuma_H2V1_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx %1, h2v1, x_3y_3, tap123_const
        emms 
    RET	
    cglobal VO_%1_MC_InterLuma_H2V2_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx %1, h2v2, x_3y_3, tap123_const
        emms 
    RET	
    cglobal VO_%1_MC_InterLuma_H2V3_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx %1, h2v3, x_3y_2, tap123_const
        emms 
    RET
    cglobal VO_%1_MC_InterLuma_H3V1_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx %1, h3v1, x_3y_3, tap123_const
        emms 
    RET
    cglobal VO_%1_MC_InterLuma_H3V2_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx %1, h3v2, x_3y_3, tap123_const
        emms 
    RET
    cglobal VO_%1_MC_InterLuma_H3V3_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx %1, h3v3, x_3y_2, tap123_const
        emms 
    RET	
%endmacro


%macro MC_InterLumaPut 1-2+ ; args: type, args

    cglobal VO_put_MC_InterLuma_H0V0_neon, %2
        MC_InterLuma_PutPixels put
        emms
    RET
    cglobal VO_put_MC_InterLuma_H1V0_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx put, h1, x_3y_0, tap1_const
        emms 
    RET	
    cglobal VO_put_MC_InterLuma_H2V0_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx put, h2, x_3y_0, tap2_const
        emms 
    RET	
    cglobal VO_put_MC_InterLuma_H3V0_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx put, h3, x_3y_0, tap3_const
        emms 
    RET	
    cglobal VO_put_MC_InterLuma_H0V1_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx put, v1, x_0y_3, tap8_1_const
        emms 
    RET	
    cglobal VO_put_MC_InterLuma_H0V2_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx put, v2, x_0y_3, tap8_2_const
        emms 
    RET	
    cglobal VO_put_MC_InterLuma_H0V3_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx put, v3, x_0y_2, tap8_3_const
        emms 
    RET	
    cglobal VO_put_MC_InterLuma_H1V1_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx put, h1v1, x_3y_3, tap123_const
        emms 
    RET	
    cglobal VO_put_MC_InterLuma_H1V2_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx put, h1v2, x_3y_3, tap123_const
        emms 
    RET	
    cglobal VO_put_MC_InterLuma_H1V3_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx put, h1v3, x_3y_2, tap123_const
        emms 
    RET	
    cglobal VO_put_MC_InterLuma_H2V1_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx put, h2v1, x_3y_3, tap123_const
        emms 
    RET	
    cglobal VO_put_MC_InterLuma_H2V2_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx put, h2v2, x_3y_3, tap123_const
        emms 
    RET	
    cglobal VO_put_MC_InterLuma_H2V3_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx put, h2v3, x_3y_2, tap123_const
        emms 
    RET
    cglobal VO_put_MC_InterLuma_H3V1_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx put, h3v1, x_3y_3, tap123_const
        emms 
    RET
    cglobal VO_put_MC_InterLuma_H3V2_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx put, h3v2, x_3y_3, tap123_const
        emms 
    RET
    cglobal VO_put_MC_InterLuma_H3V3_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx put, h3v3, x_3y_2, tap123_const
        emms 
    RET	
%endmacro


%macro MC_InterLumaAvgNornd 1-2+ ; args: type, args

    cglobal VO_avg_nornd_MC_InterLuma_H0V0_neon, %2
        MC_InterLuma_PutPixels avg_nornd
        emms
    RET
    cglobal VO_avg_nornd_MC_InterLuma_H1V0_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx avg_nornd, h1, x_3y_0, tap1_const
        emms 
    RET	
    cglobal VO_avg_nornd_MC_InterLuma_H2V0_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx avg_nornd, h2, x_3y_0, tap2_const
        emms 
    RET	
    cglobal VO_avg_nornd_MC_InterLuma_H3V0_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx avg_nornd, h3, x_3y_0, tap3_const
        emms 
    RET	
    cglobal VO_avg_nornd_MC_InterLuma_H0V1_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx avg_nornd, v1, x_0y_3, tap8_1_const
        emms 
    RET	
    cglobal VO_avg_nornd_MC_InterLuma_H0V2_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx avg_nornd, v2, x_0y_3, tap8_2_const
        emms 
    RET	
    cglobal VO_avg_nornd_MC_InterLuma_H0V3_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx avg_nornd, v3, x_0y_2, tap8_3_const
        emms 
    RET	
    cglobal VO_avg_nornd_MC_InterLuma_H1V1_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg_nornd, h1v1, x_3y_3, tap123_const
        emms 
    RET	
    cglobal VO_avg_nornd_MC_InterLuma_H1V2_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg_nornd, h1v2, x_3y_3, tap123_const
        emms 
    RET	
    cglobal VO_avg_nornd_MC_InterLuma_H1V3_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg_nornd, h1v3, x_3y_2, tap123_const
        emms 
    RET	
    cglobal VO_avg_nornd_MC_InterLuma_H2V1_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg_nornd, h2v1, x_3y_3, tap123_const
        emms 
    RET	
    cglobal VO_avg_nornd_MC_InterLuma_H2V2_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg_nornd, h2v2, x_3y_3, tap123_const
        emms 
    RET	
    cglobal VO_avg_nornd_MC_InterLuma_H2V3_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg_nornd, h2v3, x_3y_2, tap123_const
        emms 
    RET
    cglobal VO_avg_nornd_MC_InterLuma_H3V1_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg_nornd, h3v1, x_3y_3, tap123_const
        emms 
    RET
    cglobal VO_avg_nornd_MC_InterLuma_H3V2_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg_nornd, h3v2, x_3y_3, tap123_const
        emms 
    RET
    cglobal VO_avg_nornd_MC_InterLuma_H3V3_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg_nornd, h3v3, x_3y_2, tap123_const
        emms 
    RET	
%endmacro

%macro MC_InterLumaAvg 1-2+ ; args: type, args

    cglobal VO_avg_MC_InterLuma_H0V0_neon, %2
        MC_InterLuma_PutPixels avg
        emms
    RET
    cglobal VO_avg_MC_InterLuma_H1V0_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx avg, h1, x_3y_0, tap1_const
        emms 
    RET	
    cglobal VO_avg_MC_InterLuma_H2V0_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx avg, h2, x_3y_0, tap2_const
        emms 
    RET	
    cglobal VO_avg_MC_InterLuma_H3V0_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx avg, h3, x_3y_0, tap3_const
        emms 
    RET	
    cglobal VO_avg_MC_InterLuma_H0V1_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx avg, v1, x_0y_3, tap8_1_const
        emms 
    RET	
    cglobal VO_avg_MC_InterLuma_H0V2_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx avg, v2, x_0y_3, tap8_2_const
        emms 
    RET	
    cglobal VO_avg_MC_InterLuma_H0V3_neon, %2
        MC_HxVx_prologue false
        MC_InterLuma_HxVx avg, v3, x_0y_2, tap8_3_const
        emms 
    RET	
    cglobal VO_avg_MC_InterLuma_H1V1_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg, h1v1, x_3y_3, tap123_const
        emms 
    RET	
    cglobal VO_avg_MC_InterLuma_H1V2_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg, h1v2, x_3y_3, tap123_const
        emms 
    RET	
    cglobal VO_avg_MC_InterLuma_H1V3_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg, h1v3, x_3y_2, tap123_const
        emms 
    RET	
    cglobal VO_avg_MC_InterLuma_H2V1_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg, h2v1, x_3y_3, tap123_const
        emms 
    RET	
    cglobal VO_avg_MC_InterLuma_H2V2_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg, h2v2, x_3y_3, tap123_const
        emms 
    RET	
    cglobal VO_avg_MC_InterLuma_H2V3_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg, h2v3, x_3y_2, tap123_const
        emms 
    RET
    cglobal VO_avg_MC_InterLuma_H3V1_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg, h3v1, x_3y_3, tap123_const
        emms 
    RET
    cglobal VO_avg_MC_InterLuma_H3V2_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg, h3v2, x_3y_3, tap123_const
        emms 
    RET
    cglobal VO_avg_MC_InterLuma_H3V3_neon, %2
        MC_HxVx_prologue true
        MC_InterLuma_HxVx avg, h3v3, x_3y_2, tap123_const
        emms 
    RET	
%endmacro

;/************************************************************************
; 
; Chroma part
;
;************************************************************************/
%macro epel_x_1y0 0	
    sub                 pSrcq, 1   
%endmacro

%macro epel_x0y_1 0	
    sub                 pSrcq, nSrcStrideq			    
%endmacro

%macro epel_x_1y_1 0		    
    sub                 pSrcq, nSrcStrideq
    sub                 pSrcq, 1  			  
%endmacro

;input:  mm0
;output: mm1
;args:  %narrow 
%macro  chroma_eqpel4_h1_8 1

    psrlq               mm1, mm0, 8
    punpcklbw           mm1, mm1, [CONST_U8_0]
    pmullw              mm1, mm1,  mm5

    psrlq               mm2, mm0, 16
    punpcklbw           mm2, mm2, [CONST_U8_0]
    pmullw              mm2, mm2,  mm6

    psrlq               mm3, mm0, 24
    punpcklbw           mm3, mm3, [CONST_U8_0]
    pmullw              mm3, mm3,  mm7

    punpcklbw           mm0, mm0, [CONST_U8_0] 
    pmullw              mm0, mm0,  mm4

    psubsw              mm1, mm1, mm0
    psubsw              mm2, mm2, mm3
    paddsw              mm1, mm1, mm2
   

     %ifidn %1, true
        paddsw          mm1, mm1, [CONST_U16_POS32]
        psraw           mm1, mm1, 6
        packuswb        mm1, mm1, mm2
    %endif
%endmacro



;input:  mm0; mm4~mm7, coeff
;output: mm
;args:  %narrow 
%macro  chroma_eqpel4_v1_8 1


    pmullw              mm0, mm0,  mm4    ; 
    pmullw              mm1, mm1,  mm5    ;  
    pmullw              mm2, mm2,  mm6    ; 
    pmullw              mm3, mm3,  mm7    ; 

    psubsw              mm1, mm1, mm0
    psubsw              mm2, mm2, mm3
    paddsw              mm1, mm1, mm2

    %ifidn %1, true
        paddsw          mm1, mm1, [CONST_U16_POS32]
        psraw           mm1, mm1, 6
        packuswb        mm1, mm1, mm2
    %endif

%endmacro


;input: pSrcq, is 16 bit(from 16 bit H filter)
;output: V_tempmx, 32 bit. (16*16->32)
;args:  %offset 
%macro  chroma_v1_4_2_mul 1 
    pmullw              mm0, [pSrcq], mm4
    pmulhw              mm1, [pSrcq], mm4
    punpcklwd           mm2, mm0, mm1
    punpckhwd           mm3, mm0, mm1
    movq              [V_tempmx+16*0], mm2
    movq              [V_tempmx+16*0+8], mm3 
    add                 pSrcq, block_temp_stride

    pmullw              mm0, [pSrcq], mm5
    pmulhw              mm1, [pSrcq], mm5
    punpcklwd           mm2, mm0, mm1
    punpckhwd           mm3, mm0, mm1  
    movq              [V_tempmx+16*2], mm2
    movq              [V_tempmx+16*2+8], mm3
    add                 pSrcq, block_temp_stride

    pmullw              mm0, [pSrcq], mm6
    pmulhw              mm1, [pSrcq], mm6
    punpcklwd           mm2, mm0, mm1
    punpckhwd           mm3, mm0, mm1
    movq              [V_tempmx+16*4], mm2
    movq              [V_tempmx+16*4+8], mm3 
    add                 pSrcq, block_temp_stride

    pmullw              mm0, [pSrcq], mm7
    pmulhw              mm1, [pSrcq], mm7
    punpcklwd           mm2, mm0, mm1
    punpckhwd           mm3, mm0, mm1  
    movq              [V_tempmx+16*6], mm2
    movq              [V_tempmx+16*6+8], mm3
;
%endmacro

;input: V_tempmx, is 32 bit(from V filter)
;output: mm1
;args:  %offset, %narrow 
%macro  chroma_v2_4_2_add 1-2 true  

    movdqu              xmm0, [V_tempmx+16*0+%1]
    movdqu              xmm1, [V_tempmx+16*2+%1]
    movdqu              xmm2, [V_tempmx+16*4+%1]
    movdqu              xmm3, [V_tempmx+16*6+%1]

    psubd              xmm1, xmm1, xmm0
    psubd              xmm2, xmm2, xmm3
    paddd              xmm1, xmm1, xmm2

    %ifidn %2, true
        paddd           xmm1, xmm1, [CONST_U32_POS2048]
        psrad           xmm1, xmm1, 12
        packssdw        xmm1, xmm1, xmm2  
        packuswb        xmm1, xmm1, xmm2
        
    %else      
        psrad           xmm1, xmm1, 6
        packssdw        xmm1, xmm1, xmm2  
    %endif
%endmacro

;input: V_tempmx, is 32 bit(from V filter)
;output: mm1
;args:  %offset, %narrow 
%macro  chroma_v2_2_2_add 1-2 true  

    movq              mm0, [V_tempmx+16*0+%1]
    movq              mm1, [V_tempmx+16*2+%1]
    movq              mm2, [V_tempmx+16*4+%1]
    movq              mm3, [V_tempmx+16*6+%1]

    psubd              mm1, mm1, mm0
    psubd              mm2, mm2, mm3
    paddd              mm1, mm1, mm2

    %ifidn %2, true
        paddd           mm1, mm1, [CONST_U32_POS2048]
        psrad           mm1, mm1, 12       
        packssdw        mm1, mm1, mm2  
        packuswb        mm1, mm1, mm2
        
    %else      
        psrad           mm1, mm1, 6      
        packssdw        mm1, mm1, mm2 
    %endif
%endmacro

;input:  xmm0
;output: xmm1
;args:  %narrow 
;load chroma coeff into xmm4, xmm5, xmm6, xmm7
%macro  chroma_load_coeff_xmm 1

movdqa xmm4, [%1]
movdqa xmm5, [%1+16]
movdqa xmm6, [%1+32]
movdqa xmm7, [%1+48]

%endmacro

%macro  chroma_load_H_coeff_xmm 1

;movdqa xmm4, [%1]
;movdqa xmm5, [%1+16]
movdqa xmm5, [CONST_U16_POS32]
movdqa xmm6, [%1]
movdqa xmm7, [%1+16]

%endmacro

;input:  xmm0
;output: xmm1
;args:  %narrow 
;load chroma coeff into xmm4, xmm5, xmm6, xmm7
%macro  chroma_load_coeff_mm 1

movq mm4, [%1]
movq mm5, [%1+16]
movq mm6, [%1+32]
movq mm7, [%1+48]

%endmacro

%macro  chroma_load_H_coeff_mm 1

;movq mm4, [%1]
;movq mm5, [%1+16]
movq mm6, [%1]
movq mm7, [%1+16]

%endmacro

;input:  xmm0
;output: xmm1
;args:  %narrow 
%macro  chroma_eqpel8_h1_8 1

;    psrldq              xmm1, xmm0, 1
;    punpcklbw           xmm1, xmm1, [CONST_U8_0]
;    pmullw              xmm1, xmm1,  xmm5

;    psrldq              xmm2, xmm0, 2
;    punpcklbw           xmm2, xmm2, [CONST_U8_0]
;    pmullw              xmm2, xmm2,  xmm6

;    psrldq              xmm3, xmm0, 3
;    punpcklbw           xmm3, xmm3, [CONST_U8_0]
;    pmullw              xmm3, xmm3,  xmm7

;    punpcklbw           xmm0, xmm0, [CONST_U8_0] 
;    pmullw              xmm0, xmm0,  xmm4

;    psubsw              xmm1, xmm1, xmm0
;    psubsw              xmm2, xmm2, xmm3
;    paddsw              xmm1, xmm1, xmm2
   movdqa              xmm1, xmm0
   psrldq              xmm3, xmm0, 1
   punpcklbw           xmm1, xmm3
   pmaddubsw           xmm1, xmm6

   psrldq              xmm2, xmm0, 2
   psrldq              xmm3, xmm2, 1
   punpcklbw           xmm2, xmm3
   pmaddubsw           xmm2, xmm7
   paddw               xmm1, xmm2
   

     %ifidn %1, true
        paddsw          xmm1, xmm1, xmm5
        psraw           xmm1, xmm1, 6
        packuswb        xmm1, xmm1, xmm2
    %endif
%endmacro



;input:  xmm0; xmm4~xmm7, coeff
;output: xmm
;args:  %narrow 
%macro  chroma_eqpel8_v1_8 1


    pmullw              xmm0, xmm0,  xmm4    ; 
    pmullw              xmm1, xmm1,  xmm5    ;  
    pmullw              xmm2, xmm2,  xmm6    ; 
    pmullw              xmm3, xmm3,  xmm7    ; 

    psubsw              xmm1, xmm1, xmm0
    psubsw              xmm2, xmm2, xmm3
    paddsw              xmm1, xmm1, xmm2

    %ifidn %1, true
        paddsw          xmm1, xmm1, [CONST_U16_POS32]
        psraw           xmm1, xmm1, 6
        packuswb        xmm1, xmm1, xmm2
    %endif

%endmacro


;input: pSrcq, is 16 bit(from 16 bit H filter)
;output: V_tempmx, 32 bit. (16*16->32)
;args:  %offset 
%macro  chroma_v1_8_2_mul 1 
    pmullw              xmm0, [pSrcq], xmm4
    pmulhw              xmm1, [pSrcq], xmm4
    punpcklwd           xmm2, xmm0, xmm1
    punpckhwd           xmm3, xmm0, xmm1
    movdqa              [V_tempmx+16*0], xmm2
    movdqa              [V_tempmx+16*1], xmm3 
    add                 pSrcq, block_temp_stride

    pmullw              xmm0, [pSrcq], xmm5
    pmulhw              xmm1, [pSrcq], xmm5
    punpcklwd           xmm2, xmm0, xmm1
    punpckhwd           xmm3, xmm0, xmm1  
    movdqa              [V_tempmx+16*2], xmm2
    movdqa              [V_tempmx+16*3], xmm3
    add                 pSrcq, block_temp_stride

    pmullw              xmm0, [pSrcq], xmm6
    pmulhw              xmm1, [pSrcq], xmm6
    punpcklwd           xmm2, xmm0, xmm1
    punpckhwd           xmm3, xmm0, xmm1
    movdqa              [V_tempmx+16*4], xmm2
    movdqa              [V_tempmx+16*5], xmm3 
    add                 pSrcq, block_temp_stride

    pmullw              xmm0, [pSrcq], xmm7
    pmulhw              xmm1, [pSrcq], xmm7
    punpcklwd           xmm2, xmm0, xmm1
    punpckhwd           xmm3, xmm0, xmm1  
    movdqa              [V_tempmx+16*6], xmm2
    movdqa              [V_tempmx+16*7], xmm3

%endmacro

;h265_eqpel2 start

%macro h265_eqpel2_h1v0_neon 1

    lea                 pSrcq,  [g_chromaFilter_x86]
    mov                 nWidthq,  xFracmp
    sal                 nWidthq, 6         ; coeff length of one group is 64 bytes
    lea                 pSrcq, [pSrcq+nWidthq]
    chroma_load_coeff_mm pSrcq

    mov                 pSrcq, pSrcmp  ;@reuse.  pSrcq

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_eqpel2_h1_y_loop:
    movq                mm0,  [pSrcq]
%ifidn %1, put	                ;put case, shift 6 right and get final result
    chroma_eqpel4_h1_8  true
    movd                nSrcStrided, mm1  ;@reuse nSrcStrideq
    mov                 [pDstq], nSrcStridew
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    chroma_eqpel4_h1_8  false
    movd                [pDstq], mm1
%elifidn %1, avg	            ;avg case, shift  7 bits right 
    chroma_eqpel4_h1_8  false
    movq                mm0,  [nWidthq]
    paddsw               mm0, mm0, mm1
    round_7bits         mm0
    packuswb            mm0, mm0, mm2
    movd                nSrcStrided, mm0  ;@reuse nSrcStrideq
    mov                 [pDstq], nSrcStridew
    add                 nWidthq, nDstStride2mp
%endif
    mov                 nSrcStrideq, nSrcStridemp  ;@reuse nSrcStrideq
    add                 pSrcq, nSrcStrideq
    add                 pDstq, nDstStrideq
    sub                 nHeightd,  1
    jg                  %%h265_eqpel2_h1_y_loop 
    
%ifidn %1, avg	        ;move pDst2 2 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 4
    mov                 [pDst2mx], nWidthq
%endif  


%endmacro


%macro h265_eqpel2_h0v1_neon 1

    lea                 pSrcq,  [g_chromaFilter_x86] ;@reuse.  pSrcq
    mov                 nWidthq,  yFracmp
    sal                 nWidthq, 6         ; coeff length of one group is 64 bytes
    lea                 pSrcq, [pSrcq+nWidthq]
    chroma_load_coeff_mm pSrcq

    mov                 pSrcq, pSrcmp  ;@reuse.  restore pSrcq

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif
    
%%h265_eqpel2_v1_y_loop:
    mov                 nDstStrideq, pSrcq      ;back pSrcq for next y loop
    movq                mm0,  [pSrcq]
    punpcklbw           mm0, mm0, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movq                mm1,  [pSrcq]
    punpcklbw           mm1, mm1, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movq                mm2,  [pSrcq]
    punpcklbw           mm2, mm2, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movq                mm3,  [pSrcq]
    punpcklbw           mm3, mm3, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
  

%ifidn %1, put	                ;put case, shift 6 bits right and get final result
    chroma_eqpel4_v1_8  true
    movd                nSrcStrided, mm1  ;@reuse nSrcStrideq
    mov                 [pDstq], nSrcStridew
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    chroma_eqpel4_v1_8  false
    movd                [pDstq], mm1
%elifidn %1, avg	            ;avg case, shift 7 bits right
    chroma_eqpel4_v1_8  false
    movq                mm0,  [nWidthq]
    paddsw               mm0, mm0, mm1
    round_7bits         mm0
    packuswb            mm0, mm0, mm2  ;mm2 can be any REG
    movd                nSrcStrided, mm0  ;@reuse nSrcStrideq
    mov                 [pDstq], nSrcStridew
    add                 nWidthq, nDstStride2mp
%endif
    mov                 nSrcStrideq, nSrcStridemp  ;@reuse nSrcStrideq
    mov                 pSrcq, nDstStrideq          ;back pSrcq for next y loop
    add                 pSrcq, nSrcStrideq

    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    add                 pDstq, nDstStrideq

    sub                 nHeightd,  1
    jg                  %%h265_eqpel2_v1_y_loop    

%ifidn %1, avg	        ;move pDst2 2 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 4
    mov                 [pDst2mx], nWidthq
%endif 


%endmacro

%macro h265_eqpel2_h1v1_neon 1

    set_HxVx_H_result

    lea                 pSrcq,  [g_chromaFilter_x86] ;@reuse.  pSrcq
    mov                 nWidthq,  xFracmp
    sal                 nWidthq, 6         ; coeff length of one group is 64 bytes
    lea                 pSrcq, [pSrcq+nWidthq]
    chroma_load_coeff_mm pSrcq

    mov                 pSrcq, pSrcmp  ;@reuse.  restore pSrcq

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  3   ; 3 = filter_size-1
%%h265_eqpel2_h1v1_H_loop:
    movq                mm0,  [pSrcq]

    chroma_eqpel4_h1_8  false
    movq                [pDstq], mm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_eqpel2_h1v1_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm  ;nHeightd is modified in H loop

     
    ;V part, load data from block_temp
    lea                 pSrcq,  [g_chromaFilter_x86] ;@reuse.  pSrcq
    mov                 nWidthq,  yFracmp
    sal                 nWidthq, 6         ; coeff length of one group is 64 bytes
    lea                 pSrcq, [pSrcq+nWidthq]
    chroma_load_coeff_mm pSrcq


    mov                 pSrcq, HxVx_H_resultmp

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_eqpel2_h1v1_V_loop: 

    chroma_v1_4_2_mul   0 
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    chroma_v2_2_2_add   0
    movd                nSrcStrided, mm1  ;@reuse nSrcStrideq
    mov                 [pDstq], nSrcStridew
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    chroma_v2_2_2_add   0, false
    movd                [pDstq], mm1
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    chroma_v2_2_2_add   0, false
    movq                mm0,  [nWidthq]
    paddsw               mm0, mm0, mm1
    round_7bits         mm0
    packuswb            mm0, mm0, mm2
    movd                nSrcStrided, mm0  ;@reuse nSrcStrideq
    mov                 [pDstq], nSrcStridew
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*2   ;pSrcq moves 3 rows behind in chroma_v1_8_2_mul 
    sub                 nHeightd,  1
    jg                  %%h265_eqpel2_h1v1_V_loop

    mov                 nSrcStrideq, nSrcStridemp  ;@reuse nSrcStrideq

%ifidn %1, avg	        ;move pDst2 2 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 4
    mov                 [pDst2mx], nWidthq
%endif

%endmacro

;h265_eqpel2 end


;h265_eqpel4 start

%macro h265_eqpel4_h1v0_neon 1

    lea                 pSrcq,  [g_chromaFilter_x86]
    mov                 nWidthq,  xFracmp
    sal                 nWidthq, 6         ; coeff length of one group is 64 bytes
    lea                 pSrcq, [pSrcq+nWidthq]
    chroma_load_coeff_mm pSrcq

    mov                 pSrcq, pSrcmp  ;@reuse.  pSrcq

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_eqpel4_h1_y_loop:
    movq                mm0,  [pSrcq]
%ifidn %1, put	                ;put case, shift 6 right and get final result
    chroma_eqpel4_h1_8  true
    movd                [pDstq], mm1
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    chroma_eqpel4_h1_8  false
    movq                [pDstq], mm1
%elifidn %1, avg	            ;avg case, shift  7 bits right 
    chroma_eqpel4_h1_8  false
    movq                mm0,  [nWidthq]
    paddsw               mm0, mm0, mm1
    round_7bits         mm0
    packuswb            mm0, mm0, mm2
    movd                [pDstq], mm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pSrcq, nSrcStrideq
    add                 pDstq, nDstStrideq
    sub                 nHeightd,  1
    jg                  %%h265_eqpel4_h1_y_loop 
    
%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif  


%endmacro


%macro h265_eqpel4_h0v1_neon 1

    lea                 pSrcq,  [g_chromaFilter_x86] ;@reuse.  pSrcq
    mov                 nWidthq,  yFracmp
    sal                 nWidthq, 6         ; coeff length of one group is 64 bytes
    lea                 pSrcq, [pSrcq+nWidthq]
    chroma_load_coeff_mm pSrcq

    mov                 pSrcq, pSrcmp  ;@reuse.  restore pSrcq

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif
    
%%h265_eqpel4_v1_y_loop:
    mov                 nDstStrideq, pSrcq      ;back pSrcq for next y loop
    movq                mm0,  [pSrcq]
    punpcklbw           mm0, mm0, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movq                mm1,  [pSrcq]
    punpcklbw           mm1, mm1, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movq                mm2,  [pSrcq]
    punpcklbw           mm2, mm2, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movq                mm3,  [pSrcq]
    punpcklbw           mm3, mm3, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
  

%ifidn %1, put	                ;put case, shift 6 bits right and get final result
    chroma_eqpel4_v1_8  true
    movd                [pDstq], mm1
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    chroma_eqpel4_v1_8  false
    movq                [pDstq], mm1
%elifidn %1, avg	            ;avg case, shift 7 bits right
    chroma_eqpel4_v1_8  false
    movq                mm0,  [nWidthq]
    paddsw               mm0, mm0, mm1
    round_7bits         mm0
    packuswb            mm0, mm0, mm2  ;mm2 can be any REG
    movd                [pDstq], mm0
    add                 nWidthq, nDstStride2mp
%endif

    mov                 pSrcq, nDstStrideq          ;back pSrcq for next y loop
    add                 pSrcq, nSrcStrideq

    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    add                 pDstq, nDstStrideq

    sub                 nHeightd,  1
    jg                  %%h265_eqpel4_v1_y_loop    

%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif 


%endmacro

%macro h265_eqpel4_h1v1_neon 1

    set_HxVx_H_result

    lea                 pSrcq,  [g_chromaFilter_x86] ;@reuse.  pSrcq
    mov                 nWidthq,  xFracmp
    sal                 nWidthq, 6         ; coeff length of one group is 64 bytes
    lea                 pSrcq, [pSrcq+nWidthq]
    chroma_load_coeff_mm pSrcq

    mov                 pSrcq, pSrcmp  ;@reuse.  restore pSrcq

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  3   ; 3 = filter_size-1
%%h265_eqpel4_h1v1_H_loop:
    movq                mm0,  [pSrcq]

    chroma_eqpel4_h1_8  false
    movq                [pDstq], mm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_eqpel4_h1v1_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm  ;nHeightd is modified in H loop

     
    ;V part, load data from block_temp
    lea                 pSrcq,  [g_chromaFilter_x86] ;@reuse.  pSrcq
    mov                 nWidthq,  yFracmp
    sal                 nWidthq, 6         ; coeff length of one group is 64 bytes
    lea                 pSrcq, [pSrcq+nWidthq]
    chroma_load_coeff_mm pSrcq


    mov                 pSrcq, HxVx_H_resultmp

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_eqpel4_h1v1_V_loop: 

    chroma_v1_4_2_mul   0 
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    chroma_v2_4_2_add   0
    movd                [pDstq], xmm1
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    chroma_v2_4_2_add   0, false
    movq                [pDstq], xmm1
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    chroma_v2_4_2_add   0, false
    movq                xmm0,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*2   ;pSrcq moves 3 rows behind in chroma_v1_8_2_mul 
    sub                 nHeightd,  1
    jg                  %%h265_eqpel4_h1v1_V_loop

%ifidn %1, avg	        ;move pDst2 4 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 8
    mov                 [pDst2mx], nWidthq
%endif

%endmacro

;h265_eqpel4 ends

%macro h265_eqpel8_h1v0_neon 1

    lea                 pSrcq,  [g_chromaFilter_x86_8]
    mov                 nWidthq,  xFracmp
    sal                 nWidthq, 5         ; coeff length of one group is 64 bytes
    lea                 pSrcq, [pSrcq+nWidthq]
    chroma_load_H_coeff_xmm pSrcq

    mov                 pSrcq, pSrcmp  ;@reuse.  pSrcq

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_eqpel8_h1_y_loop:
    movdqu              xmm0,  [pSrcq]
%ifidn %1, put	                ;put case, shift 6 right and get final result
    chroma_eqpel8_h1_8  true
    movq                [pDstq], xmm1
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    chroma_eqpel8_h1_8  false
    movdqu              [pDstq], xmm1
%elifidn %1, avg	            ;avg case, shift  7 bits right 
    chroma_eqpel8_h1_8  false
    movdqu              xmm0,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movq                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif
    add                 pSrcq, nSrcStrideq
    add                 pDstq, nDstStrideq
    sub                 nHeightd,  1
    jg                  %%h265_eqpel8_h1_y_loop 
    
%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif  


%endmacro

%macro h265_eqpel8_h0v1_neon 1

    lea                 pSrcq,  [g_chromaFilter_x86] ;@reuse.  pSrcq
    mov                 nWidthq,  yFracmp
    sal                 nWidthq, 6         ; coeff length of one group is 64 bytes
    lea                 pSrcq, [pSrcq+nWidthq]
    chroma_load_coeff_xmm pSrcq

    mov                 pSrcq, pSrcmp  ;@reuse.  restore pSrcq

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif
    
%%h265_eqpel8_v1_y_loop:
    mov                 nDstStrideq, pSrcq      ;back pSrcq for next y loop
    movdqu              xmm0,  [pSrcq]
    punpcklbw           xmm0, xmm0, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm1,  [pSrcq]
    punpcklbw           xmm1, xmm1, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm2,  [pSrcq]
    punpcklbw           xmm2, xmm2, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
    movdqu              xmm3,  [pSrcq]
    punpcklbw           xmm3, xmm3, [CONST_U8_0]
    add                 pSrcq, nSrcStrideq
  

%ifidn %1, put	                ;put case, shift 6 bits right and get final result
    chroma_eqpel8_v1_8  true
    movq                [pDstq], xmm1
%elifidn %1, avg_nornd	        ;avg_nornd case, no shift
    chroma_eqpel8_v1_8  false
    movdqu              [pDstq], xmm1
%elifidn %1, avg	            ;avg case, shift 7 bits right
    chroma_eqpel8_v1_8  false
    movdqu              xmm0,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2  ;xmm2 can be any REG
    movq                [pDstq], xmm0
    add                 nWidthq, nDstStride2mp
%endif

    mov                 pSrcq, nDstStrideq          ;back pSrcq for next y loop
    add                 pSrcq, nSrcStrideq

    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    add                 pDstq, nDstStrideq

    sub                 nHeightd,  1
    jg                  %%h265_eqpel8_v1_y_loop    

%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif 


%endmacro



%macro h265_eqpel8_h1v1_neon 1

    set_HxVx_H_result

    lea                 pSrcq,  [g_chromaFilter_x86_8] ;@reuse.  pSrcq
    mov                 nWidthq,  xFracmp
    sal                 nWidthq, 5         ; coeff length of one group is 64 bytes
    lea                 pSrcq, [pSrcq+nWidthq]
    chroma_load_H_coeff_xmm pSrcq

    mov                 pSrcq, pSrcmp  ;@reuse.  restore pSrcq

    ;H part, save temp result into block_temp
    mov                 nDstStrideq, pDstq ;@reuse. using nDstStrideq to  backup pDstq
    mov                 pDstq, HxVx_H_resultmp
    add                 nHeightd,  3   ; 3 = filter_size-1
%%h265_eqpel8_h1v1_H_loop:
    movdqu              xmm0,  [pSrcq]

    chroma_eqpel8_h1_8  false
    movdqu              [pDstq], xmm1

    add                 pSrcq, nSrcStrideq
    add                 pDstq, block_temp_stride
    sub                 nHeightd,  1
    jg                  %%h265_eqpel8_h1v1_H_loop  

    mov                 pDstq, nDstStrideq
    mov                 nDstStrideq, nDstStridemp  ; restore  nDstStrideq
    mov                 nHeightd,  nHeightm  ;nHeightd is modified in H loop

     
    ;V part, load data from block_temp
    lea                 pSrcq,  [g_chromaFilter_x86] ;@reuse.  pSrcq
    mov                 nWidthq,  yFracmp
    sal                 nWidthq, 6         ; coeff length of one group is 64 bytes
    lea                 pSrcq, [pSrcq+nWidthq]
    chroma_load_coeff_xmm pSrcq


    mov                 pSrcq, HxVx_H_resultmp

%ifidn %1, avg	        
    mov                 nWidthq, [pDst2mx]  ;@reuse. avg case, using nWidth register to keep pDst2
%endif

%%h265_eqpel8_h1v1_V_loop: 

    chroma_v1_8_2_mul   0 
%ifidn %1, put	                ;put case, shift 12 bits right and get final result
    chroma_v2_4_2_add   0
    movd                [pDstq], xmm1
    chroma_v2_4_2_add    16
    movd                [pDstq+4], xmm1
%elifidn %1, avg_nornd	        ;avg_nornd case, shift 6 bits right
    chroma_v2_4_2_add   0, false
    movq                [pDstq], xmm1
    chroma_v2_4_2_add    16, false
    movq                [pDstq+8], xmm1
%elifidn %1, avg	            ;avg case, shift 6 bits right firstly;  add;  shift 7 bits right 
    chroma_v2_4_2_add   0, false
    movdqu              xmm0,  [nWidthq]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq], xmm0
    
    chroma_v2_4_2_add   16, false
    movdqu              xmm0,  [nWidthq+8]
    paddsw               xmm0, xmm0, xmm1
    round_7bits         xmm0
    packuswb            xmm0, xmm0, xmm2
    movd                [pDstq+4], xmm0 
    add                 nWidthq, nDstStride2mp
%endif
    add                 pDstq, nDstStrideq
    sub                 pSrcq, block_temp_stride*2   ;pSrcq moves 3 rows behind in chroma_v1_8_2_mul 
    sub                 nHeightd,  1
    jg                  %%h265_eqpel8_h1v1_V_loop

%ifidn %1, avg	        ;move pDst2 8 word right
    mov                 nWidthq, [pDst2mx] 
    add                 nWidthq, 16
    mov                 [pDst2mx], nWidthq
%endif

%endmacro


%macro MC_InterChroma_HxVx  4 

    %if ARCH_X86_64
        ; write Regs into stack for accessing later  , only x64 
        mov		        pSrcmp, pSrcq
        mov             pDstmp, pDstq 
        mov             nDstStridemp, nDstStrideq 
        mov             nSrcStridemp, nSrcStrideq 

        ; extend sign, data is in stack, d->q, then save to stack 
        mov             nDstStrided, nDstStride2m  ;@reuse  nDstStride
        mov             nDstStride2mp, nDstStrideq 
        mov             nDstStrided, xFracm       
        mov             xFracmp, nDstStrideq
        mov             nDstStrided, yFracm     
        mov             yFracmp, nDstStrideq

        mov             nDstStrideq, nDstStridemp
    %endif
    
     %ifidn %1, avg_nornd
       ;set pDst and pDstStride as pDst2 and nDstStride2
        mov             pDstq, pDst2mp
        mov             nDstStrideq, nDstStride2mp       
        mov             pDstmp, pDstq
        mov             nDstStridemp, nDstStrideq  
     %endif	

    DECLARE_ALIGNED_LOCAL_BUF V_temp, 0  ;V_temp size = 8 S32 * 8 = 32*8 = 256
    DECLARE_ALIGNED_LOCAL_BUF HxVx_H_result, 256  ;HxVx_H_result size =  8 + 8(pad) = 16
  
    %3                  ;%src_pos	
    mov                 pSrcmp,  pSrcq  ;write into stack for next width loop
  
%%MC_InterChroma_HxVx_width_loop:
    mov                 nHeightd, nHeightm
    cmp                 nWidthd,     8    
    jl                  %%MC_InterChroma_HxVx_width_case4	
    h265_eqpel8_%2_neon  %1 ;%type
			 			 

    mov                 pSrcq,  pSrcmp
    mov                 pDstq,  pDstmp
    mov                 nWidthd, nWidthm   ;nWidth is reused 
    add                 pSrcq,  8
%ifidn %1, avg_nornd	
    add                 pDstq,  16
%else
    add                 pDstq,  8
%endif
    sub                 nWidthd,     8
    mov                 pSrcmp,  pSrcq
    mov                 pDstmp,  pDstq
    mov                 nWidthm, nWidthd

    jg                  %%MC_InterChroma_HxVx_width_loop  
    jmp                 %%MC_InterChroma_HxVx_End
    
%%MC_InterChroma_HxVx_width_case4:
    mov                 nWidthd, nWidthm   ;nWidth is reused 	
    cmp                 nWidthd,     4    
    jl                  %%MC_InterChroma_HxVx_width_case2	

    h265_eqpel4_%2_neon  %1  ;%type 
    mov                 pSrcq,  pSrcmp
    mov                 pDstq,  pDstmp
    mov                 nWidthd, nWidthm   ;nWidth is reused 			 
    add                 pSrcq,  4
%ifidn %1, avg_nornd	
    add                 pDstq,  8
%else
    add                 pDstq,  4
%endif
    sub                 nWidthd,     4
    mov                 pSrcmp,  pSrcq
    mov                 pDstmp,  pDstq
    mov                 nWidthm, nWidthd

    jg                  %%MC_InterChroma_HxVx_width_loop
    jmp                 %%MC_InterChroma_HxVx_End
    
%%MC_InterChroma_HxVx_width_case2:   

    h265_eqpel2_%2_neon  %1  ;%type 
    mov                 pSrcq,  pSrcmp
    mov                 pDstq,  pDstmp
    mov                 nWidthd, nWidthm   ;nWidth is reused 			 
    add                 pSrcq,  2
%ifidn %1, avg_nornd	
    add                 pDstq,  4
%else
    add                 pDstq,  2
%endif
    sub                 nWidthd,     2
    mov                 pSrcmp,  pSrcq
    mov                 pDstmp,  pDstq
    mov                 nWidthm, nWidthd

    jg                  %%MC_InterChroma_HxVx_width_loop    
%%MC_InterChroma_HxVx_End:
%endmacro


%macro MC_InterChroma 1-2+ ; args: type, args
    cglobal VO_%1_MC_InterChroma_H0V0_neon, %2
        MC_InterLuma_PutPixels %1 
        emms
    RET
    cglobal VO_%1_MC_InterChroma_H1V0_neon, %2      
        MC_InterChroma_HxVx %1, h1v0, epel_x_1y0, tap1_const
        emms 
    RET	   
    cglobal VO_%1_MC_InterChroma_H0V1_neon, %2
        MC_InterChroma_HxVx %1, h0v1, epel_x0y_1, tap8_1_const
        emms 
    RET	  
    cglobal VO_%1_MC_InterChroma_H1V1_neon, %2
        MC_InterChroma_HxVx %1, h1v1, epel_x_1y_1, tap123_const
        emms 
    RET	   
%endmacro

%macro MC_InterChromaPut 1-2+ ; args: type, args
    cglobal VO_put_MC_InterChroma_H0V0_neon, %2
        MC_InterLuma_PutPixels put 
        emms
    RET
    cglobal VO_put_MC_InterChroma_H1V0_neon, %2      
        MC_InterChroma_HxVx put, h1v0, epel_x_1y0, tap1_const
        emms 
    RET	   
    cglobal VO_put_MC_InterChroma_H0V1_neon, %2
        MC_InterChroma_HxVx put, h0v1, epel_x0y_1, tap8_1_const
        emms 
    RET	  
    cglobal VO_put_MC_InterChroma_H1V1_neon, %2
        MC_InterChroma_HxVx put, h1v1, epel_x_1y_1, tap123_const
        emms 
    RET	   
%endmacro

%macro MC_InterChromaAvgNornd 1-2+ ; args: type, args
    cglobal VO_avg_nornd_MC_InterChroma_H0V0_neon, %2
        MC_InterLuma_PutPixels avg_nornd 
        emms
    RET
    cglobal VO_avg_nornd_MC_InterChroma_H1V0_neon, %2      
        MC_InterChroma_HxVx avg_nornd, h1v0, epel_x_1y0, tap1_const
        emms 
    RET	   
    cglobal VO_avg_nornd_MC_InterChroma_H0V1_neon, %2
        MC_InterChroma_HxVx avg_nornd, h0v1, epel_x0y_1, tap8_1_const
        emms 
    RET	  
    cglobal VO_avg_nornd_MC_InterChroma_H1V1_neon, %2
        MC_InterChroma_HxVx avg_nornd, h1v1, epel_x_1y_1, tap123_const
        emms 
    RET	   
%endmacro


%macro MC_InterChromaAvg 1-2+ ; args: type, args
    cglobal VO_avg_MC_InterChroma_H0V0_neon, %2
        MC_InterLuma_PutPixels avg 
        emms
    RET
    cglobal VO_avg_MC_InterChroma_H1V0_neon, %2      
        MC_InterChroma_HxVx avg, h1v0, epel_x_1y0, tap1_const
        emms 
    RET	   
    cglobal VO_avg_MC_InterChroma_H0V1_neon, %2
        MC_InterChroma_HxVx avg, h0v1, epel_x0y_1, tap8_1_const
        emms 
    RET	  
    cglobal VO_avg_MC_InterChroma_H1V1_neon, %2
        MC_InterChroma_HxVx avg, h1v1, epel_x_1y_1, tap123_const
        emms 
    RET	   
%endmacro




  MC_InterLumaPut put, 6,6,8,272, pDst, nDstStride, pSrc, nSrcStride, nWidth, nHeight, pDst2, nDstStride2  ;using pDst2, nDstStride2 to pass HxVx_H_result addr
  MC_InterLumaAvgNornd avg_nornd, 6,6,8,272, pDst, nDstStride, pSrc, nSrcStride,  nWidth, nHeight, pDst2, nDstStride2
  MC_InterLumaAvg avg, 6,6,8,272, pDst, nDstStride, pSrc, nSrcStride,  nWidth, nHeight, pDst2, nDstStride2

  MC_InterChromaPut put, 6,6,8,272, pDst, nDstStride, pSrc, nSrcStride, nWidth, nHeight,  pDst2, nDstStride2, xFrac, yFrac
  MC_InterChromaAvgNornd avg_nornd, 6,6,8,272, pDst, nDstStride, pSrc, nSrcStride,  nWidth, nHeight, pDst2, nDstStride2, xFrac, yFrac
  MC_InterChromaAvg avg, 6,6,8,272, pDst, nDstStride, pSrc, nSrcStride,  nWidth, nHeight, pDst2, nDstStride2, xFrac, yFrac


end                                                        ; Mark end of file 
