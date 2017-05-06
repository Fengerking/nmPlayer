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

    cextern CONST_U16_POS8
    cextern CONST_U16_0
    cextern CONST_S16_POS4
    cextern CONST_U16_2
    section  .text

	
    align    4  
    
    
;/************************************************************************
; MACRO:            transpose_load8X8
; function:      prepare input for vertical filter
; args:            
; regs:
; steps:               
;************************************************************************/      
        
%macro cal_stride_mul3     6 ; args: ;src, stride, src+3*stride, 3*stide, flag(-4), flag[-4*stride]

    lea             %4, [4*%2]  
%ifidn %5, true
    sub             %1, 4 
%endif
%ifidn %6, true
    sub             %1, %4     ;[src-4*stride-4]
%endif
    sub             %4, %2
    mov             %3, %1
    add             %3, %4    
    
%endmacro


;/************************************************************************
; MACRO:            transpose_load8X8
; function:      prepare input for vertical filter
; args:            
; regs:
; steps:               
;************************************************************************/      
        
%macro transpose_load8X8     4 ; args: ;src, stride, src+3*stride, 3*stide

    movq            xmm7, [%1]
    movq            xmm3, [%1+%2]
    movq            xmm5, [%1+2*%2]
    movq            xmm1, [%3]
    movq            xmm4, [%3+%2]
    movq            xmm2, [%3+2*%2]
    movq            xmm6, [%3+%4]
    movq            xmm0, [%3+4*%2]

    punpcklbw       xmm7, xmm7, xmm3
    punpcklbw       xmm5, xmm5, xmm1
    punpcklbw       xmm4, xmm4, xmm2
    punpcklbw       xmm6, xmm6, xmm0

    movq            xmm3, xmm7
    movq            xmm1, xmm5
    movq            xmm2, xmm4
    movq            xmm0, xmm6
    punpckhwd       xmm7, xmm7, xmm5
    punpckhwd       xmm4, xmm4, xmm6
    punpcklwd       xmm3, xmm3, xmm1
    punpcklwd       xmm2, xmm2, xmm0
    
    movq            xmm1, xmm3
    movq            xmm5, xmm7

    punpckldq       xmm1, xmm2  ;0, 1
    punpckhdq       xmm3, xmm2  ;2, 3

    punpckldq       xmm5, xmm4  ;4, 5 
    punpckhdq       xmm7, xmm4  ;6 ,7

    pxor            xmm0, xmm0

    movq            xmm2, xmm3
    movq            xmm4, xmm5
    movq            xmm6, xmm7

    punpckhbw       xmm7, xmm7, xmm0 ;7 row
    punpcklbw       xmm2, xmm2, xmm0 ;2 row
    punpckhbw       xmm3, xmm3, xmm0 ;3 row
    punpcklbw       xmm4, xmm4, xmm0 ;4 row
    punpckhbw       xmm5, xmm5, xmm0 ;5 row
    punpcklbw       xmm6, xmm6, xmm0 ;6 row

    movdqa          [src_tempmx+16*7], xmm7 ;save 7 row
    movq            xmm7, xmm1      ;reuse xmm7


    punpcklbw       xmm7, xmm7, xmm0 ;0 row
    punpckhbw       xmm1, xmm1, xmm0 ;1 row
    movdqa          [src_tempmx+16*0], xmm7 ;save 0 row

    movdqa          [src_tempmx+16*1], xmm1 ;save 1 row   
    movdqa          [src_tempmx+16*2], xmm2 ;save 2 row
    movdqa          [src_tempmx+16*3], xmm3 ;save 3 row
    movdqa          [src_tempmx+16*4], xmm4 ;save 4 row   
    movdqa          [src_tempmx+16*5], xmm5 ;save 5 row
    movdqa          [src_tempmx+16*6], xmm6 ;save 6 row 
                
%endmacro

;/************************************************************************
; MACRO:            load8X4_BW
; function:      prepare input for horizontal. input: frame buffer
; args:            
; regs:
; steps:               
;************************************************************************/      
        
%macro load8X4_BW     4 ; args: ;src, stride, src+3*stride, 3*stide

    movd            xmm0, [%1]            ;load 0 row  
    movd            xmm1, [%1+%2]
    movd            xmm2, [%1+2*%2]
    movd            xmm3, [%3]
    movd            xmm4, [%3+%2]
    movd            xmm5, [%3+2*%2]
    movd            xmm6, [%3+%4] 
  
    pxor            xmm7, xmm7
    punpcklbw       xmm0, xmm0, xmm7 
    movq            [src_tempmx+16*0], xmm0 ;save 1 row   
    punpcklbw       xmm1, xmm1, xmm7 
    punpcklbw       xmm2, xmm2, xmm7 
    punpcklbw       xmm3, xmm3, xmm7 
    punpcklbw       xmm4, xmm4, xmm7 
    punpcklbw       xmm5, xmm5, xmm7 
    punpcklbw       xmm6, xmm6, xmm7 
    pxor            xmm0, xmm0
    movq            xmm7, [%3+4*%2] ;load 7 row  
    punpcklbw       xmm7, xmm7, xmm0
    
    movq            [src_tempmx+16*1], xmm1 ;save 1 row   
    movq            [src_tempmx+16*2], xmm2 
    movq            [src_tempmx+16*3], xmm3  
    movq            [src_tempmx+16*4], xmm4   
    movq            [src_tempmx+16*5], xmm5   
    movq            [src_tempmx+16*6], xmm6    
    movq            [src_tempmx+16*7], xmm7  
                
%endmacro

;/************************************************************************
; MACRO:            transpose_load8X4_BW
; function:      prepare input for vertical. input: frame buffer
; args:            
; regs:
; steps:               
;************************************************************************/      
        
%macro transpose_load8X4_BW     4 ; args: ;src, stride, src+3*stride, 3*stide

    movq            xmm4, [%1]            ;load 0 row  
    movq            xmm0, [%1+%2]
    movq            xmm5, [%1+2*%2]
    movq            xmm1, [%3]
 
    punpcklbw       xmm4, xmm4, xmm0
    punpcklbw       xmm5, xmm5, xmm1

    movq            xmm0, xmm4    
    movq            xmm1, xmm5

    punpcklwd       xmm0, xmm0, xmm1    ;3 2 1 0
    punpckhwd       xmm4, xmm4, xmm5    ;7 6 5 4


    movhlps         xmm2, xmm2, xmm0   ; xmm2 low: 3 2
    movhlps         xmm6, xmm6, xmm4  ; xmm6 low: 7 6

    pxor            xmm7, xmm7
    punpcklbw       xmm0, xmm0, xmm7    ; 1 0 
    punpcklbw       xmm2, xmm2, xmm7     ; 3 2
    punpcklbw       xmm4, xmm4, xmm7   ; 5 4 
    punpcklbw       xmm6, xmm6, xmm7    ; 7 6

    movhlps         xmm1, xmm1, xmm0   ; xmm2 low: 3 2
    movhlps         xmm3, xmm3, xmm2   ; xmm3 low: 7 6
    movhlps         xmm5, xmm5, xmm4   ; xmm2 low: 3 2
    movhlps         xmm7, xmm7, xmm6   ; xmm3 low: 7 6
    
    movq            [src_tempmx+16*0], xmm0 ;save 1 row   
    movq            [src_tempmx+16*1], xmm1 ;save 1 row   
    movq            [src_tempmx+16*2], xmm2 
    movq            [src_tempmx+16*3], xmm3  
    movq            [src_tempmx+16*4], xmm4   
    movq            [src_tempmx+16*5], xmm5   
    movq            [src_tempmx+16*6], xmm6    
    movq            [src_tempmx+16*7], xmm7  
    pxor            xmm0, xmm0
                
%endmacro



;/************************************************************************
; MACRO:            load8X8_BW
; function:      prepare input for horizontal. input: frame buffer
; args:            
; regs:
; steps:               
;************************************************************************/      
        
%macro load8X8_BW     4 ; args: ;src, stride, src+3*stride, 3*stide

    movq            xmm0, [%1]            ;load 0 row  
    movq            xmm1, [%1+%2]
    movq            xmm2, [%1+2*%2]
    movq            xmm3, [%3]
    movq            xmm4, [%3+%2]
    movq            xmm5, [%3+2*%2]
    movq            xmm6, [%3+%4] 
  
    pxor            xmm7, xmm7
    punpcklbw       xmm0, xmm0, xmm7 
    movdqa          [src_tempmx+16*0], xmm0 ;save 1 row   
    punpcklbw       xmm1, xmm1, xmm7 
    punpcklbw       xmm2, xmm2, xmm7 
    punpcklbw       xmm3, xmm3, xmm7 
    punpcklbw       xmm4, xmm4, xmm7 
    punpcklbw       xmm5, xmm5, xmm7 
    punpcklbw       xmm6, xmm6, xmm7 
    pxor            xmm0, xmm0
    movq            xmm7, [%3+4*%2] ;load 7 row  
    punpcklbw       xmm7, xmm7, xmm0
    
    movdqa          [src_tempmx+16*1], xmm1 ;save 1 row   
    movdqa          [src_tempmx+16*2], xmm2 
    movdqa          [src_tempmx+16*3], xmm3  
    movdqa          [src_tempmx+16*4], xmm4   
    movdqa          [src_tempmx+16*5], xmm5   
    movdqa          [src_tempmx+16*6], xmm6    
    movdqa          [src_tempmx+16*7], xmm7  
                
%endmacro



;/************************************************************************
; MACRO:            transpose_load8X8_WW
; function:      prepare input for horizontal. input: result of vertical filter
; args:            
; regs:
; steps:               
;************************************************************************/      
        
%macro transpose_load8X8_WW     4 ; args: ;src, stride, src+3*stride, 3*stide

    movdqa          xmm7, [%1]
    movdqa          xmm3, [%1+%2]
    movdqa          xmm5, [%1+2*%2]
    movdqa          xmm1, [%3]
    movdqa          xmm4, [%3+%2]
    movdqa          xmm2, [%3+2*%2]
    movdqa          xmm6, [%3+%4]
    movdqa          xmm0, [%3+4*%2]
    
    packuswb        xmm7, xmm7, xmm7
    packuswb        xmm3, xmm3, xmm3
    packuswb        xmm5, xmm5, xmm5
    packuswb        xmm1, xmm1, xmm1
    packuswb        xmm4, xmm4, xmm4
    packuswb        xmm2, xmm2, xmm2
    packuswb        xmm6, xmm6, xmm6
    packuswb        xmm0, xmm0, xmm0

    punpcklbw       xmm7, xmm7, xmm3
    punpcklbw       xmm5, xmm5, xmm1
    punpcklbw       xmm4, xmm4, xmm2
    punpcklbw       xmm6, xmm6, xmm0

    movq            xmm3, xmm7
    movq            xmm1, xmm5
    movq            xmm2, xmm4
    movq            xmm0, xmm6
    punpckhwd       xmm7, xmm7, xmm5
    punpckhwd       xmm4, xmm4, xmm6
    punpcklwd       xmm3, xmm3, xmm1
    punpcklwd       xmm2, xmm2, xmm0
   
    movq            xmm1, xmm3
    movq            xmm5, xmm7

    punpckldq       xmm1, xmm2  ;0, 1
    punpckhdq       xmm3, xmm2  ;2, 3
    
    punpckldq       xmm5, xmm4  ;4, 5 
    punpckhdq       xmm7, xmm4  ;6 ,7

    pxor            xmm0, xmm0

    movq            xmm2, xmm3
    movq            xmm4, xmm5
    movq            xmm6, xmm7

    punpckhbw       xmm7, xmm7, xmm0 ;7 row
    punpcklbw       xmm2, xmm2, xmm0 ;2 row
    punpckhbw       xmm3, xmm3, xmm0 ;3 row
    punpcklbw       xmm4, xmm4, xmm0 ;4 row
    punpckhbw       xmm5, xmm5, xmm0 ;5 row
    punpcklbw       xmm6, xmm6, xmm0 ;6 row
    
    movdqa          [src_tempmx+16*7], xmm7 ;save 7 row
    movq            xmm7, xmm1      ;reuse xmm7
   
    punpcklbw       xmm7, xmm7, xmm0 ;0 row
    punpckhbw       xmm1, xmm1, xmm0 ;1 row
    movdqa          [src_tempmx+16*0], xmm7 ;save 0 row
    
    movdqa          [src_tempmx+16*1], xmm1 ;save 1 row   
    movdqa          [src_tempmx+16*2], xmm2 ;save 2 row
    movdqa          [src_tempmx+16*3], xmm3 ;save 3 row
    movdqa          [src_tempmx+16*4], xmm4 ;save 4 row   
    movdqa          [src_tempmx+16*5], xmm5 ;save 5 row
    movdqa          [src_tempmx+16*6], xmm6 ;save 6 row  
                
%endmacro


;/************************************************************************
; MACRO:            transpose_save8X8_WB
; function:      save Ver filter output to frame buffer
; args:            
; regs:
; steps:               
;************************************************************************/      
        
%macro transpose_save8X8_WB     4 ; args: ;src, stride, src+3*stride, 3*stide

    movdqa          xmm7, [src_tempmx+16*0]  
    movdqa          xmm3, [src_tempmx+16*1] 
    movdqa          xmm5, [src_tempmx+16*2]
    movdqa          xmm1, [src_tempmx+16*3]
    movdqa          xmm4, [src_tempmx+16*4] 
    movdqa          xmm2, [src_tempmx+16*5] 
    movdqa          xmm6, [src_tempmx+16*6]
    movdqa          xmm0, [src_tempmx+16*7]

    packuswb        xmm7, xmm7, xmm7
    packuswb        xmm3, xmm3, xmm3
    packuswb        xmm5, xmm5, xmm5
    packuswb        xmm1, xmm1, xmm1
    packuswb        xmm4, xmm4, xmm4
    packuswb        xmm2, xmm2, xmm2
    packuswb        xmm6, xmm6, xmm6
    packuswb        xmm0, xmm0, xmm0

    punpcklbw       xmm7, xmm7, xmm3
    punpcklbw       xmm5, xmm5, xmm1
    punpcklbw       xmm4, xmm4, xmm2
    punpcklbw       xmm6, xmm6, xmm0

    movq            xmm3, xmm7
    movq            xmm1, xmm5
    movq            xmm2, xmm4
    movq            xmm0, xmm6
    punpckhwd       xmm7, xmm7, xmm5
    punpckhwd       xmm4, xmm4, xmm6
    punpcklwd       xmm3, xmm3, xmm1
    punpcklwd       xmm2, xmm2, xmm0

    movq            xmm1, xmm3
    movq            xmm5, xmm7

    punpckldq       xmm1, xmm1, xmm2  ;0, 1
    punpckhdq       xmm3, xmm3, xmm2  ;2, 3

    punpckldq       xmm5, xmm5, xmm4  ;4, 5 
    punpckhdq       xmm7, xmm7, xmm4  ;6 ,7

    movhlps         xmm0, xmm0, xmm1
    movhlps         xmm2, xmm2, xmm3
    movhlps         xmm4, xmm4, xmm5
    movhlps         xmm6, xmm6, xmm7
    movq            [%1], xmm1 ;save 0 row       
    movq            [%1+%2], xmm0 ;save 1 row       
    movq            [%1+2*%2], xmm3 ;save 2 row 
    movq            [%3], xmm2 ;save 3 row 
    movq            [%3+%2], xmm5 ;save 4 row   
    movq            [%3+2*%2], xmm4 ;save 5 row
    movq            [%3+%4], xmm7 ;save 6 row  
    movq            [%3+4*%2], xmm6 ;save 0 row        
                
%endmacro

;/************************************************************************
; MACRO:          save8X4_WB
; function:       save Hor filter output to frame buffer
; args:            
; regs:
; steps:               
;************************************************************************/      
        
%macro save8X4_WB     4 ; args: ;src, stride, src+3*stride, 3*stide

    movq            xmm7, [src_tempmx+16*0]  
    movq            xmm3, [src_tempmx+16*1] 
    movq            xmm5, [src_tempmx+16*2]
    movq            xmm1, [src_tempmx+16*3]
    movq            xmm4, [src_tempmx+16*4] 
    movq            xmm2, [src_tempmx+16*5] 
    movq            xmm6, [src_tempmx+16*6]
    movq            xmm0, [src_tempmx+16*7]

    packuswb        xmm7, xmm7, xmm7
    packuswb        xmm3, xmm3, xmm3
    packuswb        xmm5, xmm5, xmm5
    packuswb        xmm1, xmm1, xmm1
    packuswb        xmm4, xmm4, xmm4
    packuswb        xmm2, xmm2, xmm2
    packuswb        xmm6, xmm6, xmm6
    packuswb        xmm0, xmm0, xmm0

    movd            [%1], xmm7 ;save 0 row       
    movd            [%1+%2], xmm3 ;save 1 row       
    movd            [%1+2*%2], xmm5 ;save 2 row 
    movd            [%3], xmm1 ;save 3 row 
    movd            [%3+%2], xmm4 ;save 4 row   
    movd            [%3+2*%2], xmm2 ;save 5 row
    movd            [%3+%4], xmm6 ;save 6 row  
    movd            [%3+4*%2], xmm0 ;save 0 row        
                
%endmacro

;/************************************************************************
; MACRO:          save8X4_WB
; function:       save Ver filter output to frame buffer
; args:            
; regs:
; steps:               
;************************************************************************/      
        
%macro transpose_save8X4_WB     4 ; args: ;src, stride, src+3*stride, 3*stide

    movq            xmm7, [src_tempmx+16*0]  
    movq            xmm3, [src_tempmx+16*1] 
    movq            xmm5, [src_tempmx+16*2]
    movq            xmm1, [src_tempmx+16*3]
    movq            xmm4, [src_tempmx+16*4] 
    movq            xmm2, [src_tempmx+16*5] 
    movq            xmm6, [src_tempmx+16*6]
    movq            xmm0, [src_tempmx+16*7]
    
    packuswb        xmm7, xmm7, xmm7
    packuswb        xmm3, xmm3, xmm3
    packuswb        xmm5, xmm5, xmm5
    packuswb        xmm1, xmm1, xmm1
    packuswb        xmm4, xmm4, xmm4
    packuswb        xmm2, xmm2, xmm2
    packuswb        xmm6, xmm6, xmm6
    packuswb        xmm0, xmm0, xmm0

    punpcklbw       xmm7, xmm7, xmm3
    punpcklbw       xmm5, xmm5, xmm1
    punpcklbw       xmm4, xmm4, xmm2
    punpcklbw       xmm6, xmm6, xmm0
 
    punpcklwd       xmm7, xmm7, xmm5
    punpcklwd       xmm4, xmm4, xmm6
   
    movq            xmm1, xmm7
    movq            xmm5, xmm4

    punpckldq       xmm1, xmm1, xmm5  ;0, 1
    punpckhdq       xmm7, xmm7, xmm4  ;2, 3   
   
    movhlps         xmm0, xmm0, xmm1
    movhlps         xmm6, xmm6, xmm7
  
    movq            [%1], xmm1 ;save 0 row       
    movq            [%1+%2], xmm0 ;save 1 row       
    movq            [%1+2*%2], xmm7 ;save 2 row 
    movq            [%3], xmm6 ;save 3 row 
                
%endmacro

;/************************************************************************
; MACRO:          save8X8_WB
; function:       save cross filter output to frame buffer
; args:            
; regs:
; steps:               
;************************************************************************/      
        
%macro save8X8_WB     4 ; args: ;src, stride, src+3*stride, 3*stide

    movdqa          xmm7, [src_tempmx+16*0]  
    movdqa          xmm3, [src_tempmx+16*1] 
    movdqa          xmm5, [src_tempmx+16*2]
    movdqa          xmm1, [src_tempmx+16*3]
    movdqa          xmm4, [src_tempmx+16*4] 
    movdqa          xmm2, [src_tempmx+16*5] 
    movdqa          xmm6, [src_tempmx+16*6]
    movdqa          xmm0, [src_tempmx+16*7]
    
    packuswb        xmm7, xmm7, xmm7
    packuswb        xmm3, xmm3, xmm3
    packuswb        xmm5, xmm5, xmm5
    packuswb        xmm1, xmm1, xmm1
    packuswb        xmm4, xmm4, xmm4
    packuswb        xmm2, xmm2, xmm2
    packuswb        xmm6, xmm6, xmm6
    packuswb        xmm0, xmm0, xmm0
  
    movq            [%1], xmm7 ;save 0 row       
    movq            [%1+%2], xmm3 ;save 1 row       
    movq            [%1+2*%2], xmm5 ;save 2 row 
    movq            [%3], xmm1 ;save 3 row 
    movq            [%3+%2], xmm4 ;save 4 row   
    movq            [%3+2*%2], xmm2 ;save 5 row
    movq            [%3+%4], xmm6 ;save 6 row  
    movq            [%3+4*%2], xmm0 ;save 0 row 
                
%endmacro


;/************************************************************************
; MACRO:           dupWord0347
; function:      word type, dump word0 -> word1, word3 -> word2, word4 -> word5, word7 -> word6
; args:            
; regs:
; steps:               
;************************************************************************/      
        
%macro dupWord0347     1 ; args: ;src

    pshufhw         %1, %1, 0xf0 ;0b11110000;          ;1v3, 1v3, 1v0, 1v0,0v3, x, x, 0v0
    pshuflw         %1, %1, 0xf0 ;0b11110000;          ;1v3, 1v3, 1v0, 1v0,0v3, 0v3, 0v0, 0v0

%endmacro

;/************************************************************************
; MACRO:           swapWord0347
; function:      word type, dump word0 -> word3, word3 -> word0, word4 -> word7, word7 -> word4
; args:            
; regs:
; steps:               
;************************************************************************/      
        
%macro swapWord0347     2 ; args: ;dstReg, srcReg

    pshufhw         %1, %2, 0x27 ;
    pshuflw         %1, %1, 0x27 ;

%endmacro


;/************************************************************************
; MACRO:           maskedCopy
; function:      
; args:            
; regs:
; steps:               
;************************************************************************/      
        
%macro maskedCopy    3  ; args: org, src&dst, mask

    pand            %2, %2, %3
    pandn           %3, %3, %1
    pxor            %2, %2, %3

%endmacro

;/************************************************************************
; MACRO:           luma_deblock_core
; function:      
; args:            
; regs:
; steps:        output: r1=1, filter, r1=0, no filter      
;************************************************************************/      
        
%macro luma_deblock_core    2 ; args: LumaCross_bypasswrite

    ;VO_S32 dp0 = ABS( src[-3*stride] - 2*src[-2*stride] + src[-stride] );
	;VO_S32 dq0 = ABS( src[0] - 2*src[stride] + src[2*stride] );
	;VO_S32 dp3 = ABS( src[3-3*stride] - 2*src[3-2*stride] + src[3-stride] );
	;VO_S32 dq3 = ABS( src[3] - 2*src[3+stride] + src[3+2*stride] );   
    psllw           xmm2, 1  ; *2
    psubw           xmm1, xmm1, xmm2
    paddw           xmm1, xmm1, xmm3
    pabsw           xmm1, xmm1            ; 1dp3, x, x, 1dp0, 0dp3 ,x, x, 0dp0
    movdqa          xmm2, xmm1            ; @using xmm2 as dp0, dp3 , for weak

    psllw           xmm5, 1  ; *2
    psubw           xmm4, xmm4, xmm5
    paddw           xmm4, xmm4, xmm6
    pabsw           xmm4, xmm4             ; 1dq3, x, x, 1dq0, 0dq3 ,x, x, 0dq0   , @using xmm4 as dq0, dq3 , for weak

    ;VO_S32 d0 = dp0 + dq0;
	;VO_S32 d3 = dp3 + dq3;
    ;cal d0 , d3
    paddw           xmm1, xmm1, xmm4      ; 1d3, x, x, 1d0, 0d3,x, x, 0d0

     ;cal d0+d3
    pshufhw         xmm7, xmm1, 0x0f ;  1d0, 1d0, 1d3, 1d3, 0d0,x, x, 0d3
    pshuflw         xmm7, xmm7, 0x0f ;  1d0, 1d0, 1d3, 1d3, 0d0,0d0, 0d3, 0d3

    pshufhw         xmm1, xmm1, 0xf0 ;  1d3, 1d3, 1d0, 1d0, 0d3,x, x, 0d0
    pshuflw         xmm1, xmm1, 0xf0 ;  1d3, 1d3, 1d0, 1d0, 0d3,0d3, 0d0, 0d0 ,  @using xmm1 as d0, d3, for weak/string decision
    paddw           xmm7, xmm7, xmm1  ;  1d3+1d0, 1d3+1d0, 1d0+1d3, 1d0+1d3, 0d3+0d0,0d3+0d0, 0d0+0d3, 0d0+0d3 

     ;cal beta   
    movd            xmm5, iBetad
    punpcklbw       xmm5, xmm5, xmm0
    punpcklwd       xmm5, xmm5, xmm0
    punpckldq       xmm5, xmm5, xmm0 ;  0, 0, 0, 1iBeta, 0, 0, 0, 0iBeta
    pshufhw         xmm5, xmm5, 0x00    ;  0, 0, 0, 1iBeta, 0, 0iBeta, 0iBeta, 0iBeta
    pshuflw         xmm5, xmm5, 0x00    ;  1iBeta, 1iBeta, 1iBeta, 1iBeta, 0iBeta, 0iBeta, 0iBeta, 0iBeta
    movdqa          xmm6, xmm5          ; @using xmm6 as iBeta , for weak/string decision and weak 
     ;if (d0 + d3 < iBeta) 
    pcmpgtw         xmm5, xmm5, xmm7
    movmskps        r1, xmm5            ; r1: 1filtermask, 1filtermask, 0filtermask, 0filtermask
    and             r1, 1
    cmp             r1, 0
    je              %1

    and             r1, 1   ; filter mask, bits 0     

    ;compare iBeta>>2
    ;(d0 << 1) < (iBeta>>2) && 
    ;(d3 << 1) < (iBeta>>2))
    psllw           xmm1, xmm1, 1  ; (1d3, 1d3, 1d0, 1d0, 0d3,0d3, 0d0, 0d0) * 2
    movdqa          xmm5, xmm6     ; restore Beta
    psrlw           xmm5, xmm5, 2  ; (1iBeta, 1iBeta, 1iBeta, 1iBeta, 0iBeta, 0iBeta, 0iBeta, 0iBeta) >> 2    ?
    pcmpgtw         xmm5, xmm5, xmm1   ;@release xmm1 as d0, d3, for weak/string decision
    movmskps        r4, xmm5 
  
    
    ;compare tc_s
    ;cal tc_s
    ;VO_S32 tc_s = (iTc*5+1)>>1;
     movdqa         xmm1, [src_tempmx+16*4]  ;load 4 row
     movd           xmm5, iTcd
     punpcklbw      xmm5, xmm5, xmm0
     punpcklwd      xmm5, xmm5, xmm0
     punpckldq      xmm5, xmm5, xmm0 ;  xmm5: 0, 0, 0, 1iTc, 0, 0, 0, 0iTc  ;@release xmm0 as const 0
     pshufhw        xmm5, xmm5, 0x00    ;  0, 0, 0, 1iTc, 0, 0iTc, 0iTc, 0iTc
     pshuflw        xmm5, xmm5, 0x00    ;  1iTc, 1iTc, 1iTc, 1iTc, 0iTc, 0iTc, 0iTc, 0iTc
     movdqa         xmm7, xmm5           ; @using xmm7 as iTc , for weak and strong
     psllw          xmm5, xmm5, 2
     pavgw          xmm5, xmm5, xmm7
    ;ABS(src[-stride] -  src[0]) < tc_s &&
    ;ABS(src[3-stride] - src[3]) < tc_s &&
    movdqa          xmm0, xmm1     ; xmm1 will be used soon
    psubw           xmm0, xmm0, xmm3               
    pabsw           xmm0, xmm0
    pshufhw         xmm0, xmm0, 0xf0 ;0b11110000;          ;1v3, 1v3, 1v0, 1v0,0v3, x, x, 0v0
    pshuflw         xmm0, xmm0, 0xf0 ;0b11110000;          ;1v3, 1v3, 1v0, 1v0,0v3, 0v3, 0v0, 0v0
    pcmpgtw         xmm5, xmm5, xmm0
    movmskps        r5, xmm5
    and             r4, r5                        ;r4: 0000 0000 0000 1flag1,1flag0,0flag1,0flag0,

    ;compare iBeta>>3
    ;ABS(src[-4*stride] -  src[-stride]) + ABS( src[3*stride] -  src[0]) < (iBeta>>3) && 		   
    ;ABS(src[3-4*stride] - src[3-stride]) + ABS(src[3+3*stride] - src[3]) < (iBeta>>3) && 
    movdqa          xmm0, [src_tempmx+16*0]  ;load 0 row  
    psubw           xmm0, xmm0, xmm3                      
    pabsw           xmm0, xmm0

    psubw           xmm1, xmm1, [src_tempmx+16*7]                      
    pabsw           xmm1, xmm1
    paddw           xmm0, xmm0, xmm1                      ;1v3, x, x, 1v0,0v3, x, x, 0v0

    pshufhw         xmm0, xmm0, 0xf0 ;0b11110000;          ;1v3, 1v3, 1v0, 1v0,0v3, x, x, 0v0
    pshuflw         xmm0, xmm0, 0xf0 ;0b11110000;          ;1v3, 1v3, 1v0, 1v0,0v3, 0v3, 0v0, 0v0
      
    movdqa          xmm5, xmm6     ; restore Beta
    psrlw           xmm5, xmm5, 3  ; (1iBeta, 1iBeta, 1iBeta, 1iBeta, 0iBeta, 0iBeta, 0iBeta, 0iBeta) >> 3    ?
    pcmpgtw         xmm5, xmm5, xmm0
    movmskps        r5, xmm5
    and             r4, r5 
   
    
    mov             r5, r4;
    shr             r5, 1;
    and             r4, r5               
    and             r4, 1                ; strong mask, bits 0.  r4:  0000 0000 0000 x,x,x,0flag     

    ;check and do weak filter firstly   
    cmp             r4, 0                ;r4: strong mask
    jne             %%luma_deblock_core_do_strong_filter
    pcmpeqd         xmm0, xmm0      ; set all bits to 1
    pxor            xmm5, xmm5      ;set mask of high bits as 0  
    shufps          xmm0, xmm5, 0
    movdqa          [final_maskmx+0], xmm0    
  
    swapWord0347    xmm0, xmm2
    swapWord0347    xmm5, xmm4
    paddw           xmm0, xmm0, xmm2    ;@release xmm2 as dp0, dp3 , for weak 
    paddw           xmm4, xmm4, xmm5    ;@release xmm4 as dp0, dp3 , for weak
    dupWord0347     xmm0               ;xmm0: 1(dp0+dp3), 1(dp0+dp3), 1(dp0+dp3), 1(dp0+dp3), 0(dp0+dp3), 0(dp0+dp3), 0(dp0+dp3), 0(dp0+dp3)    
    dupWord0347     xmm4               ;xmm4: 1(dq0+dq3), 1(dq0+dq3), 1(dq0+dq3), 1(dq0+dq3), 0(dq0+dq3), 0(dq0+dq3), 0(dq0+dq3), 0(dq0+dq3)
    
    ;((iBeta+(iBeta>>1))>>3)
    movdqa          xmm5, xmm6     ; restore Beta
    psrlw           xmm5, xmm5, 1  ; (1iBeta, 1iBeta, 1iBeta, 1iBeta, 0iBeta, 0iBeta, 0iBeta, 0iBeta) >> 1   
    paddw           xmm5, xmm5, xmm6  ; @release xmm6 as iBeta , for weak/string decision and weak 
    psrlw           xmm5, xmm5, 3      ;@using xmm5:  (1iBeta, 1iBeta, 1iBeta, 1iBeta, 0iBeta, 0iBeta, 0iBeta, 0iBeta)
    
    ;VO_S32 p2 = src[-3*stride];VO_S32 p1 = src[-2*stride];VO_S32 p0 = src[-stride];
    ;VO_S32 q0 = src[0]; VO_S32 q1 = src[stride]; VO_S32 q2 = src[2*stride];
    ;VO_S32 delta = (9*(q0 - p0) - 3*(q1 - p1) + 8) >> 4;   
    movdqa          xmm1, [src_tempmx+16*4]   ; xmm1: q0 
    movdqa          xmm6, [src_tempmx+16*5]   ; xmm6: q1
    psubw           xmm1, xmm1, xmm3
    movdqa          xmm3, [src_tempmx+16*2]   ; xmm3: p1
    movdqa          xmm2, xmm1               ; xmm2: (q0 - p0)
    psllw           xmm1, xmm1, 3            ; xmm1: 8*(q0 - p0)
    paddw           xmm1, xmm1, xmm2         ; xmm1: 9*(q0 - p0)    
  
    psubw           xmm6, xmm6, xmm3         ; xmm6: (q1 - p1)
    movdqa          xmm2, xmm6               ; xmm2: (q0 - p0)
    psllw           xmm6, xmm6, 1            ; xmm6: 2*(q0 - p0)
    paddw           xmm6, xmm6, xmm2         ; xmm6: 3*(q0 - p0)  
    
    psubw           xmm1, xmm1, xmm6         ; xmm1: 9*(q0 - p0) - 3*(q1 - p1)
    paddw           xmm1, xmm1, [CONST_U16_POS8]  ; xmm1: 9*(q0 - p0) - 3*(q1 - p1) + 8
    psraw           xmm1, xmm1, 4                 ; @using xmm1: delta
    movdqa          xmm3, xmm1    
    pabsw           xmm3, xmm3                    ; @using xmm3: abs(delta)

    ;if (ABS(delta) < 10 * iTc) 
    movdqa          xmm2, xmm7
    movdqa          xmm6, xmm7
    psllw           xmm2, xmm2, 3            ; xmm2: 8*iTc
    psllw           xmm6, xmm6, 1            ; xmm6: 2*iTc
    paddw           xmm2, xmm2, xmm6         ; xmm2: 10*iTc
    pxor            xmm6, xmm6    
    pcmpgtw         xmm2, xmm2, xmm3         ;@using xmm2: mask of (ABS(delta) < 10 * iTc)   @release xmm3: abs(delta)
    pand            xmm2, xmm2, [final_maskmx+0]
    movdqa          [final_maskmx+0], xmm2 
    
    ;delta = Clip3( -iTc, iTc,delta);
    psubw           xmm6, xmm6, xmm7               ;xmm6: -iTc
    pmaxsw          xmm1, xmm1, xmm6
    pminsw          xmm1, xmm1, xmm7              ;@using xmm1: delta    
    
    ; if(nd_p > 1) 
    ;{
    ;   VO_S32 delta_p = Clip3( -tc_2, tc_2,(((p2 + p0 + 1) >> 1) - p1 + delta) >> 1);
    ;   src[-2*stride] = Clip(p1 + delta_p);
    ;}  
    movdqa          xmm2, [src_tempmx+16*2]   ; xmm2: p1 
    pxor            xmm6, xmm6 
    psraw           xmm7, xmm7, 1
    psubw           xmm6, xmm6, xmm7               ;xmm6: -tc_2
    movdqa          xmm3, [src_tempmx+16*3]   ; xmm3: p0 
    pavgw           xmm3, xmm3, [src_tempmx+16*1]
    psubw           xmm3, xmm3, xmm2   ; - p1
    paddw           xmm3, xmm3, xmm1                ; + delta
    psraw           xmm3, xmm3, 1
    pmaxsw          xmm3, xmm3, xmm6
    movdqa          xmm6, xmm5                    ; xmm5, for q cal
    pminsw          xmm3, xmm3, xmm7              ;@using xmm3: delta_p    
    paddw           xmm3, xmm3, xmm2              ; p1 + delta_p    
   
    pcmpgtw         xmm6, xmm6, xmm0             ;@mask of (dp0 + dp3 < ((iBeta+(iBeta>>1))>>3))  @release xmm0 as dp0 + dp3  
    pand            xmm6, xmm6, [final_maskmx+0]  ; mask of (ABS(delta) < 10 * iTc)  
    maskedCopy      xmm2, xmm3, xmm6              ;output is xmm3
    
    ;if(nd_q > 1) 
    ;{
    ;   VO_S32 delta_q = Clip3( -tc_2, tc_2,(((q2 + q0 + 1) >> 1) - q1 - delta) >> 1);
    ;   src[stride] = Clip(q1 + delta_q);
    ;}
    movdqa          xmm0, [src_tempmx+16*5]   ; xmm2: q1 
    pxor            xmm6, xmm6  
    psubw           xmm6, xmm6, xmm7               ;xmm6: -tc_2
    movdqa          xmm2, [src_tempmx+16*4]   ; xmm2: q0 
    pavgw           xmm2, xmm2, [src_tempmx+16*6]
    psubw           xmm2, xmm2, xmm0   ; - q1
    psubw           xmm2, xmm2, xmm1                ; - delta
    psraw           xmm2, xmm2, 1
    pmaxsw          xmm2, xmm2, xmm6 
    pminsw          xmm2, xmm2, xmm7              ;@using xmm2: delta_q  
   
    paddw           xmm2, xmm2, xmm0              ; q1 + delta_q  
    movdqa          xmm6, [src_tempmx+16*3]       ; xmm6: pre-fetch p0
    movdqa          xmm7, [src_tempmx+16*4]       ; xmm7: pre-fetch q0   
    pcmpgtw         xmm5, xmm5, xmm4              ; @mask of (dq0 + dq3 < ((iBeta+(iBeta>>1))>>3))  @release xmm4 as dq0 + dq3  
    movdqa          xmm4, [final_maskmx+0]   ; xmm4: mask
    pand            xmm5, xmm5, xmm4   ; mask of (ABS(delta) < 10 * iTc)  
    maskedCopy      xmm0, xmm2, xmm5              ; output is xmm2    
    
    ;src[-stride] = Clip(p0 + delta);
    ;src[0] = Clip(q0 - delta);  
    movdqa          xmm0, xmm6
    movdqa          xmm5, xmm7
    paddw           xmm6, xmm6, xmm1          ; p0 + delta
    psubw           xmm7, xmm7, xmm1          ; q0 - delta
    movdqa          xmm1, xmm4
    maskedCopy      xmm0, xmm6, xmm4       ;output is xmm6
    maskedCopy      xmm5, xmm7, xmm1       ;output is xmm7
    
    ;save weak filter result 
    movdqa          [src_tempmx+16*2], xmm3       
    movdqa          [src_tempmx+16*3], xmm6       
    movdqa          [src_tempmx+16*4], xmm7       
    movdqa          [src_tempmx+16*5], xmm2
    jmp             %%luma_deblock_core_done       
    
    ;strong filter
%%luma_deblock_core_do_strong_filter: 
    pcmpeqd         xmm0, xmm0      ; set all bits to 1
    pxor            xmm5, xmm5      ;set mask of high bits as 0  
    shufps          xmm0, xmm5, 0
    movdqa          [final_maskmx+0], xmm0  
    
    ;VO_S32 tc2 = iTc << 1;
    psllw           xmm7, xmm7, 1               ;@using xmm7
    
    ;src[-3*stride] = Clip3( p2-tc2, p2+tc2,( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3);
    movdqa          xmm0, [src_tempmx+16*0]   ; xmm0: p3
    movdqa          xmm1, [src_tempmx+16*1]   ; xmm1: p2   ;@using xmm1
    movdqa          xmm2, [src_tempmx+16*2]   ; xmm2: p1   ;@using xmm2
    movdqa          xmm3, [src_tempmx+16*3]   ; xmm3: p0
    movdqa          xmm4, [src_tempmx+16*4]   ; xmm4: q0
    paddw           xmm4, xmm4, xmm3          ; q0 + p0
    paddw           xmm4, xmm4, xmm2          ; q0 + p0 + p1  ;@using xmm4
    
    psllw           xmm0, xmm0, 1
    movdqa          xmm3, xmm1                ; for  3*p2
    psllw           xmm3, xmm3, 1
    paddw           xmm3, xmm3, xmm1          ; 3*p2
    paddw           xmm0, xmm0, xmm3          ; 2*p3 + 3*p2
   
    paddw           xmm0, xmm0, xmm4          ; 2*p3 + 3*p2 + p1 + p0 + q0
    paddw           xmm0, xmm0, [CONST_S16_POS4]          ; 2*p3 + 3*p2 + p1 + p0 + q0 + 4
    psraw           xmm0, xmm0, 3
    
    movdqa          xmm5, xmm1                ; for p2-tc2
    movdqa          xmm6, xmm1                ; for p2+tc2   
    psubw           xmm6, xmm6, xmm7          ; p2-tc2
    paddw           xmm5, xmm5, xmm7          ; p2+tc2
    
    pmaxsw          xmm0, xmm0, xmm6 
    movdqa          xmm6, [final_maskmx+0]
    pminsw          xmm0, xmm0, xmm5              ;xmm0 as src[-3*stride]
    maskedCopy      xmm1, xmm0, xmm6              ; output is xmm0
    movdqa          [src_tempmx+16*1], xmm0      ; save src[-3*stride]    
    
    ;src[-2*stride] = Clip3( p1-tc2, p1+tc2,( p2 + p1 + p0 + q0 + 2 ) >> 2);
    movdqa          xmm3, xmm1                ; 
    paddw           xmm3, xmm3, xmm4          ; p2 + p1 + p0 + q0 
    paddw           xmm3, xmm3, [CONST_U16_2]          ; p2 + p1 + p0 + q0 + 2
    psraw           xmm3, xmm3, 2
    
    movdqa          xmm5, xmm2                ; for p1-tc2
    movdqa          xmm6, xmm2                ; for p1+tc2   
    psubw           xmm6, xmm6, xmm7          ; p1-tc2
    paddw           xmm5, xmm5, xmm7          ; p1+tc2
    movdqa          xmm0, [src_tempmx+16*5]   ; @using xmm0: q1 , pre-fetch q1    
    
    pmaxsw          xmm3, xmm3, xmm6 
    movdqa          xmm6, [final_maskmx+0]
    pminsw          xmm3, xmm3, xmm5              ;xmm3 as src[-2*stride]
    maskedCopy      xmm2, xmm3, xmm6              ; output is xmm3  
    movdqa          [src_tempmx+16*2], xmm3       ; save src[-2*stride]
    movdqa          xmm6, [src_tempmx+16*3]   ; @using xmm6: p0
    
    ;src[-stride] = Clip3( p0-tc2, p0+tc2,( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3); 
    psllw           xmm4, xmm4, 1             ;2*p1 + 2*p0 + 2*q0
    paddw           xmm1, xmm1, xmm4          ;p2 + 2*p1 + 2*p0 + 2*q0
    movdqa          xmm4, [src_tempmx+16*4]   ; xmm4: q0
    paddw           xmm1, xmm1, xmm0          ;p2 + 2*p1 + 2*p0 + 2*q0 + q1
    paddw           xmm1, xmm1, [CONST_S16_POS4]          ; p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4
    psraw           xmm1, xmm1, 3
    
    movdqa          xmm5, xmm6                ; for p0-tc2
    movdqa          xmm3, xmm6                ; for p0+tc2   
    psubw           xmm3, xmm3, xmm7          ; p0-tc2
    paddw           xmm5, xmm5, xmm7          ; p0+tc2
    
    pmaxsw          xmm1, xmm1, xmm3 
    movdqa          xmm3, [final_maskmx+0]
    pminsw          xmm1, xmm1, xmm5              ;@xmm1 as src[-stride]
    maskedCopy      xmm6, xmm1, xmm3              ; output is xmm1  
    movdqa          [src_tempmx+16*3], xmm1       ;save src[-*stride]
    
    ;src[0] = Clip3( q0-tc2, q0+tc2,( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3);
    movdqa          xmm5, xmm4                ; for q0-tc2
    movdqa          xmm3, xmm4                ; for q0+tc2   
    psubw           xmm3, xmm3, xmm7          ; q0-tc2
    paddw           xmm5, xmm5, xmm7          ; q0+tc2
    movdqa          xmm1, [src_tempmx+16*6]       ;load q2
    
    paddw           xmm6, xmm6, xmm4          ; p0 + q0
    paddw           xmm6, xmm6, xmm0          ; p0 + q0 + q1  ;@using xmm6
    psllw           xmm6, xmm6, 1             ; 2*p0 + 2*q0 + 2*q1
    paddw           xmm2, xmm2, xmm6          ; p1 + 2*p0 + 2*q0 + 2*q1
    paddw           xmm2, xmm2, xmm1          ; p1 + 2*p0 + 2*q0 + 2*q1 + q2
    paddw           xmm2, xmm2, [CONST_S16_POS4]          ; p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4
    psraw           xmm2, xmm2, 3
    
    pmaxsw          xmm2, xmm2, xmm3 
    movdqa          xmm3, [final_maskmx+0]
    pminsw          xmm2, xmm2, xmm5              ;@xmm1 as src[-stride]
    maskedCopy      xmm4, xmm2, xmm3              ; output is xmm2   
    movdqa          [src_tempmx+16*4], xmm2       ;save src[0]    
    
    ;src[stride] = Clip3( q1-tc2, q1+tc2,( p0 + q0 + q1 + q2 + 2 ) >> 2);
    movdqa          xmm5, xmm0                ; for q1-tc2
    movdqa          xmm3, xmm0                ; for q1+tc2   
 
    psubw           xmm3, xmm3, xmm7          ; q1-tc2
    paddw           xmm5, xmm5, xmm7          ; q1+tc2
    
    psrlw           xmm6, xmm6, 1             ; 2*p0 + 2*q0 + 2*q1
    movdqa          xmm2, xmm6                ;   p0 + q0 + q1
    paddw           xmm2, xmm2, xmm1          ;   p0 + q0 + q1 + q2
    paddw           xmm2, xmm2, [CONST_U16_2] ;   p0 + q0 + q1 + q2 + 2
    psraw           xmm2, xmm2, 2
    
    pmaxsw          xmm2, xmm2, xmm3 
    movdqa          xmm3, [final_maskmx+0]
    pminsw          xmm2, xmm2, xmm5              ;@xmm1 as src[-stride]
    maskedCopy      xmm0, xmm2, xmm3              ; output is xmm2 
    movdqa          xmm0, [src_tempmx+16*7]   ; xmm0, q3, pre-fetch
    movdqa          [src_tempmx+16*5], xmm2       ;save src[stride]    
    
    ;src[2*stride] = Clip3( q2-tc2, q2+tc2,( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3);    
    psllw           xmm0, xmm0, 1  
    movdqa          xmm3, xmm1                ; for  3*q2
    psllw           xmm3, xmm3, 1
    paddw           xmm3, xmm3, xmm1          ; 3*q2
    paddw           xmm0, xmm0, xmm3          ; 2*q3 + 3*q2
   
    paddw           xmm0, xmm0, xmm6          ; 2*q3 + 3*q2 + q1 + q0 + p0
    paddw           xmm0, xmm0, [CONST_S16_POS4]          ; 2*q3 + 3*q2 + q1 + q0 + p0 + 4
    psraw           xmm0, xmm0, 3
    
    movdqa          xmm5, xmm1                ; for q2-tc2
    movdqa          xmm6, xmm1                ; for q2+tc2   
    psubw           xmm6, xmm6, xmm7          ; q2-tc2
    paddw           xmm5, xmm5, xmm7          ; q2+tc2
    
    pmaxsw          xmm0, xmm0, xmm6 
    movdqa          xmm6, [final_maskmx+0]
    pminsw          xmm0, xmm0, xmm5              ;xmm0 as src[2*stride]
    
    maskedCopy      xmm1, xmm0, xmm6              ; output is xmm0 
    movdqa          [src_tempmx+16*6], xmm0       ;save src[2*stride]    

%%luma_deblock_core_done:

    
%endmacro

;/************************************************************************
; MACRO:           luma_deblock_cross_weak
; function:      
; args:            
; regs:
; steps:        output: r1=1, filter, r1=0, no filter      
;************************************************************************/
%macro luma_deblock_cross_weak    2    ; args: Hor/Ver, flag(all/mask)

    swapWord0347    xmm0, xmm2
    swapWord0347    xmm5, xmm4
    paddw           xmm0, xmm0, xmm2    ;@release xmm2 as dp0, dp3 , for weak 
    paddw           xmm4, xmm4, xmm5    ;@release xmm4 as dp0, dp3 , for weak
    dupWord0347     xmm0               ;xmm0: 1(dp0+dp3), 1(dp0+dp3), 1(dp0+dp3), 1(dp0+dp3), 0(dp0+dp3), 0(dp0+dp3), 0(dp0+dp3), 0(dp0+dp3)    
    dupWord0347     xmm4               ;xmm4: 1(dq0+dq3), 1(dq0+dq3), 1(dq0+dq3), 1(dq0+dq3), 0(dq0+dq3), 0(dq0+dq3), 0(dq0+dq3), 0(dq0+dq3)
    
    ;((iBeta+(iBeta>>1))>>3)
    movdqa          xmm5, xmm6     ; restore Beta
    psrlw           xmm5, xmm5, 1  ; (1iBeta, 1iBeta, 1iBeta, 1iBeta, 0iBeta, 0iBeta, 0iBeta, 0iBeta) >> 1   
    paddw           xmm5, xmm5, xmm6  ; @release xmm6 as iBeta , for weak/string decision and weak 
    psrlw           xmm5, xmm5, 3      ;@using xmm5:  (1iBeta, 1iBeta, 1iBeta, 1iBeta, 0iBeta, 0iBeta, 0iBeta, 0iBeta)
    
    ;VO_S32 p2 = src[-3*stride];VO_S32 p1 = src[-2*stride];VO_S32 p0 = src[-stride];
    ;VO_S32 q0 = src[0]; VO_S32 q1 = src[stride]; VO_S32 q2 = src[2*stride];
    ;VO_S32 delta = (9*(q0 - p0) - 3*(q1 - p1) + 8) >> 4;   
    movdqa          xmm1, [src_tempmx+16*4]   ; xmm1: q0 
    movdqa          xmm6, [src_tempmx+16*5]   ; xmm6: q1
    psubw           xmm1, xmm1, xmm3
    movdqa          xmm3, [src_tempmx+16*2]   ; xmm3: p1
    movdqa          xmm2, xmm1               ; xmm2: (q0 - p0)
    psllw           xmm1, xmm1, 3            ; xmm1: 8*(q0 - p0)
    paddw           xmm1, xmm1, xmm2         ; xmm1: 9*(q0 - p0)    
  
    psubw           xmm6, xmm6, xmm3         ; xmm6: (q1 - p1)
    movdqa          xmm2, xmm6               ; xmm2: (q0 - p0)
    psllw           xmm6, xmm6, 1            ; xmm6: 2*(q0 - p0)
    paddw           xmm6, xmm6, xmm2         ; xmm6: 3*(q0 - p0)  
    
    psubw           xmm1, xmm1, xmm6         ; xmm1: 9*(q0 - p0) - 3*(q1 - p1)
    paddw           xmm1, xmm1, [CONST_U16_POS8]  ; xmm1: 9*(q0 - p0) - 3*(q1 - p1) + 8
    psraw           xmm1, xmm1, 4                 ; @using xmm1: delta
    movdqa          xmm3, xmm1    
    pabsw           xmm3, xmm3                    ; @using xmm3: abs(delta)

    ;if (ABS(delta) < 10 * iTc) 
    movdqa          xmm2, xmm7
    movdqa          xmm6, xmm7
    psllw           xmm2, xmm2, 3            ; xmm2: 8*iTc
    psllw           xmm6, xmm6, 1            ; xmm6: 2*iTc
    paddw           xmm2, xmm2, xmm6         ; xmm2: 10*iTc
    pxor            xmm6, xmm6    
    pcmpgtw         xmm2, xmm2, xmm3         ;@using xmm2: mask of (ABS(delta) < 10 * iTc)   @release xmm3: abs(delta)
    pand            xmm2, xmm2, [final_maskmx+0]
    movdqa          [final_maskmx+0], xmm2 
    
    ;delta = Clip3( -iTc, iTc,delta);
    psubw           xmm6, xmm6, xmm7               ;xmm6: -iTc
    pmaxsw          xmm1, xmm1, xmm6
    pminsw          xmm1, xmm1, xmm7              ;@using xmm1: delta    
    
    ; if(nd_p > 1) 
    ;{
    ;   VO_S32 delta_p = Clip3( -tc_2, tc_2,(((p2 + p0 + 1) >> 1) - p1 + delta) >> 1);
    ;   src[-2*stride] = Clip(p1 + delta_p);
    ;}  
    movdqa          xmm2, [src_tempmx+16*2]   ; xmm2: p1 
    pxor            xmm6, xmm6 
    psraw           xmm7, xmm7, 1
    psubw           xmm6, xmm6, xmm7               ;xmm6: -tc_2
    movdqa          xmm3, [src_tempmx+16*3]   ; xmm3: p0 
    pavgw           xmm3, xmm3, [src_tempmx+16*1]
    psubw           xmm3, xmm3, xmm2   ; - p1
    paddw           xmm3, xmm3, xmm1                ; + delta
    psraw           xmm3, xmm3, 1
    pmaxsw          xmm3, xmm3, xmm6
    movdqa          xmm6, xmm5                    ; xmm5, for q cal
    pminsw          xmm3, xmm3, xmm7              ;@using xmm3: delta_p    
    paddw           xmm3, xmm3, xmm2              ; p1 + delta_p    
   
    pcmpgtw         xmm6, xmm6, xmm0             ;@mask of (dp0 + dp3 < ((iBeta+(iBeta>>1))>>3))  @release xmm0 as dp0 + dp3  
    pand            xmm6, xmm6, [final_maskmx+0]  ; mask of (ABS(delta) < 10 * iTc)  
    maskedCopy      xmm2, xmm3, xmm6              ;output is xmm3
    
    ;if(nd_q > 1) 
    ;{
    ;   VO_S32 delta_q = Clip3( -tc_2, tc_2,(((q2 + q0 + 1) >> 1) - q1 - delta) >> 1);
    ;   src[stride] = Clip(q1 + delta_q);
    ;}
    movdqa          xmm0, [src_tempmx+16*5]   ; xmm2: q1 
    pxor            xmm6, xmm6  
    psubw           xmm6, xmm6, xmm7               ;xmm6: -tc_2
    movdqa          xmm2, [src_tempmx+16*4]   ; xmm2: q0 
    pavgw           xmm2, xmm2, [src_tempmx+16*6]
    psubw           xmm2, xmm2, xmm0   ; - q1
    psubw           xmm2, xmm2, xmm1                ; - delta
    psraw           xmm2, xmm2, 1
    pmaxsw          xmm2, xmm2, xmm6 
    pminsw          xmm2, xmm2, xmm7              ;@using xmm2: delta_q  
   
    paddw           xmm2, xmm2, xmm0              ; q1 + delta_q  
    movdqa          xmm6, [src_tempmx+16*3]       ; xmm6: pre-fetch p0
    movdqa          xmm7, [src_tempmx+16*4]       ; xmm7: pre-fetch q0   
    pcmpgtw         xmm5, xmm5, xmm4              ; @mask of (dq0 + dq3 < ((iBeta+(iBeta>>1))>>3))  @release xmm4 as dq0 + dq3  
    movdqa          xmm4, [final_maskmx+0]   ; xmm4: mask
    pand            xmm5, xmm5, xmm4   ; mask of (ABS(delta) < 10 * iTc)  
    maskedCopy      xmm0, xmm2, xmm5              ; output is xmm2
    
    
    ;src[-stride] = Clip(p0 + delta);
    ;src[0] = Clip(q0 - delta);  
    movdqa          xmm0, xmm6
    movdqa          xmm5, xmm7
    paddw           xmm6, xmm6, xmm1          ; p0 + delta
    psubw           xmm7, xmm7, xmm1          ; q0 - delta
    movdqa          xmm1, xmm4
    maskedCopy      xmm0, xmm6, xmm4       ;output is xmm6
    maskedCopy      xmm5, xmm7, xmm1       ;output is xmm7

%endmacro

;/************************************************************************
; MACRO:           luma_deblock_cross_strong
; function:      
; args:            
; regs:
; steps:          if Hor filter, write result into frame buffer
;                 if all strong, no need to use mask   
;************************************************************************/
%macro luma_deblock_cross_strong    2    ; args: Hor/Ver, flag(all/mask)

    mov		        srcq, srcmp
    mov		        strideq, stridemp     
    cal_stride_mul3 srcq, strideq, r4 ,r5, true, true  
   
    ;VO_S32 tc2 = iTc << 1;
    psllw           xmm7, xmm7, 1               ;@using xmm7
    
    ;src[-3*stride] = Clip3( p2-tc2, p2+tc2,( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3);
    movdqa          xmm0, [src_tempmx+16*0]   ; xmm0: p3
    movdqa          xmm1, [src_tempmx+16*1]   ; xmm1: p2   ;@using xmm1
    movdqa          xmm2, [src_tempmx+16*2]   ; xmm2: p1   ;@using xmm2
    movdqa          xmm3, [src_tempmx+16*3]   ; xmm3: p0
    movdqa          xmm4, [src_tempmx+16*4]   ; xmm4: q0
    paddw           xmm4, xmm4, xmm3          ; q0 + p0
    paddw           xmm4, xmm4, xmm2          ; q0 + p0 + p1  ;@using xmm4
    
    psllw           xmm0, xmm0, 1
    movdqa          xmm3, xmm1                ; for  3*p2
    psllw           xmm3, xmm3, 1
    paddw           xmm3, xmm3, xmm1          ; 3*p2
    paddw           xmm0, xmm0, xmm3          ; 2*p3 + 3*p2
   
    paddw           xmm0, xmm0, xmm4          ; 2*p3 + 3*p2 + p1 + p0 + q0
    paddw           xmm0, xmm0, [CONST_S16_POS4]          ; 2*p3 + 3*p2 + p1 + p0 + q0 + 4
    psraw           xmm0, xmm0, 3
    
    movdqa          xmm5, xmm1                ; for p2-tc2
    movdqa          xmm6, xmm1                ; for p2+tc2   
    psubw           xmm6, xmm6, xmm7          ; p2-tc2
    paddw           xmm5, xmm5, xmm7          ; p2+tc2
    
    pmaxsw          xmm0, xmm0, xmm6 
 %ifidn %2, false
    movdqa          xmm6, [final_maskmx+0]
 %endif
    pminsw          xmm0, xmm0, xmm5              ;xmm0 as src[-3*stride]
 %ifidn %2, false
    maskedCopy      xmm1, xmm0, xmm6              ; output is xmm0
 %endif
 
 %ifidn %1, Hor
    packuswb        xmm0, xmm0, xmm0
    movq            [srcq+strideq], xmm0      ; save src[-3*stride]
 %else
    movdqa          [src_tempmx+16*1], xmm0      ; save src[-3*stride]
 %endif
    
    ;src[-2*stride] = Clip3( p1-tc2, p1+tc2,( p2 + p1 + p0 + q0 + 2 ) >> 2);
    movdqa          xmm3, xmm1                ; 
    paddw           xmm3, xmm3, xmm4          ; p2 + p1 + p0 + q0 
    paddw           xmm3, xmm3, [CONST_U16_2]          ; p2 + p1 + p0 + q0 + 2
    psraw           xmm3, xmm3, 2
    
    movdqa          xmm5, xmm2                ; for p1-tc2
    movdqa          xmm6, xmm2                ; for p1+tc2   
    psubw           xmm6, xmm6, xmm7          ; p1-tc2
    paddw           xmm5, xmm5, xmm7          ; p1+tc2
    movdqa          xmm0, [src_tempmx+16*5]   ; @using xmm0: q1 , pre-fetch q1
    
    pmaxsw          xmm3, xmm3, xmm6 
%ifidn %2, false
    movdqa          xmm6, [final_maskmx+0]
%endif
    pminsw          xmm3, xmm3, xmm5              ;xmm3 as src[-2*stride]
%ifidn %2, false
    maskedCopy      xmm2, xmm3, xmm6              ; output is xmm3 
%endif
%ifidn %1, Hor
    packuswb        xmm3, xmm3, xmm3
    movq            [srcq+2*strideq], xmm3        ; save src[-2*stride]
%else    
    movdqa          [src_tempmx+16*2], xmm3       ; save src[-2*stride]
%endif
    movdqa          xmm6, [src_tempmx+16*3]   ; @using xmm6: p0
    
    ;src[-stride] = Clip3( p0-tc2, p0+tc2,( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3); 
    psllw           xmm4, xmm4, 1             ;2*p1 + 2*p0 + 2*q0
    paddw           xmm1, xmm1, xmm4          ;p2 + 2*p1 + 2*p0 + 2*q0
    movdqa          xmm4, [src_tempmx+16*4]   ; xmm4: q0
    paddw           xmm1, xmm1, xmm0          ;p2 + 2*p1 + 2*p0 + 2*q0 + q1
    paddw           xmm1, xmm1, [CONST_S16_POS4]          ; p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4
    psraw           xmm1, xmm1, 3
    
    movdqa          xmm5, xmm6                ; for p0-tc2
    movdqa          xmm3, xmm6                ; for p0+tc2   
    psubw           xmm3, xmm3, xmm7          ; p0-tc2
    paddw           xmm5, xmm5, xmm7          ; p0+tc2
    
    pmaxsw          xmm1, xmm1, xmm3
%ifidn %2, false    
    movdqa          xmm3, [final_maskmx+0]
%endif
    pminsw          xmm1, xmm1, xmm5              ;@xmm1 as src[-stride]
%ifidn %2, false
    maskedCopy      xmm6, xmm1, xmm3              ; output is xmm1 
%endif
%ifidn %1, Hor
    packuswb        xmm1, xmm1, xmm1
    movq            [r4], xmm1        ; save src[-stride]
%else      
    movdqa          [src_tempmx+16*3], xmm1       ;save src[-stride]
%endif
    
    ;src[0] = Clip3( q0-tc2, q0+tc2,( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3);
    movdqa          xmm5, xmm4                ; for q0-tc2
    movdqa          xmm3, xmm4                ; for q0+tc2   
    psubw           xmm3, xmm3, xmm7          ; q0-tc2
    paddw           xmm5, xmm5, xmm7          ; q0+tc2
    movdqa          xmm1, [src_tempmx+16*6]       ;load q2
    
    paddw           xmm6, xmm6, xmm4          ; p0 + q0
    paddw           xmm6, xmm6, xmm0          ; p0 + q0 + q1  ;@using xmm6
    psllw           xmm6, xmm6, 1             ; 2*p0 + 2*q0 + 2*q1
    paddw           xmm2, xmm2, xmm6          ; p1 + 2*p0 + 2*q0 + 2*q1
    paddw           xmm2, xmm2, xmm1          ; p1 + 2*p0 + 2*q0 + 2*q1 + q2
    paddw           xmm2, xmm2, [CONST_S16_POS4]          ; p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4
    psraw           xmm2, xmm2, 3
    
    pmaxsw          xmm2, xmm2, xmm3
%ifidn %2, false    
    movdqa          xmm3, [final_maskmx+0]
%endif
    pminsw          xmm2, xmm2, xmm5              ;@xmm1 as src[-stride]
%ifidn %2, false
    maskedCopy      xmm4, xmm2, xmm3              ; output is xmm2
%endif
%ifidn %1, Hor
    packuswb        xmm2, xmm2, xmm2
    movq            [r4+strideq], xmm2        ; save src[-stride]
%else        
    movdqa          [src_tempmx+16*4], xmm2       ;save src[0]
%endif
    
    
    ;src[stride] = Clip3( q1-tc2, q1+tc2,( p0 + q0 + q1 + q2 + 2 ) >> 2);
    movdqa          xmm5, xmm0                ; for q1-tc2
    movdqa          xmm3, xmm0                ; for q1+tc2   
 
    psubw           xmm3, xmm3, xmm7          ; q1-tc2
    paddw           xmm5, xmm5, xmm7          ; q1+tc2
    
    psrlw           xmm6, xmm6, 1             ; 2*p0 + 2*q0 + 2*q1
    movdqa          xmm2, xmm6                ;   p0 + q0 + q1
    paddw           xmm2, xmm2, xmm1          ;   p0 + q0 + q1 + q2
    paddw           xmm2, xmm2, [CONST_U16_2] ;   p0 + q0 + q1 + q2 + 2
    psraw           xmm2, xmm2, 2
    
    pmaxsw          xmm2, xmm2, xmm3 
 %ifidn %2, false
    movdqa          xmm3, [final_maskmx+0]
 %endif
    pminsw          xmm2, xmm2, xmm5              ;@xmm1 as src[-stride]
 %ifidn %2, false
    maskedCopy      xmm0, xmm2, xmm3              ; output is xmm2 
%endif
    movdqa          xmm0, [src_tempmx+16*7]   ; xmm0, q3, pre-fetch
%ifidn %1, Hor
    packuswb        xmm2, xmm2, xmm2
    movq            [r4+2*strideq], xmm2        ; save src[-stride]
%else   
    movdqa          [src_tempmx+16*5], xmm2       ;save src[stride]
%endif
    
    
    ;src[2*stride] = Clip3( q2-tc2, q2+tc2,( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3);    
    psllw           xmm0, xmm0, 1  
    movdqa          xmm3, xmm1                ; for  3*q2
    psllw           xmm3, xmm3, 1
    paddw           xmm3, xmm3, xmm1          ; 3*q2
    paddw           xmm0, xmm0, xmm3          ; 2*q3 + 3*q2
   
    paddw           xmm0, xmm0, xmm6          ; 2*q3 + 3*q2 + q1 + q0 + p0
    paddw           xmm0, xmm0, [CONST_S16_POS4]          ; 2*q3 + 3*q2 + q1 + q0 + p0 + 4
    psraw           xmm0, xmm0, 3
    
    movdqa          xmm5, xmm1                ; for q2-tc2
    movdqa          xmm6, xmm1                ; for q2+tc2   
    psubw           xmm6, xmm6, xmm7          ; q2-tc2
    paddw           xmm5, xmm5, xmm7          ; q2+tc2
    
    pmaxsw          xmm0, xmm0, xmm6
%ifidn %2, false    
    movdqa          xmm6, [final_maskmx+0]
%endif
    pminsw          xmm0, xmm0, xmm5              ;xmm0 as src[2*stride]
%ifidn %2, false   
    maskedCopy      xmm1, xmm0, xmm6              ; output is xmm0
%endif 
%ifidn %1, Hor
    packuswb        xmm0, xmm0, xmm0
    movq            [r4+r5], xmm0        ; save src[-stride]
%else      
    movdqa          [src_tempmx+16*6], xmm0       ;save src[2*stride]
%endif

%ifidn %1, Hor
    movdqa          xmm0, [src_tempmx+16*0]     
    movdqa          xmm7, [src_tempmx+16*7]     
    packuswb        xmm0, xmm0, xmm0
    packuswb        xmm7, xmm7, xmm7
    movq            [srcq], xmm0        
    movq            [r4+4*strideq], xmm7       
%endif

%endmacro

;/************************************************************************
; MACRO:           luma_deblock_cross_core
; function:      
; args:            
; regs:
; steps:        output: r1=1, filter, r1=0, no filter      
;************************************************************************/      
        
%macro luma_deblock_cross_core    2 ; args: LumaCross_bypasswrite, Hor/Ver
    ;VO_S32 dp0 = ABS( src[-3*stride] - 2*src[-2*stride] + src[-stride] );
	;VO_S32 dq0 = ABS( src[0] - 2*src[stride] + src[2*stride] );
	;VO_S32 dp3 = ABS( src[3-3*stride] - 2*src[3-2*stride] + src[3-stride] );
	;VO_S32 dq3 = ABS( src[3] - 2*src[3+stride] + src[3+2*stride] );   
    psllw           xmm2, 1  ; *2
    psubw           xmm1, xmm1, xmm2
    paddw           xmm1, xmm1, xmm3
    pabsw           xmm1, xmm1            ; 1dp3, x, x, 1dp0, 0dp3 ,x, x, 0dp0
    movdqa          xmm2, xmm1            ; @using xmm2 as dp0, dp3 , for weak

    psllw           xmm5, 1  ; *2
    psubw           xmm4, xmm4, xmm5
    paddw           xmm4, xmm4, xmm6
    pabsw           xmm4, xmm4             ; 1dq3, x, x, 1dq0, 0dq3 ,x, x, 0dq0   , @using xmm4 as dq0, dq3 , for weak

    ;VO_S32 d0 = dp0 + dq0;
	;VO_S32 d3 = dp3 + dq3;
    ;cal d0 , d3
    paddw           xmm1, xmm1, xmm4      ; 1d3, x, x, 1d0, 0d3,x, x, 0d0

     ;cal d0+d3
    pshufhw         xmm7, xmm1, 0x0f ;  1d0, 1d0, 1d3, 1d3, 0d0,x, x, 0d3
    pshuflw         xmm7, xmm7, 0x0f ;  1d0, 1d0, 1d3, 1d3, 0d0,0d0, 0d3, 0d3

    pshufhw         xmm1, xmm1, 0xf0 ;  1d3, 1d3, 1d0, 1d0, 0d3,x, x, 0d0
    pshuflw         xmm1, xmm1, 0xf0 ;  1d3, 1d3, 1d0, 1d0, 0d3,0d3, 0d0, 0d0 ,  @using xmm1 as d0, d3, for weak/string decision
    paddw           xmm7, xmm7, xmm1  ;  1d3+1d0, 1d3+1d0, 1d0+1d3, 1d0+1d3, 0d3+0d0,0d3+0d0, 0d0+0d3, 0d0+0d3 

     ;cal beta   
     movd           xmm5, pbetad
     punpcklbw      xmm5, xmm5, xmm0
     punpcklwd      xmm5, xmm5, xmm0
     punpckldq      xmm5, xmm5, xmm0 ;  0, 0, 0, 1iBeta, 0, 0, 0, 0iBeta
     pshufhw        xmm5, xmm5, 0x00    ;  0, 0, 0, 1iBeta, 0, 0iBeta, 0iBeta, 0iBeta
     pshuflw        xmm5, xmm5, 0x00    ;  1iBeta, 1iBeta, 1iBeta, 1iBeta, 0iBeta, 0iBeta, 0iBeta, 0iBeta
     movdqa         xmm6, xmm5          ; @using xmm6 as iBeta , for weak/string decision and weak 
     ;if (d0 + d3 < iBeta) 
     pcmpgtw        xmm5, xmm5, xmm7
     movmskps       r1, xmm5            ; r1: 1filtermask, 1filtermask, 0filtermask, 0filtermask
     cmp            r1, 0
     je             %1
     mov            r5, r1
     shr            r5, 3
     shl            r5, 1
     and            r1, 1
     or             r1, r5       ; filter mask, bits 1 and bits 0  

    ;compare iBeta>>2
    ;(d0 << 1) < (iBeta>>2) && 
    ;(d3 << 1) < (iBeta>>2))
    psllw           xmm1, xmm1, 1  ; (1d3, 1d3, 1d0, 1d0, 0d3,0d3, 0d0, 0d0) * 2
    movdqa          xmm5, xmm6     ; restore Beta
    psrlw           xmm5, xmm5, 2  ; (1iBeta, 1iBeta, 1iBeta, 1iBeta, 0iBeta, 0iBeta, 0iBeta, 0iBeta) >> 2    ?
    pcmpgtw         xmm5, xmm5, xmm1   ;@release xmm1 as d0, d3, for weak/string decision
    movmskps        r4, xmm5   
    
    ;compare tc_s
    ;cal tc_s
    ;VO_S32 tc_s = (iTc*5+1)>>1;
     movdqa         xmm1, [src_tempmx+16*4]  ;load 4 row
     movd           xmm5, ptcd
     punpcklbw      xmm5, xmm5, xmm0
     punpcklwd      xmm5, xmm5, xmm0
     punpckldq      xmm5, xmm5, xmm0 ;  xmm5: 0, 0, 0, 1iTc, 0, 0, 0, 0iTc  ;@release xmm0 as const 0
     pshufhw        xmm5, xmm5, 0x00    ;  0, 0, 0, 1iTc, 0, 0iTc, 0iTc, 0iTc
     pshuflw        xmm5, xmm5, 0x00    ;  1iTc, 1iTc, 1iTc, 1iTc, 0iTc, 0iTc, 0iTc, 0iTc
     movdqa         xmm7, xmm5           ; @using xmm7 as iTc , for weak and strong
     movdqa         [tc_bufmx], xmm7
     psllw          xmm5, xmm5, 2
     pavgw          xmm5, xmm5, xmm7
    ;ABS(src[-stride] -  src[0]) < tc_s &&
    ;ABS(src[3-stride] - src[3]) < tc_s &&
    movdqa          xmm0, xmm1     ; xmm1 will be used soon
    psubw           xmm0, xmm0, xmm3               
    pabsw           xmm0, xmm0
    pshufhw         xmm0, xmm0, 0xf0 ;0b11110000;          ;1v3, 1v3, 1v0, 1v0,0v3, x, x, 0v0
    pshuflw         xmm0, xmm0, 0xf0 ;0b11110000;          ;1v3, 1v3, 1v0, 1v0,0v3, 0v3, 0v0, 0v0
    pcmpgtw         xmm5, xmm5, xmm0
    movmskps        r5, xmm5
    and             r4, r5                        ;r4: 0000 0000 0000 1flag1,1flag0,0flag1,0flag0,

    ;compare iBeta>>3
    ;ABS(src[-4*stride] -  src[-stride]) + ABS( src[3*stride] -  src[0]) < (iBeta>>3) && 		   
    ;ABS(src[3-4*stride] - src[3-stride]) + ABS(src[3+3*stride] - src[3]) < (iBeta>>3) && 
    movdqa          xmm0, [src_tempmx+16*0]  ;load 0 row  
    psubw           xmm0, xmm0, xmm3                      
    pabsw           xmm0, xmm0

    psubw           xmm1, xmm1, [src_tempmx+16*7]                      
    pabsw           xmm1, xmm1
    paddw           xmm0, xmm0, xmm1                      ;1v3, x, x, 1v0,0v3, x, x, 0v0

    pshufhw         xmm0, xmm0, 0xf0 ;0b11110000;          ;1v3, 1v3, 1v0, 1v0,0v3, x, x, 0v0
    pshuflw         xmm0, xmm0, 0xf0 ;0b11110000;          ;1v3, 1v3, 1v0, 1v0,0v3, 0v3, 0v0, 0v0
      
    movdqa          xmm5, xmm6     ; restore Beta
    psrlw           xmm5, xmm5, 3  ; (1iBeta, 1iBeta, 1iBeta, 1iBeta, 0iBeta, 0iBeta, 0iBeta, 0iBeta) >> 3    ?
    pcmpgtw         xmm5, xmm5, xmm0
    movmskps        r5, xmm5
    and             r4, r5 
   
    
    mov             r5, r4;
    shr             r5, 1;
    and             r4, r5               
    and             r4, 5                ; strong mask, bits 2 and 0  r4:  0000 0000 0000 x,1flag,x,0flag        
    
    mov             r5, r4
    shr             r5, 2
    shl             r5, 1
    and             r4, 1
    or              r4, r5               ; strong mask, bits 1 and 0  r4:  0000 0000 0000 0,0,1flag,0flag
    
    mov             [filter_stong_maskmx], r1   ;save filter mask
    mov             [filter_stong_maskmx+8], r4  ;save strong mask

    ;check and do weak filter firstly     
    not             r4            ; weak mask
    and             r1, r4        ; final weak mask
    cmp             r1, 0
    je              %%check_strong_filter
    mov             r4, r1
    and             r1, 1
    shr             r4, 1
    movd            xmm0, r1
    movd            xmm5, r4
    shufps          xmm0, xmm5, 0
    
    pcmpeqd         xmm5, xmm5; set all bits to 1
    psrld           xmm5, 31; set to 32bit 1
    pcmpeqd         xmm0, xmm0, xmm5; filtering mask
    movdqa          [final_maskmx+0], xmm0
    
    luma_deblock_cross_weak %2, false
    
    ;save weak filter result     
%ifidn %2, Hor
    mov             r1, [filter_stong_maskmx]
    mov             r4, [filter_stong_maskmx+8]

    and             r1, r4        ; final strong mask
    cmp             r1, 0
    je              %%weak_write_framebuffer
    movdqa          [src_tempmx+16*2], xmm3       
    movdqa          [src_tempmx+16*3], xmm6       
    movdqa          [src_tempmx+16*4], xmm7       
    movdqa          [src_tempmx+16*5], xmm2 
    movdqa          xmm7, [tc_bufmx]             ;for following strong filter
    jmp             %%do_strong_filter
    
%%weak_write_framebuffer 
    mov		        srcq, srcmp
    mov		        strideq, stridemp     
    cal_stride_mul3 srcq, strideq, r4 ,r5, true, true  
    
    packuswb        xmm3, xmm3, xmm3
    packuswb        xmm6, xmm6, xmm6
    packuswb        xmm7, xmm7, xmm7
    packuswb        xmm2, xmm2, xmm2
    movq            [srcq+strideq*2], xmm3       
    movq            [r4], xmm6       
    movq            [r4+strideq], xmm7       
    movq            [r4+strideq*2], xmm2 
    
    movdqa          xmm3, [src_tempmx+16*0] 
    movdqa          xmm6, [src_tempmx+16*1]     
    movdqa          xmm7, [src_tempmx+16*6]     
    movdqa          xmm2, [src_tempmx+16*7]     
    packuswb        xmm3, xmm3, xmm3
    packuswb        xmm6, xmm6, xmm6
    packuswb        xmm7, xmm7, xmm7
    packuswb        xmm2, xmm2, xmm2
    movq            [srcq], xmm3 
    movq            [srcq+strideq], xmm6
    movq            [r4+r5], xmm7      
    movq            [r4+4*strideq], xmm2   

    jmp             %%luma_deblock_cross_core_done   
    
%else
    movdqa          [src_tempmx+16*2], xmm3       
    movdqa          [src_tempmx+16*3], xmm6       
    movdqa          [src_tempmx+16*4], xmm7       
    movdqa          [src_tempmx+16*5], xmm2 
    movdqa          xmm7, [tc_bufmx]             ;for following strong filter
%endif
    
    ;strong filter   
%%check_strong_filter: 
    mov             r1, [filter_stong_maskmx]
    mov             r4, [filter_stong_maskmx+8]
   
    and             r1, r4        ; final strong mask
    cmp             r1, 0
    je              %%luma_deblock_cross_core_done
    cmp             r1, 3
    je              %%luma_deblock_cross_core_all_strong
%%do_strong_filter:   
    mov             r4, r1
    and             r1, 1
    shr             r4, 1
    movd            xmm0, r1
    movd            xmm5, r4
    shufps          xmm0, xmm5, 0
    
    pcmpeqd         xmm5, xmm5; set all bits to 1
    psrld           xmm5, 31; set to 32bit 1
    pcmpeqd         xmm0, xmm0, xmm5; filtering mask
    movdqa          [final_maskmx+0], xmm0    
  
    luma_deblock_cross_strong %2, false
    
    jmp             %%luma_deblock_cross_core_done    
     
%%luma_deblock_cross_core_all_strong:
    luma_deblock_cross_strong %2, true

%%luma_deblock_cross_core_done:   
    
%endmacro

;/************************************************************************
;static void voLumaFilterHor_C(VO_U8* src, VO_S32 stride, VO_S32 iBeta, VO_S32 iTc)
;************************************************************************/
cglobal voLumaFilterHor_ASM, 4, 6, 8 , 144, src, stride, iBeta, iTc

DECLARE_ALIGNED_LOCAL_BUF src_temp, 0  ;src_temp size = 8 S16 * 8 = 16*8 = 128
DECLARE_ALIGNED_LOCAL_BUF final_mask, 128 ;weak_mask size = 8 U16 = 16

%if ARCH_X86_64
    ; write Regs into stack for accessing later  , only x64 
    mov		        srcmp,   srcq

    ; extend sign, because stride is loaded by qword, however, saved in dword
    movsxd          strideq, strided
    mov		        stridemp,   strideq
%endif 

    cmp             iTcb, 0  ;tc[0]=0, jump to Luma_Hor_bypasswrite
    je              Luma_Hor_bypasswrite
    mov		        srcq, srcmp
    mov		        strideq, stridemp     
    cal_stride_mul3 srcq, strideq, r4 ,r5, false, true 
    load8X4_BW      srcq, strideq, r4 ,r5 ;src, stride, src+3*stride, 3*stide   
  
Luma_Hor_cal:
  
    luma_deblock_core Luma_Hor_bypasswrite, Hor   ; do horizontal filter and save result into frame buffer

    mov		        srcq, srcmp
    mov		        strideq, stridemp  
    cal_stride_mul3 srcq, strideq, r4 ,r5, false, true 
    save8X4_WB      srcq, strideq, r4 ,r5 ;src, stride, src+3*stride, 3*stide    
 
Luma_Hor_bypasswrite:    

    RET
    
;/************************************************************************
;static void voLumaFilterVer_C(VO_U8* src, VO_U32 stride, VO_S32 iBeta, VO_S32 iTc)
;************************************************************************/
cglobal voLumaFilterVer_ASM, 4, 6, 8 , 144, src, stride, iBeta, iTc

DECLARE_ALIGNED_LOCAL_BUF src_temp, 0  ;src_temp size = 8 S16 * 8 = 16*8 = 128
DECLARE_ALIGNED_LOCAL_BUF final_mask, 128 ;weak_mask size = 8 U16 = 16

%if ARCH_X86_64
        ; write Regs into stack for accessing later  , only x64 
    mov		        srcmp,   srcq

    ; extend sign, because stride is loaded by qword, however, saved in dword
    movsxd          strideq, strided
    mov		        stridemp,   strideq
%endif 

    cmp             iTcb, 0  ;tc[0]=0, jump to Luma_Ver_bypasswrite
    je              Luma_Ver_bypasswrite
    mov		        srcq, srcmp
    mov		        strideq, stridemp     
    cal_stride_mul3 srcq, strideq, r4 ,r5, true, false  
    transpose_load8X4_BW srcq, strideq, r4 ,r5 ;src, stride, src+3*stride, 3*stide   
  
Luma_Ver_cal:
  
    luma_deblock_core Luma_Ver_bypasswrite, Ver   ; do Ver filter 

    mov		        srcq, srcmp
    mov		        strideq, stridemp  
    cal_stride_mul3 srcq, strideq, r4 ,r5, true, false  
    transpose_save8X4_WB srcq, strideq, r4 ,r5 ;src, stride, src+3*stride, 3*stide    

Luma_Ver_bypasswrite:    

    RET

;/************************************************************************
;static void voLumaFilterCross_C(VO_U8* src, VO_U32 stride, VO_U32 pbeta, VO_U32 ptc)
;************************************************************************/
cglobal voLumaFilterCross_ASM, 4, 6, 8 , 176, src, stride, pbeta, ptc

DECLARE_ALIGNED_LOCAL_BUF src_temp, 0  ;src_temp size = 8 S16 * 8 = 16*8 = 128
DECLARE_ALIGNED_LOCAL_BUF final_mask, 128 ;weak_mask size = 8 U16 = 16
DECLARE_ALIGNED_LOCAL_BUF filter_stong_mask, 144 ;filter_stong_mask size = 8 U16 = 16
DECLARE_ALIGNED_LOCAL_BUF tc_buf, 160 ;tc_buf size = 8 U16 = 16, tc is used for both weak and strong filter

%if ARCH_X86_64
        ; write Regs into stack for accessing later  , only x64 
    mov		        srcmp,   srcq

    ; extend sign, because stride is loaded by qword, however, saved in dword
    movsxd          strideq, strided
    mov		        stridemp,   strideq
%endif 

    mov             r4, 0
    mov             [filter_stong_maskmx], r4  ;clear filter flag

    cmp             ptcw, 0  ;tc[0]=0&&tc[1]=0, jump to cross Hor filter
    je              LumaCross_Hor_check


    cal_stride_mul3 srcq, strideq, r4 ,r5, true, true  
    transpose_load8X8 srcq, strideq, r4 ,r5 ;src, stride, src+3*stride, 3*stide

    luma_deblock_cross_core LumaCross_Hor_check, Ver   ; do vertical filter    

LumaCross_Hor_check:
    shr             pbetad, 16
    shr             ptcd, 16
    cmp             ptcw, 0  ;tc[2]=0&&tc[3]=0, jump to cross Hor filter
    je              LumaCross_Hor_bypasswrite  

    mov             r1, [filter_stong_maskmx]
    cmp             r1, 0   ;check if doing Ver filter
    je              LumaCross_Hor_load
    ;transpose Ver and write result into src_temp for Hor filter
    transpose_load8X8_WW src_tempmx, 16, src_tempmx+48, 48

    jmp             LumaCross_Hor_cal
LumaCross_Hor_load: 
    ;no Ver filter, load data firstly
    mov		        srcq, srcmp
    mov		        strideq, stridemp     
    cal_stride_mul3 srcq, strideq, r4 ,r5, true, true  
    load8X8_BW      srcq, strideq, r4 ,r5 ;src, stride, src+3*stride, 3*stide     
  
LumaCross_Hor_cal:  
    luma_deblock_cross_core LumaCross_Hor_bypasswrite, Hor   ; do horizontal filter
    RET
    
LumaCross_Hor_bypasswrite:    
    mov             r1, [filter_stong_maskmx]
    cmp             r1, 0   ;check if doing Ver filter
    je              LumaCross_Ver_bypasswrite
    ;only Ver filter, transpose Ver and write result into src
    mov		        srcq, srcmp
    mov		        strideq, stridemp  
    cal_stride_mul3 srcq, strideq, r4 ,r5, true, true  

    cmp             ptcw, 0  ;tc[2]=0&&tc[3]=0, jump to cross Hor filter
    je              LumaTranspose_case

    save8X8_WB      srcq, strideq, r4 ,r5 ;src, stride, src+3*stride, 3*stide
    RET
    
LumaTranspose_case:
    transpose_save8X8_WB srcq, strideq, r4 ,r5 ;src, stride, src+3*stride, 3*stide

LumaCross_Ver_bypasswrite:    
    RET
    
 ;/************************************************************************
; MACRO:           chroma_deblock_core
; function:      
; args:            
; regs:         xmm2, xmm3, xmm4, xmm5 should be correct value
; steps:           
;************************************************************************/      
        
%macro chroma_deblock_core    1 ; args: Hor/Ver

    ;cal iTc
    pxor            xmm0, xmm0
    
    movd            xmm7, iTcd
    punpcklbw       xmm7, xmm7, xmm0
    punpcklwd       xmm7, xmm7, xmm0
    punpckldq       xmm7, xmm7, xmm0 ;  xmm7: 0, 0, 0, 1iTc, 0, 0, 0, 0iTc  ;@release xmm0 as const 0
    pshufhw         xmm7, xmm7, 0x00    ;  0, 0, 0, 1iTc, 0, 0iTc, 0iTc, 0iTc
    pshuflw         xmm7, xmm7, 0x00    ;  1iTc, 1iTc, 1iTc, 1iTc, 0iTc, 0iTc, 0iTc, 0iTc
    psubw           xmm0, xmm0, xmm7    

    movdqa          xmm1, xmm4

    ;Cal delta
    psubw           xmm4, xmm4, xmm3   ;q0 - p0 
    psubw           xmm2, xmm2, xmm5   ;p1 - q1    
    psllw           xmm4, xmm4, 2
    paddw           xmm4, xmm4, xmm2
    paddw           xmm4, xmm4, [CONST_S16_POS4]
    psraw           xmm4, xmm4, 3    
    pmaxsw          xmm4, xmm4, xmm0
    pminsw          xmm4, xmm4, xmm7              ; xmm4, delta
    
    ;
    paddw           xmm3, xmm3, xmm4             ;p0+delta
    psubw           xmm1, xmm1, xmm4             ;q0-delta
    
%ifidn %1, Hor     
    packuswb        xmm3, xmm3, xmm3
    packuswb        xmm1, xmm1, xmm1  
    movd            [r4], xmm3         ;save 3 row 
    movd            [r4+strideq], xmm1      ;save 4 row   
%else
    movdqa          [src_tempmx+16*3], xmm3 
    movdqa          [src_tempmx+16*4], xmm1  
%endif

%endmacro
   
;/************************************************************************
; MACRO:           chroma_deblock_cross_core
; function:      
; args:            
; regs:   xmm2, xmm3, xmm4, xmm5 should be correct value
; steps:           
;************************************************************************/      
        
%macro chroma_deblock_cross_core    2 ; args: LumaCross_bypasswrite, Hor/Ver

    ;cal iTc
    pxor            xmm0, xmm0
 %ifidn %2, Hor   
    movd            xmm7, iTcd
 %else
    movd            xmm7, r3d
 %endif
    punpcklbw       xmm7, xmm7, xmm0
    punpcklwd       xmm7, xmm7, xmm0
    punpckldq       xmm7, xmm7, xmm0 ;  xmm7: 0, 0, 0, 1iTc, 0, 0, 0, 0iTc  ;@release xmm0 as const 0
    pshufhw         xmm7, xmm7, 0x00    ;  0, 0, 0, 1iTc, 0, 0iTc, 0iTc, 0iTc
    pshuflw         xmm7, xmm7, 0x00    ;  1iTc, 1iTc, 1iTc, 1iTc, 0iTc, 0iTc, 0iTc, 0iTc
    psubw           xmm0, xmm0, xmm7    
 
    movdqa          xmm1, xmm4

    ;Cal delta
    psubw           xmm4, xmm4, xmm3   ;q0 - p0 
    psubw           xmm2, xmm2, xmm5   ;p1 - q1    
    psllw           xmm4, xmm4, 2
    paddw           xmm4, xmm4, xmm2
    paddw           xmm4, xmm4, [CONST_S16_POS4]
    psraw           xmm4, xmm4, 3    
    pmaxsw          xmm4, xmm4, xmm0
    pminsw          xmm4, xmm4, xmm7              ; xmm4, delta
    
    
    paddw           xmm3, xmm3, xmm4             ;p0+delta
    psubw           xmm1, xmm1, xmm4             ;q0-delta
    
 %ifidn %2, Hor
 
    ;Hor , write frame buffer
    cmp             r3w, 0
    je              %%chroma_deblock_cross_core_only_Hor
    
    movdqa          xmm0, [src_tempmx+16*0]  
    movdqa          xmm4, [src_tempmx+16*1] 
    movdqa          xmm2, [src_tempmx+16*2]

    movdqa          xmm5, [src_tempmx+16*5] 
    movdqa          xmm6, [src_tempmx+16*6]
    movdqa          xmm7, [src_tempmx+16*7]
    
    packuswb        xmm7, xmm7, xmm7
    packuswb        xmm3, xmm3, xmm3
    packuswb        xmm5, xmm5, xmm5
    packuswb        xmm1, xmm1, xmm1
    packuswb        xmm4, xmm4, xmm4
    packuswb        xmm2, xmm2, xmm2
    packuswb        xmm6, xmm6, xmm6
    packuswb        xmm0, xmm0, xmm0
    
 
    movq            [srcq], xmm0 ;save 0 row       
    movq            [srcq+strideq], xmm4 ;save 1 row       
    movq            [srcq+2*strideq], xmm2 ;save 2 row 
    movq            [r4], xmm3 ;save 3 row 
    movq            [r4+strideq], xmm1 ;save 4 row   
    movq            [r4+2*strideq], xmm5 ;save 5 row
    movq            [r4+r5], xmm6 ;save 6 row  
    movq            [r4+4*strideq], xmm7 ;save 7 row  
    
    jmp             %%chroma_deblock_cross_core_done    
 
%%chroma_deblock_cross_core_only_Hor:
    ;only Hor , write frame buffer, only two rows
    packuswb        xmm3, xmm3, xmm3
    packuswb        xmm1, xmm1, xmm1    

    movq            [r4], xmm3 ;save 3 row 
    movq            [r4+strideq], xmm1 ;save 4 row  
 %else    
    movdqa          [src_tempmx+16*3], xmm3 
    movdqa          [src_tempmx+16*4], xmm1  
    cmp             iTcw, 0
    je              %%chroma_deblock_cross_core_only_Ver
    ;Ver + Hor case   
    transpose_load8X8_WW src_tempmx, 16, src_tempmx+48, 48
    jmp             %%chroma_deblock_cross_core_done
    
 %%chroma_deblock_cross_core_only_Ver:
    ;only Ver , write frame buffer
    transpose_save8X8_WB srcq, strideq, r4 ,r5 ;src, stride, src+3*stride, 3*stide
    
 %endif 
 
%%chroma_deblock_cross_core_done:     

%endmacro

;/************************************************************************
;static void voChromaFilterHor_C(VO_U8* src,VO_S32 stride, VO_S32 iTc)
;************************************************************************/
cglobal voChromaFilterHor_ASM, 3, 6, 8 , 128, src, stride, iTc

DECLARE_ALIGNED_LOCAL_BUF src_temp, 0  ;src_temp size = 8 S16 * 8 = 16*8 = 128
%if ARCH_X86_64
    ; extend sign, because stride is loaded by qword, however, saved in dword
    movsxd          strideq, strided       
%endif 
  
    cal_stride_mul3 srcq, strideq, r4 ,r5, false, true     
    load8X4_BW      srcq, strideq, r4 ,r5 ;src, stride, src+3*stride, 3*stide  
    chroma_deblock_core Hor   ; do horizontal filter and write frame buffer
    
    RET

;/************************************************************************
;static void voChromaFilterVer_C(VO_U8* src,VO_U32 stride, VO_S32 iTc)
;************************************************************************/
cglobal voChromaFilterVer_ASM, 3, 6, 8 , 128, src, stride, iTc

DECLARE_ALIGNED_LOCAL_BUF src_temp, 0  ;src_temp size = 8 S16 * 8 = 16*8 = 128

%if ARCH_X86_64
    ; extend sign, because stride is loaded by qword, however, saved in dword
    movsxd          strideq, strided
%endif 
   
    cal_stride_mul3 srcq, strideq, r4 ,r5, true, false  

    transpose_load8X4_BW srcq, strideq, r4 ,r5 ;src, stride, src+3*stride, 3*stide    
    chroma_deblock_core Ver   ; do Ver filter   
    transpose_save8X4_WB srcq, strideq, r4 ,r5 ;src, stride, src+3*stride, 3*stide 

    RET
    
;/************************************************************************
;static void voChromaFilterCross_C(VO_U8* src, VO_U32 stride, VO_U32 iTc)
;************************************************************************/
cglobal voChromaFilterCross_ASM, 3, 6, 8 , 128, src, stride, iTc

DECLARE_ALIGNED_LOCAL_BUF src_temp, 0  ;src_temp size = 8 S16 * 8 = 16*8 = 128
%if ARCH_X86_64
    ; extend sign, because stride is loaded by qword, however, saved in dword
    movsxd          strideq, strided
%endif 

    cal_stride_mul3 srcq, strideq, r4 ,r5, true, true   ;srcq, strideq, r4 ,r5 must be global

    mov             r3w, iTcw
    shr             iTcd, 16
    cmp             r3w, 0  ;tc[0]=0&&tc[1]=0, jump to cross Hor filter
    je              ChromaCross_Hor_check
   
    transpose_load8X8 srcq, strideq, r4 ,r5 ;src, stride, src+3*stride, 3*stide  
    chroma_deblock_cross_core ChromaCross_Hor_check, Ver  ; do vertical filter    

ChromaCross_Hor_check:
    cmp             iTcw, 0  ;tc[2]=0&&tc[3]=0, jump to cross Hor filter
    je              ChromaCross_Hor_bypasswrite  

    cmp             r3w, 0   ;check if doing Ver filter
    jne             ChromaCross_Hor_cal
  
ChromaCross_Hor_load: 
    ;no Ver filter, load data firstly
    load8X8_BW      srcq, strideq, r4 ,r5 ;src, stride, src+3*stride, 3*stide     
  
ChromaCross_Hor_cal:  
    chroma_deblock_cross_core ChromaCross_Hor_bypasswrite, Hor   ; do horizontal filter
  
ChromaCross_Hor_bypasswrite:  

    RET


    end                                                        ; Mark end of file