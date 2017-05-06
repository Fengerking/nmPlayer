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

	cextern CONST_U16_0;
	cextern CONST_U16_1;
	cextern CONST_U16_2;

	cextern CONST_U8_0
	cextern CONST_U8_1
	cextern CONST_U8_2
	cextern CONST_U8_3
	cextern CONST_U8_4

    section  .text

	
    align    4  

	

;/************************************************************************
; MACRO:            qAddUS
; function:         
; args:             
; regs: 
; steps: convert into 16 bits then use packuswb(Unsigned Saturation)            
;************************************************************************/
       
%macro  qAddUS              5  ; args: input1, input2, output, temp1(for CONST_U8_0), temp2
       
        movdqa %4, [CONST_U8_0]

        ;deal low 64 bits
        punpcklbw %5, %1, [CONST_U8_0]				
        punpcklbw %3, %4, %2
        psraw %3, %3, 8       
        paddw %5, %5,%3  

        ;deal high 64 bits
        punpckhbw %3, %1, [CONST_U8_0]				
        punpckhbw %4, %4, %2  
        psraw %4, %4, 8
        paddw %4, %4,%3   

        packuswb %3, %5, %4
        
%endmacro

;/************************************************************************
; MACRO:            xSignSimple
; function:         $input1 < $input2, return 0; =, return 1; >, return 2
; args:             $
; regs:
; steps: convert into 16 bits then use packuswb(Unsigned Saturation)                
;************************************************************************/      
        
%macro xSignSimple     4 ; args: output, input1, input2, temp1

        cmp %2, %3
        setg %1
        setge %4
        add %1,%4
                
%endmacro

;/************************************************************************
; MACRO:            xSign
; function:         $input1 < $input2, return 0; =, return 1; >, return 2
; args:             $
; regs:
; steps: convert into 16 bits then use packuswb(Unsigned Saturation)                
;************************************************************************/       
        
%macro  xSign              6    ; args: input1, input2, output, temp1, temp2,temp3

		punpcklbw %4, %1, [CONST_U8_0] 
	    punpcklbw %5, %2, [CONST_U8_0]		
		pcmpeqw %3,%4,%5        ;we can NOT use pcmpeqb, because it's signed byte
		andps  %3,%3,[CONST_U16_1]
		pcmpgtw %4,%4,%5
		andps  %4,%4,[CONST_U16_2]
		orps %3, %3, %4

		punpckhbw %4, %1, [CONST_U8_0]
	    punpckhbw %5, %2, [CONST_U8_0]		
		pcmpeqw %6,%4,%5
		andps  %6,%6,[CONST_U16_1]
		pcmpgtw %4,%4,%5
		andps  %4,%4,[CONST_U16_2]
		orps %6, %6, %4

		packuswb %3, %3, %6	
    
%endmacro


;/************************************************************************
; MACRO:            writeQReg
; function:         cnt is between 1 to 15, including 1 and 15
; args:            
; regs:
; steps:  read 16 bits from memory , shift it and REG, then merge them             
;************************************************************************/
%macro  writeQReg_const              5    ; args: reg,addr,offset, cnt, go_label
      
        psrldq      xmm6, %4       ;Operand 2 can only be imm
        pslldq      xmm6, %4
        pslldq      %1,16-%4 
        psrldq      %1,16-%4 

        pxor        %1, %1, xmm6
        movdqu      [%2+%3], %1
        jmp         %5
%endmacro

%macro  writeQReg              5    ; args: reg,addr,offset, cnt, go_label
        movdqu      xmm6, [%2+%3]
        cmp         %4, 15
        jz          %%write_15_bytes
        cmp         %4, 14
        jz          %%write_14_bytes
        cmp         %4, 7
        jz          %%write_7_bytes
        cmp         %4, 6
        jz          %%write_6_bytes
        cmp         %4, 13
        jz          %%write_13_bytes
        cmp         %4, 12
        jz          %%write_12_bytes
        cmp         %4, 11
        jz          %%write_11_bytes
        cmp         %4, 10
        jz          %%write_10_bytes
        cmp         %4, 9
        jz          %%write_9_bytes
        cmp         %4, 8
        jz          %%write_8_bytes
        cmp         %4, 5
        jz          %%write_5_bytes
        cmp         %4, 4
        jz          %%write_4_bytes
        cmp         %4, 3
        jz          %%write_3_bytes
        cmp         %4, 2
        jz          %%write_2_bytes
      
%%write_1_bytes:
        writeQReg_const %1, %2, %3, 1, %5  
%%write_2_bytes:
        writeQReg_const %1, %2, %3, 2, %5 
%%write_3_bytes:
        writeQReg_const %1, %2, %3, 3, %5
%%write_4_bytes:
        writeQReg_const %1, %2, %3, 4, %5
%%write_5_bytes:
        writeQReg_const %1, %2, %3, 5, %5
%%write_6_bytes:
        writeQReg_const %1, %2, %3, 6, %5 
%%write_7_bytes:
        writeQReg_const %1, %2, %3, 7, %5 
%%write_8_bytes:
        writeQReg_const %1, %2, %3, 8, %5 
%%write_9_bytes:
        writeQReg_const %1, %2, %3, 9, %5
%%write_10_bytes:
        writeQReg_const %1, %2, %3, 10, %5
%%write_11_bytes:
        writeQReg_const %1, %2, %3, 11, %5
%%write_12_bytes:
        writeQReg_const %1, %2, %3, 12, %5
%%write_13_bytes:
        writeQReg_const %1, %2, %3, 13, %5
%%write_14_bytes:
        writeQReg_const %1, %2, %3, 14, %5 
%%write_15_bytes:
        writeQReg_const %1, %2, %3, 15, %5
        
%endmacro

;/************************************************************************
; MACRO:            mm_set1_epi8
; function:         set dst[0~15] = src[0],  src is a byte
; args:             
; regs:
; steps:                 
;************************************************************************/
       
%macro  mm_set1_epi8  6           ; args: src, offset, eax, xmm0, xmm1, dst 

        movsx       %3,byte  [%1+%2] 
        movd       %4,%3  

        movdqu     %5,%4 
        punpcklbw  %4,%4,%5  

        movdqu     %5,%4
        punpcklbw  %4,%4,%5  

        pshufd     %4,%4,0  
        movdqu     %6,%4
        
%endmacro


;/************************************************************************
; MACRO:            extend_offset
; function:        
; args:             
; regs:
; steps:          extend   iOffsetEo[0], iOffsetEo[1], iOffsetEo[3] , iOffsetEo[4]   
;************************************************************************/
       
%macro  extend_offset  9           ; args: iOffsetEo, [iOffsetEo], eax, xmm0, xmm1, dst0, dst1, dst3, dst4 

		 mov       %2,%1     ;extend iOffsetEo[0]
		 mm_set1_epi8 %2, 0, %3, %4, %5, %6  
		 mov       %2,%1 	 ;extend iOffsetEo[1]
		 mm_set1_epi8 %2, 1, %3, %4, %5, %7   
		 mov       %2,%1     ;extend iOffsetEo[3]
		 mm_set1_epi8 %2, 3, %3, %4, %5, %8   
		 mov       %2,%1     ;extend iOffsetEo[4]
		 mm_set1_epi8 %2, 4, %3, %4, %5, %9   
          
%endmacro


;/************************************************************************
; MACRO:            get_edge_offset
; function:        
; args:             
; regs:
; steps:              
;************************************************************************/
       
%macro  get_edge_offset  7           ; args: edgeType(i), edgeOffset(o),temp1, temp2, temp3, temp4, offset0m

		movdqa		 %3, [CONST_U8_0]
        pcmpeqb %3,  %1

        movdqa		 %4, [CONST_U8_1]
        pcmpeqb %4,  %1

        movdqa		 %5, [CONST_U8_3]
        pcmpeqb %5,  %1

        movdqa		 %6, [CONST_U8_4]
        pcmpeqb %6,  %1
			
        ;get edge offset   
        pand %3, %3, [%7]    
        pand %4,  %4, [%7+16]
        pand %5,  %5, [%7+16*3]
        pand %6,  %6, [%7+16*4]  

        por %3, %3, %4
        por %5, %5, %6
        por %2, %3, %5

%endmacro  

;/************************************************************************
;   VO_VOID processSaoEO0(VO_S32 iStartX, VO_S32 uiCurLCURowStartY, 
;                 VO_S32 uiCurLCURowStartY, VO_S32 uiCurLCURowEndY, VO_U8* pRec, 
;                 VO_U32 uiStride, VO_U8 *pTmpL, VO_S8 *iOffsetEo)
;************************************************************************/

cglobal processSaoEO0_asm, 6, 6, 8 , 80, iStartX, uiCurLCURowStartY,iEndX, uiCurLCURowEndY, pRec, uiStride,pTmpL,iOffsetEo

DECLARE_ALIGNED_LOCAL_BUF offset0, 0

case_SAO_EO_0_yLoop_prepare:  

%if ARCH_X86_64
        ; write Regs into stack for accessing later  , only x64 
        mov		   iStartXmp,   iStartXq
        mov		   iEndXmp,    iEndXq

        ; extend sign, because uiStride is loaded by qword, however, saved in dword
        movsxd   uiStrideq, uiStrided
%endif  

        sub                 uiCurLCURowEndYq, uiCurLCURowStartYq 
        sub                 iEndXq, iStartXq
          
        add                 pTmpLmp, uiCurLCURowStartYq   ;pTmpL+=uiCurLCURowStartY

        ;extend offset into 16 bytes   
        extend_offset iOffsetEom, r1, r1, xmm0, xmm1, [offset0mx+0], [offset0mx+16], [offset0mx+16*3], [offset0mx+16*4]	      
                
case_SAO_EO_0_yLoop:        ; for height	

        ; load 16 bits, but lowest byte is from pTmpL
        movdqu		 xmm0, [pRecq+iStartXq-1]				 
        mov r1d, [pRecq+iStartXq-1]
        mov r0, pTmpLm
        mov r1b , [r0]
        mov	iStartXq,   iStartXmp        	
        movd xmm3, r1d
        movss xmm0,xmm3   

case_SAO_EO_0_xLoop:        ; for width

        movdqu		 xmm1, [pRecq+iStartXq]
        movdqu		 xmm2, [pRecq+iStartXq+1]

        ;get edge type
        xSign xmm1, xmm0, xmm3, xmm4, xmm5, xmm6 ;xmm3, iSignLeft
        xSign xmm1, xmm2, xmm4, xmm5, xmm6, xmm7 ;xmm4, iSignRight	
        paddb xmm3, xmm3, xmm4                    ;xmm3, edgeType

        get_edge_offset xmm3, xmm4, xmm4, xmm5, xmm7, xmm0, offset0mx ; xmm4, edgeOffset

        qAddUS xmm1,xmm4,xmm7,xmm6,xmm5                    ; xmm1, YUV.  xmm4,edgeOffset. xmm7, out YUV

    
        movdqu		 xmm0, [pRecq+iStartXq+15]   ;prefetch for next x loop               
        cmp iEndXq, 16
        jge case_SAO_EO_0_16values
       

        writeQReg     xmm7, pRecq, iStartXq, iEndXq, case_SAO_EO_0_post_xLoop 

case_SAO_EO_0_16values:				 
        movdqu		 [pRecq+iStartXq],xmm7
        add          iStartXq, 16
        sub          iEndXq, 16
        jg           case_SAO_EO_0_xLoop               ; width loop

case_SAO_EO_0_post_xLoop:       
        lea          pRecq,[pRecq+uiStrideq]			; dst += dststride    

        add          pTmpLmp, 1                     ;  pTmpL += 1          

        mov		   iStartXq,   iStartXmp
        mov		   iEndXq,    iEndXmp
        sub        iEndXq, iStartXq   
				         
        dec          uiCurLCURowEndYq                
        jg           case_SAO_EO_0_yLoop               ; height loop

        RET      


;/************************************************************************
;       VO_VOID processSaoEO1(VO_S32 iLcuWidth, VO_S32 iStartY, 
;           VO_S32 iEndY, VO_U8* pRec, VO_U32 uiStride, 
;           VO_U8 *pTmpBuff1, VO_S8 *iOffsetEo){
;************************************************************************/

cglobal processSaoEO1_asm, 6, 6, 8 , 80, iLcuWidth, iStartY, iEndY, pRec, uiStride, pTmpBuff1, iOffsetEo

DECLARE_ALIGNED_LOCAL_BUF offset0, 0

case_SAO_EO_1_yLoop_prepare:  

%if ARCH_X86_64
        ; write Regs into stack for accessing later  , only x64 
        mov		   iLcuWidthmp,   iLcuWidthq

        ; extend sign, because uiStride is loaded by qword, however, saved in dword
        movsxd   uiStrideq, uiStrided
        mov      uiStridemp, uiStrideq   ;for access from memory later
%endif  
        sub                 iEndYq, iStartYq 

        ;extend offset into 16 bytes   
        extend_offset iOffsetEom, r1, r1, xmm0, xmm1, [offset0mx+0], [offset0mx+16], [offset0mx+16*3], [offset0mx+16*4]	      
                
case_SAO_EO_1_yLoop:        ; for height	

        mov r1, 0

case_SAO_EO_1_xLoop:        ; for width

        movdqu		 xmm1, [pRecq+r1]
        add          uiStrideq, r1
        movdqu		 xmm2, [pRecq+uiStrideq]
        mov          uiStrideq, uiStridemp

        ;get edge type
        xSign xmm1, xmm2, xmm3, xmm4, xmm5, xmm6     ;xmm3, iSignDown
        movdqu		 xmm4, [pTmpBuff1q+r1]              ;xmm4, iSignUp , from pTmpBuff1
        movdqa		 xmm2, [CONST_U8_2]
        psubb        xmm2, xmm2, xmm3
        movdqu		 [pTmpBuff1q+r1], xmm2             ;save (2-iSignDown) into pTmpBuff1 
        paddb xmm3, xmm3, xmm4                       ;xmm3, edgeType

        get_edge_offset xmm3, xmm4, xmm4, xmm5, xmm7, xmm0, offset0mx ; xmm4, edgeOffset
        qAddUS xmm1,xmm4,xmm7,xmm6,xmm5                    ; xmm1, YUV.  xmm4,edgeOffset. xmm7, out YUV
               
        cmp iLcuWidthq, 16
        jge case_SAO_EO_1_16values       
        
        writeQReg     xmm7, pRecq, r1, iLcuWidthq, case_SAO_EO_1_post_xLoop 

case_SAO_EO_1_16values:				 
        movdqu		 [pRecq+r1],xmm7
        add          r1, 16
        sub          iLcuWidthq, 16
        jg           case_SAO_EO_1_xLoop               ; width loop

case_SAO_EO_1_post_xLoop:      
        lea          pRecq,[pRecq+uiStrideq]			; dst += dststride    
        mov		     iLcuWidthq,   iLcuWidthmp          ; restore iLcuWidth from stack
        dec          iEndYq                
        jg           case_SAO_EO_1_yLoop               ; height loop

        RET

;/************************************************************************
;    VO_VOID processSaoEO2(VO_S32 iStartX, VO_S32 iStartY, VO_S32 iEndX,
;          VO_S32 iEndY, VO_U8* pRec, VO_U32 uiStride,
;          VO_U8 *pTmpBuff1, VO_U8 *pTmpL, VO_S8 *iOffsetEo){
;************************************************************************/

cglobal processSaoEO2_asm, 6, 6, 8 , 80, iStartX, iStartY, iEndX, iEndY, pRec,  uiStride, pTmpBuff1, pTmpL, iOffsetEo

DECLARE_ALIGNED_LOCAL_BUF offset0, 0

case_SAO_EO_2_yLoop_prepare:  

%if ARCH_X86_64
        ; write Regs into stack for accessing later  , only x64 
        mov		   iStartXmp,   iStartXq
        mov		   iEndXmp,    iEndXq

        ; extend sign, because uiStride is loaded by qword, however, saved in dword
        movsxd   uiStrideq, uiStrided
        mov      uiStridemp, uiStrideq   ;for access from memory later
%endif  
        sub                 iEndXq, iStartXq  
        sub                 iEndYq, iStartYq

        add                 pTmpLmp, iStartYq   ;pTmpL+=iStartY
        

        ;extend offset into 16 bytes   
        extend_offset iOffsetEom, r1, r1, xmm0, xmm1, [offset0mx+0], [offset0mx+16], [offset0mx+16*3], [offset0mx+16*4]
        mov r1, pTmpBuff1m	      
                
case_SAO_EO_2_yLoop:        ; for height	

      
        movdqu xmm0 , [r1+iStartXq] ;xmm0, iSignUp , from pTmpBuff1

case_SAO_EO_2_xLoop:        ; for width

        movdqu		 xmm1, [pRecq+iStartXq]
        add          uiStrideq, iStartXq
        movdqu		 xmm2, [pRecq+uiStrideq+1]
        mov          uiStrideq, uiStridemp

        ;get edge type
        xSign xmm1, xmm2, xmm3, xmm4, xmm5, xmm6         ;xmm3, iSignDown       
        paddb xmm6, xmm3, xmm0                            ;xmm6, edgeType        
        movdqa		 xmm2, [CONST_U8_2]
        movdqu		 xmm0, [r1+iStartXq+16]              ;xmm0, iSignUp , from pTmpBuff1
        psubb        xmm2, xmm2, xmm3
        movdqu		 [r1+iStartXq+1], xmm2               ;save (2-iSignDown) into pTmpBuff1         

        get_edge_offset xmm6, xmm4, xmm4, xmm5, xmm7, xmm2, offset0mx ; xmm4, edgeOffset
        qAddUS xmm1,xmm4,xmm7,xmm6,xmm5                    ; xmm1, YUV.  xmm4,edgeOffset. xmm7, out YUV
               
        cmp iEndXq, 16
        jge case_SAO_EO_2_16values       
        
        writeQReg     xmm7, pRecq, iStartXq, iEndXq, case_SAO_EO_2_post_xLoop 

case_SAO_EO_2_16values:	       	 
        movdqu		 [pRecq+iStartXq],xmm7
        add          iStartXq, 16
        sub          iEndXq, 16
        jg           case_SAO_EO_2_xLoop               ; width loop

case_SAO_EO_2_post_xLoop:       
        lea          pRecq,[pRecq+uiStrideq]			; dst += dststride
        
        ;iSignDown2 = xSign(pRec[uiStride+iStartX] - pTmpL[y]);
        ;pTmpBuff1[iStartX] = iSignDown2;
        mov r0, pTmpLm       
        movzx r1w, [r0]
        mov		     iStartXq,   iStartXmp              ; restore from stack        
        movzx r2w, [pRecq+iStartXq]   
        xSignSimple  r2b, r2w, r1w, r1b                 ; output is in r2b
        mov       r1, pTmpBuff1m	 
        mov		 [r1+iStartXq], r2b              

        add          pTmpLmp, 1                     ;  pTmpL += 1   

        mov		     iEndXq,   iEndXmp
        sub          iEndXq,   iStartXq                     
        dec          iEndYq 
                  
        jg           case_SAO_EO_2_yLoop               ; height loop

        RET


;/************************************************************************
;     VO_VOID processSaoEO3(VO_S32 iStartX, VO_S32 iStartY, VO_S32 iEndX, 
;           VO_S32 iEndY, VO_U8* pRec, VO_U32 uiStride, 
;           VO_U8 *pTmpBuff1, VO_U8 *pTmpL, VO_S8 *iOffsetEo){
;************************************************************************/

cglobal processSaoEO3_asm, 6, 6, 8 , 80, iStartX, iStartY, iEndX, iEndY, pRec,  uiStride, pTmpBuff1, pTmpL, iOffsetEo

DECLARE_ALIGNED_LOCAL_BUF offset0, 0

case_SAO_EO_3_yLoop_prepare:  

%if ARCH_X86_64
        ; write Regs into stack for accessing later  , only x64 
        mov		   iStartXmp,   iStartXq
        mov		   iEndXmp,    iEndXq

        ; extend sign, because uiStride is loaded by qword, however, saved in dword
        movsxd   uiStrideq, uiStrided
        mov      uiStridemp, uiStrideq   ;for access from memory later
%endif      
        sub                 iEndYq, iStartYq

        add                 pTmpLmp, iStartYq                  ;pTmpL+=iStartY
        add                 pTmpLmp, 1                         ;for  access pTmpL[y+1]

        ;extend offset into 16 bytes   
        extend_offset iOffsetEom, r1, r1, xmm0, xmm1, [offset0mx+0], [offset0mx+16], [offset0mx+16*3], [offset0mx+16*4]
           
                
case_SAO_EO_3_yLoop:        ; for height

        ; x=iStartX;
        ; iSignDown1      =  xSign(pRec[x] - pTmpL[y+1]) ;
        ; uiEdgeType      =  iSignDown1 + pTmpBuff1[x] ;  
        ; pRec[x] = Clip(pRec[x] + iOffsetEo[uiEdgeType]);
        mov r2, pTmpLm 
        movzx         r1w, [pRecq+iStartXq]
        movzx r2w, [r2]     
        xSignSimple  r1b, r1w, r2w, r0b                 ; output is in r1b

        mov r2, pTmpBuff1m	
        mov		 iStartXq,   iStartXmp                  ; restore from stack    
        mov r0b, [r2+iStartXq]   
        add r0b, r1b    
        movzx r0, r0b   ;uiEdgeType is in r0
        mov r1, iOffsetEom
        add r1, r0
        movsx r2w, [r1]                             ;iOffsetEo[uiEdgeType] is in r2w

        mov		 iStartXq,   iStartXmp              ; restore from stack  
        movzx         r1w, [pRecq+iStartXq] 
        add r1w, r2w                                ;pRec[x] + iOffsetEo[uiEdgeType]
        mov r2w, 0
        cmp r1w, r2w
        cmovl r1w,r2w
        mov r2w, 255
        cmp r1w, r2w
        cmovg r1w,r2w
        mov [pRecq+iStartXq], r1b      
        
        ; restore from stack
        mov		 iEndXq,   iEndXmp  
        mov		 iStartXq,   iStartXmp              
        inc      iStartXq
        sub      iEndXq,   iStartXq    
                
        mov      r1, pTmpBuff1m	
        

case_SAO_EO_3_xLoop:        ; for width

        movdqu		 xmm1, [pRecq+iStartXq]
        add          uiStrideq, iStartXq
        movdqu		 xmm2, [pRecq+uiStrideq-1]
        mov          uiStrideq, uiStridemp

        ;get edge type
        movdqu		 xmm0, [r1+iStartXq]              ;xmm0, iSignUp , from pTmpBuff1
        xSign xmm1, xmm2, xmm3, xmm4, xmm5, xmm6         ;xmm3, iSignDown       
        paddb xmm6, xmm3, xmm0                            ;xmm6, edgeType        
        movdqa		 xmm2, [CONST_U8_2]
       
        psubb        xmm2, xmm2, xmm3
        movdqu		 [r1+iStartXq-1], xmm2               ;save (2-iSignDown) into pTmpBuff1         

        get_edge_offset xmm6, xmm4, xmm4, xmm5, xmm7, xmm2, offset0mx ; xmm4, edgeOffset
        qAddUS xmm1,xmm4,xmm7,xmm6,xmm5                    ; xmm1, YUV.  xmm4,edgeOffset. xmm7, out YUV
               
        cmp iEndXq, 16
        jge case_SAO_EO_3_16values       
        
        writeQReg     xmm7, pRecq, iStartXq, iEndXq, case_SAO_EO_3_post_xLoop 

case_SAO_EO_3_16values:				 
        movdqu		 [pRecq+iStartXq],xmm7
        add          iStartXq, 16
        sub          iEndXq, 16
        jg           case_SAO_EO_3_xLoop               ; width loop

case_SAO_EO_3_post_xLoop:  
        
        ;pTmpBuff1[iEndX-1] = xSign(pRec[iEndX-1 + uiStride] - pRec[iEndX]);
        mov		     iEndXq,   iEndXmp
        movzx         r0w, [pRecq+iEndXq]
        lea          pRecq,[pRecq+uiStrideq]			; dst += dststride
        movzx         r1w, [pRecq+iEndXq-1]   
        xSignSimple  r0b, r1w, r0w, r1b                 ; output is in r0b
        mov       r1, pTmpBuff1m	 
        mov		 [r1+iEndXq-1], r0b              

        add          pTmpLmp, 1                         ;  pTmpL += 1
        mov		     iStartXq,   iStartXmp              ; restore from stack      
                        
        dec          iEndYq 
        jg           case_SAO_EO_3_yLoop               ; height loop

        RET

;/************************************************************************
;     VO_VOID processSaoBo(VO_S32 iLcuWidth, VO_S32 uiCurLCURowStartY, 
;           VO_S32 uiCurLCURowEndY, VO_U8* pRec, 
;           VO_U32 uiStride, VO_U8 *iOffsetBo){
;************************************************************************/

cglobal processSaoBo_asm, 6, 7, 8 , 0, iLcuWidth, uiCurLCURowStartY, uiCurLCURowEndY, pRec, uiStride, iOffsetBo

case_SAO_BO_yLoop_prepare:  

%if ARCH_X86_64
        ; write Regs into stack for accessing later  , only x64 
        mov		   iLcuWidthmp,   iLcuWidthq

         ; extend sign, because uiStride is loaded by qword, however, saved in dword
        movsxd   uiStrideq, uiStrided       
%endif  
        sub                 uiCurLCURowEndYq, uiCurLCURowStartYq 
                
case_SAO_BO_yLoop:        ; for height	
        mov r6, 0

case_SAO_BO_xLoop:        ; for width     
        movzx        r1, byte [pRecq+r6]           
        mov          r1b, [iOffsetBoq+r1]        
        mov          byte [pRecq+r6]  , r1b
                
        inc          r6
        dec          iLcuWidthq
        jg           case_SAO_BO_xLoop                  ; width loop

        lea          pRecq,[pRecq+uiStrideq]			; dst += dststride    
        mov		     iLcuWidthq,   iLcuWidthmp          ; restore iLcuWidth from stack
        dec          uiCurLCURowEndYq                
        jg           case_SAO_BO_yLoop                  ; height loop

        RET

end                                                        ; Mark end of file
    
