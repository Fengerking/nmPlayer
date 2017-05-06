@/************************************************************************
@ VisualOn Proprietary
@ Copyright (c) 2012, VisualOn Incorporated. All rights Reserved
@
@ VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
@
@ All data and information contained in or disclosed by this document are
@ confidential and proprietary information of VisualOn, and all rights
@ therein are expressly reserved. By accepting this material, the
@ recipient agrees that this material and the information contained
@ therein are held in confidence and in trust. The material may only be
@ used and/or disclosed as authorized in a license agreement controlling
@ such use and disclosure.
@************************************************************************/
    
    #include     "h265dec_ASM_config.h"
    @AREA |.text|, CODE, READONLY, ALIGN=4
    .section  .text
    .align    4  
    .if SAO_ASM_ENABLED==1    
    .global processSaoEO0_asm
    .global processSaoEO1_asm    
    .global processSaoEO2_asm    
    .global processSaoEO3_asm    
    .global processSaoBo_asm    
   
@/************************************************************************
@ MACRO:            qAddUS
@ function:         $output = sat($input1+$input2), output!=input1!=input2
@ args:             $output,unsigned   $input1,signed   $input2,unsigned
@ regs:             $temp1, $temp2, $temp3  
@************************************************************************/
       
.macro  qAddUS               output_0, output_1, input1_0, input1_1, input2_0, input2_1, temp1, temp2  

        vmovl.u8            \temp1, \input1_0
        vmovl.u8            \temp2, \input1_1
        vaddw.s8            \temp1, \temp1, \input2_0
        vaddw.s8            \temp2, \temp2, \input2_1
      
        vqmovun.s16         \output_0,  \temp1
        vqmovun.s16         \output_1,  \temp2
     
        
        .endm
        
@/************************************************************************
@ MACRO:            xSign
@ function:         $input1 < $input2, retu.req r0@ =, retu.req r1@ >, retu.req r2
@ args:             $
@ regs:             $temp1, $temp2, 
@************************************************************************/       
        
 .macro xSign               output, input1, input2, temp1, temp2 
        
        vcgt.u8             \temp1,\input1,\input2
        vand.u8             \temp1,\temp1,CONST_1
        vcge.u8             \temp2,\input1,\input2 
        vand.u8             \temp2,\temp2,CONST_1      
        vadd.u8             \output,\temp1,\temp2    
        
        .endm
        
@/************************************************************************
@ MACRO:            xSignSimple
@ function:         $input1 < $input2, retu.req r0@ =, retu.req r1@ >, retu.req r2
@ args:             $
@ regs:             $temp1, $temp2, 
@************************************************************************/      
        
 .macro xSignSimple               output, input1, input2
        
        subs                \input1,\input2
        movlt               \output,#0
        moveq               \output,#1      
        movgt               \output,#2
                
        .endm
        
@/************************************************************************
@ MACRO:            writeQReg
@ function:         cnt is between 1 to 15, including 1 and 15
@ args:             $
@ regs:             $temp1, $temp2, 
@************************************************************************/
        
 .macro writeQReg          label, reg0, reg1, addr, cnt, tmp, go_label

        add                 \addr, \addr, \cnt
        vld1.8              {\reg1}, [\addr]
        

        ADR \tmp, \label\(_caseTable)
        LDR pc, [\tmp,\cnt,LSL#2]
           
\label\(_caseTable):
        .word \label\(_paddingOffset)
        .word \label\(_write1bytes)
        .word \label\(_write2bytes)
        .word \label\(_write3bytes)
        .word \label\(_write4bytes)
        .word \label\(_write5bytes)
        .word \label\(_write6bytes)
        .word \label\(_write7bytes)
        .word \label\(_write8bytes)
        .word \label\(_write9bytes)
        .word \label\(_write10bytes)
        .word \label\(_write11bytes)
        .word \label\(_write12bytes)
        .word \label\(_write13bytes)
        .word \label\(_write14bytes)
        .word \label\(_write15bytes)
        
\label\(_paddingOffset):
        b                   \label\(_writeData) 
        
\label\(_write1bytes): 
        vext.8              \reg0, \reg1, \reg0, #1
        vext.8              \reg0, \reg0, \reg1, #15  
        b                   \label\(_writeData) 
        
\label\(_write2bytes): 
        vext.8              \reg0, \reg1, \reg0, #2
        vext.8              \reg0, \reg0, \reg1, #14
        b                   \label\(_writeData)   
        
\label\(_write3bytes):
        vext.8              \reg0, \reg1, \reg0, #3 
        vext.8              \reg0, \reg0, \reg1, #13
        b                   \label\(_writeData)  
        
\label\(_write4bytes): 
        vext.8              \reg0, \reg1, \reg0, #4
        vext.8              \reg0, \reg0, \reg1, #12 
        b                   \label\(_writeData)  
        
\label\(_write5bytes): 
        vext.8              \reg0, \reg1, \reg0, #5
        vext.8              \reg0, \reg0, \reg1, #11
        b                   \label\(_writeData)   
        
\label\(_write6bytes): 
        vext.8              \reg0, \reg1, \reg0, #6
        vext.8              \reg0, \reg0, \reg1, #10
        b                   \label\(_writeData)    
        
\label\(_write7bytes): 
        vext.8              \reg0, \reg1, \reg0, #7
        vext.8              \reg0, \reg0, \reg1, #9
        b                   \label\(_writeData)    
        
\label\(_write8bytes): 
        vext.8              \reg0, \reg1, \reg0, #8
        vext.8              \reg0, \reg0, \reg1, #8 
        b                   \label\(_writeData)   
        
\label\(_write9bytes): 
        vext.8              \reg0, \reg1, \reg0, #9
        vext.8              \reg0, \reg0, \reg1, #7 
        b                   \label\(_writeData)   
        
\label\(_write10bytes): 
        vext.8              \reg0, \reg1, \reg0, #10
        vext.8              \reg0, \reg0, \reg1, #6 
        b                   \label\(_writeData)  
        
\label\(_write11bytes): 
        vext.8              \reg0, \reg1, \reg0, #11
        vext.8              \reg0, \reg0, \reg1, #5
        b                   \label\(_writeData)    
        
\label\(_write12bytes): 
        vext.8              \reg0, \reg1, \reg0, #12
        vext.8              \reg0, \reg0, \reg1, #4 
        b                   \label\(_writeData)       
        
\label\(_write13bytes):
        vext.8              \reg0, \reg1, \reg0, #13
        vext.8              \reg0, \reg0, \reg1, #3
        b                   \label\(_writeData)      
        
\label\(_write14bytes):
        vext.8              \reg0, \reg1, \reg0, #14
        vext.8              \reg0, \reg0, \reg1, #2
        b                   \label\(_writeData)  
        
\label\(_write15bytes):
        vext.8              \reg0, \reg1, \reg0, #15
        vext.8              \reg0, \reg0, \reg1, #1
        
\label\(_writeData):  
        sub                 \addr, \addr, \cnt 
        vst1.u8             {\reg0}, [\addr], \cnt 
        
        b                   \go_label  
        
        .endm   

@/************************************************************************
@   Registers for processSaoEo, global registers
@************************************************************************/
CONST_1                     .req q15
CONST_2                     .req q14

pSAO_m_iOffsetEo_val        .req d26
        
@/************************************************************************
@   VO_VOID processSaoEO0(VO_S32 iStartX, VO_S32 uiCurLCURowStartY, 
@                 VO_S32 iEndX, VO_S32 uiCurLCURowEndY, VO_U8* pRec, 
@                 VO_U32 uiStride, VO_U8 *pTmpL, VO_S8 *iOffsetEo)
@************************************************************************/
processSaoEO0_asm:      @function
        stmfd               sp!, {r4 - r11, lr}
        vmov.u8             CONST_1, #0x1
        vmov.u8             CONST_2, #0x2  

@input REGs
iStartX                     .req r0
uiCurLCURowStartY           .req r1
iEndX                       .req r2
uiCurLCURowEndY             .req r3
       
@global REGs
uiStride                    .req r4
pRec                        .req r11
pRecBak                     .req r9


xCntEO0                     .req r8
yCnt                        .req r3 

pTmpL                       .req r5

pRecLeft                    .req r6
pRecRight                   .req r7

processSaoCuOrg_asm_temp    .req r14
       
@Q REGs
pRec_val                    .req q0
pRec_val_0                  .req d0
pRec_val_1                  .req d1
pRecLeft_val                .req q1
pRecLeft_val_0              .req d2
pRecLeft_val_1              .req d3
pRecRight_val               .req q4
iSignLeft                   .req q2
uiEdgeType                  .req q3
uiEdgeType_0                .req d6
uiEdgeType_1                .req d7


pSAO_m_iOffsetEo_value      .req q5
pSAO_m_iOffsetEo_value_0    .req d10
pSAO_m_iOffsetEo_value_1    .req d11

pRec_val_new                .req q6
pRec_val_new_0              .req d12
pRec_val_new_1              .req d13
iSignRight                  .req q7
q_reg_temp                  .req q8


case_SAO_EO_0_yLoop_prepare:  

        sub                 yCnt, uiCurLCURowEndY, uiCurLCURowStartY  
        
        ldr                 pRec, [sp,#36]
        add                 pRec, pRec, iStartX
        mov                 pRecBak, pRec
        ldr                 uiStride, [sp,#40]     
    
        ldr                 pTmpL, [sp,#44]
        add                 pTmpL, pTmpL, uiCurLCURowStartY
        mov                 pRecBak, pRec
        
         @load table, pSAO->m_iOffsetEo       
        ldr                 processSaoCuOrg_asm_temp, [sp, #48]   
        vld1.32             pSAO_m_iOffsetEo_val[0], [processSaoCuOrg_asm_temp]! 
        vld1.8              pSAO_m_iOffsetEo_val[4], [processSaoCuOrg_asm_temp]!  
        
case_SAO_EO_0_yLoop: 
        sub                 xCntEO0, iEndX, iStartX            
     
        
        sub                 pRecLeft, pRec, #1 
        vld1.u8             {pRecLeft_val}, [pRecLeft]!
        vld1.u8             pRecLeft_val_0[0], [pTmpL]!
        
        add                 pRecRight, pRec, #1
        
case_SAO_EO_0_xLoop: 

        vld1.u8             {pRec_val}, [pRec]                          @load 16 bytes
        
        xSign               iSignLeft,pRec_val,pRecLeft_val,q8,q9
        
        vld1.u8             {pRecRight_val}, [pRecRight]!
     
        xSign               iSignRight, pRec_val, pRecRight_val, q8, q9       
 
        vadd.u8             uiEdgeType, iSignLeft, iSignRight        
        
        vtbl.8              pSAO_m_iOffsetEo_value_0,{pSAO_m_iOffsetEo_val},uiEdgeType_0
        vtbl.8              pSAO_m_iOffsetEo_value_1,{pSAO_m_iOffsetEo_val},uiEdgeType_1
        
        qAddUS              pRec_val_new_0, pRec_val_new_1, pRec_val_0, pRec_val_1, pSAO_m_iOffsetEo_value_0, pSAO_m_iOffsetEo_value_1, q8,q9
        
        subs                xCntEO0, xCntEO0, #16
        bge                 case_SAO_EO_0_16values
        add                 xCntEO0, xCntEO0, #16        
      
        writeQReg           EO_0, pRec_val_new, q_reg_temp, pRec, xCntEO0, processSaoCuOrg_asm_temp, case_SAO_EO_0_post_xLoop   
       
case_SAO_EO_0_16values:  
        vld1.u8             {pRecLeft_val}, [pRecLeft]!      
        vst1.u8             {pRec_val_new},[pRec]!                 @save pRec_val 
        
        bgt                 case_SAO_EO_0_xLoop           
       
case_SAO_EO_0_post_xLoop:         
      
        @pRec += uiStride@     
        add                 pRec, pRecBak, uiStride                  @pRec[iEndX-1 + uiStride]
        mov                 pRecBak, pRec
        
        subs                yCnt, yCnt, #1
        bgt                 case_SAO_EO_0_yLoop
        
        ldmfd               sp!, {r4 - r11, PC}                     @processSaoEO0_asm  
        @endp                                                        @@endp of processSaoEO0_asm

@/************************************************************************
@   VO_VOID processSaoEO1(VO_S32 iLcuWidth, VO_S32 iStartY, 
@                 VO_S32 iEndY, VO_U8* pRec, VO_U32 uiStride, 
@                 VO_U8 *pTmpBuff1, VO_S8 *iOffsetEo)
@************************************************************************/
processSaoEO1_asm:      @function
        stmfd               sp!, {r4 - r11, lr}
        
        vmov.u8             CONST_1, #0x1
        vmov.u8             CONST_2, #0x2 

@input REGs
iLcuWidth                   .req r0
iStartY                     .req r1
iEndYEO1                    .req r2
pRecEO1                     .req r3
 
@global REGs
uiStride                    .req r4
pRecNextRow                 .req r5

xCnt                        .req r2
yCntEO1                     .req r7 

pTmpUpBuff                  .req r6
pTmpUpBuffBak               .req r8

processSaoCuOrg_asm_temp    .req r14
 
@Q REGs       
pRec_val                    .req q0
pRec_val_0                  .req d0
pRec_val_1                  .req d1
pRecNextRow_val             .req q1
iSignDown                   .req q2
uiEdgeType                  .req q3
uiEdgeType_0                .req d6
uiEdgeType_1                .req d7

pSAO_m_iUpBuff1_val         .req q4
pSAO_m_iOffsetEo_value      .req q5
pSAO_m_iOffsetEo_value_0    .req d10
pSAO_m_iOffsetEo_value_1    .req d11

pRec_val_new                .req q6
pRec_val_new_0              .req d12
pRec_val_new_1              .req d13
q_reg_temp                  .req q8
     
case_SAO_EO_1_yLoop_prepare:   
      
        sub                 yCntEO1, iEndYEO1, iStartY         
        ldr                 uiStride, [sp,#36]     
    
        ldr                 pTmpUpBuff, [sp,#40]  
        mov                 pTmpUpBuffBak, pTmpUpBuff
        
        @load table, pSAO->m_iOffsetEo       
        ldr                 processSaoCuOrg_asm_temp, [sp, #44]   
        vld1.32             pSAO_m_iOffsetEo_val[0], [processSaoCuOrg_asm_temp]! 
        vld1.8              pSAO_m_iOffsetEo_val[4], [processSaoCuOrg_asm_temp]!  
        
case_SAO_EO_1_yLoop: 
        mov                 xCnt, iLcuWidth        
        add                 pRecNextRow, pRecEO1, uiStride
        
case_SAO_EO_1_xLoop: 

        vld1.u8             {pRec_val}, [pRecEO1]                          @load 16 bytes
        vld1.u8             {pRecNextRow_val}, [pRecNextRow]!
     
        xSign               iSignDown, pRec_val, pRecNextRow_val, q8, q9
        vld1.u8             {pSAO_m_iUpBuff1_val}, [pTmpUpBuff]
        vsub.u8             q8, CONST_2, iSignDown
        vadd.u8             uiEdgeType, iSignDown, pSAO_m_iUpBuff1_val
                
        vst1.u8             {q8},[pTmpUpBuff]!                     @save pSAO->m_iUpBuff1
        
        
        vtbl.8              pSAO_m_iOffsetEo_value_0,{pSAO_m_iOffsetEo_val},uiEdgeType_0
        vtbl.8              pSAO_m_iOffsetEo_value_1,{pSAO_m_iOffsetEo_val},uiEdgeType_1
        
        qAddUS              pRec_val_new_0, pRec_val_new_1, pRec_val_0, pRec_val_1, pSAO_m_iOffsetEo_value_0, pSAO_m_iOffsetEo_value_1, q8,q9
        
        subs                xCnt, xCnt, #16
        bge                 case_SAO_EO_1_16values
        add                 xCnt, xCnt, #16        
      
        writeQReg           EO_1, pRec_val_new, q_reg_temp, pRecEO1, xCnt, processSaoCuOrg_asm_temp, case_SAO_EO_1_post_xLoop   
       
case_SAO_EO_1_16values:      
        vst1.u8             {pRec_val_new},[pRecEO1]!                      @save 16 bytes  
        bgt                 case_SAO_EO_1_xLoop           
       
case_SAO_EO_1_post_xLoop:  
        @pRecEO1 += uiStride@
        add                 pRecEO1, pRecEO1, uiStride
        sub                 pRecEO1, pRecEO1, iLcuWidth
        
        mov                 pTmpUpBuff, pTmpUpBuffBak@
        
        subs                yCntEO1, yCntEO1, #1
        bgt                 case_SAO_EO_1_yLoop        
      
        ldmfd               sp!, {r4 - r11, PC}                     @processSaoEO1_asm  
        @endp                                                        @@endp of processSaoEO1_asm
        
@/************************************************************************
@   VO_VOID processSaoEO2(VO_S32 iStartX, VO_S32 iStartY, 
@                 VO_S32 iEndX, VO_S32 iEndY, VO_U8* pRec, VO_U32 uiStride, 
@                 VO_U8 *pTmpBuff1, VO_U8 *pTmpL, VO_S8 *iOffsetEo)
@************************************************************************/
processSaoEO2_asm:      @function
        stmfd               sp!, {r4 - r11, lr}
        vmov.u8             CONST_1, #0x1
        vmov.u8             CONST_2, #0x2  
        
@input REGs
iStartX                     .req r0
iStartY                     .req r1
iEndX                       .req r2
iEndY                       .req r3

@global REGs
uiStride                    .req r4
pRec                        .req r11
pRecBak                     .req r9
pRecNextRowAddOne           .req r10

xCnt                        .req r2
yCnt                        .req r3
xCntBak                     .req r0   

pTmpL                       .req r5

pTmpUpBuff                  .req r6
pSAO_m_iUpBuff1Add1         .req r7
pTmpUpBuffBak               .req r8

processSaoCuOrg_asm_temp    .req r14

@reused REGs
EO2TmpLValue                .req r2
       
@Q REGs
pRec_val                    .req q0
pRec_val_0                  .req d0
pRec_val_1                  .req d1
pRecNextRowAddOne_val       .req q1
iSignDown1                  .req q2
uiEdgeType                  .req q3
uiEdgeType_0                .req d6
uiEdgeType_1                .req d7

pSAO_m_iUpBuff1_val         .req q4
pSAO_m_iOffsetEo_value      .req q5
pSAO_m_iOffsetEo_value_0    .req d10
pSAO_m_iOffsetEo_value_1    .req d11

pRec_val_new                .req q6
pRec_val_new_0              .req d12
pRec_val_new_1              .req d13
q_reg_temp                  .req q8
     
case_SAO_EO_2_yLoop_prepare:   

      
        sub                 yCnt, iEndY, iStartY   
        
        ldr                 pRec, [sp,#36]
        add                 pRec, pRec, iStartX
        mov                 pRecBak, pRec
        ldr                 uiStride, [sp,#40]  
        
        ldr                 pTmpUpBuff, [sp,#44]
        add                 pTmpUpBuff, pTmpUpBuff, iStartX
        mov                 pTmpUpBuffBak, pTmpUpBuff
    
        
        ldr                 pTmpL, [sp,#48]
        add                 pTmpL, pTmpL, iStartY      
        
        sub                 xCnt, iEndX, iStartX   
        mov                 xCntBak, xCnt
        
        @load table, pSAO->m_iOffsetEo       
        ldr                 processSaoCuOrg_asm_temp, [sp, #52]   
        vld1.32             pSAO_m_iOffsetEo_val[0], [processSaoCuOrg_asm_temp]! 
        vld1.8              pSAO_m_iOffsetEo_val[4], [processSaoCuOrg_asm_temp]!    
        
case_SAO_EO_2_yLoop: 
       
        add                 pSAO_m_iUpBuff1Add1, pTmpUpBuff, #1
        vld1.u8             {pSAO_m_iUpBuff1_val}, [pTmpUpBuff]!            @load upbuffer 16 bytes
        
        add                 pRecNextRowAddOne, pRec, uiStride
        ldrb                processSaoCuOrg_asm_temp, [pRecNextRowAddOne],#1 
     
        ldrb                EO2TmpLValue, [pTmpL], #1       
        xSignSimple         processSaoCuOrg_asm_temp, processSaoCuOrg_asm_temp,EO2TmpLValue
        
        strb                processSaoCuOrg_asm_temp, [pTmpUpBuffBak]
        
        mov                 xCnt, xCntBak
        
case_SAO_EO_2_xLoop: 

        vld1.u8             {pRec_val}, [pRec]                          @load 16 bytes
        vld1.u8             {pRecNextRowAddOne_val}, [pRecNextRowAddOne]!
     
        xSign               iSignDown1,pRec_val,pRecNextRowAddOne_val,q8,q9
       
        vsub.u8             q8,CONST_2,iSignDown1
        vadd.u8             uiEdgeType, iSignDown1, pSAO_m_iUpBuff1_val
          
        vld1.u8             {pSAO_m_iUpBuff1_val}, [pTmpUpBuff]!        @load upbuffer 16 bytes
        vst1.u8             {q8},[pSAO_m_iUpBuff1Add1]!                 @save pSAO->m_iUpBuff
        
        vtbl.8              pSAO_m_iOffsetEo_value_0,{pSAO_m_iOffsetEo_val},uiEdgeType_0
        vtbl.8              pSAO_m_iOffsetEo_value_1,{pSAO_m_iOffsetEo_val},uiEdgeType_1
        
        qAddUS              pRec_val_new_0, pRec_val_new_1, pRec_val_0, pRec_val_1, pSAO_m_iOffsetEo_value_0, pSAO_m_iOffsetEo_value_1, q8,q9
        
        subs                xCnt, xCnt, #16
        bge                 case_SAO_EO_2_16values
        add                 xCnt, xCnt, #16
        
        writeQReg           EO_2, pRec_val_new, q_reg_temp, pRec, xCnt, processSaoCuOrg_asm_temp, case_SAO_EO_2_post_xLoop
     
       
case_SAO_EO_2_16values:        
        vst1.u8             {pRec_val_new},[pRec]!                 @save pRec_val  
        bgt                 case_SAO_EO_2_xLoop   
        
       
case_SAO_EO_2_post_xLoop:     

        mov                 pTmpUpBuff, pTmpUpBuffBak
        
        @pRec += uiStride@
        add                 pRec, pRecBak, uiStride                  @pRec[iEndX-1 + uiStride]
        mov                 pRecBak, pRec   
        
        subs                yCnt, yCnt, #1
        bgt                 case_SAO_EO_2_yLoop
        
        ldmfd               sp!, {r4 - r11, PC}                     @processSaoEO2_asm  
        @endp                                                        @@endp of processSaoEO2_asm
               
@/************************************************************************
@   VO_VOID processSaoEO3(VO_S32 iStartX, VO_S32 iStartY, 
@                 VO_S32 iEndX, VO_S32 iEndY, VO_U8* pRec, VO_U32 uiStride, 
@                 VO_U8 *pTmpBuff1, VO_U8 *pTmpL, VO_S8 *iOffsetEo)
@************************************************************************/
processSaoEO3_asm:      @function
        stmfd               sp!, {r4 - r11, lr}
        vmov.u8             CONST_1, #0x1
        vmov.u8             CONST_2, #0x2   

@input REGs
iStartX                     .req r0
iStartY                     .req r1
iEndX                       .req r2
iEndY                       .req r3

@global REGs
uiStride                    .req r4
pRec                        .req r11
pRecBak                     .req r9
pRecNextRowSubOne           .req r10
pRec_iEndX                  .req r12

xCnt                        .req r2
yCnt                        .req r3
xCntBak                     .req r0   

pTmpL                       .req r5

pTmpUpBuff                  .req r6
pSAO_m_iUpBuff1iEndXSub1    .req r7
pTmpUpBuffBak               .req r8

neg_one                     .req r1

processSaoCuOrg_asm_temp    .req r14
 
@Q REGs      
pRec_val                    .req q0
pRec_val_0                  .req d0
pRec_val_1                  .req d1
pRecNextRowSubOne_val       .req q1
pRecNextRowSubOne_val_0     .req d2
pRecNextRowSubOne_val_1     .req d3
iSignDown1                  .req q2
uiEdgeType                  .req q3
uiEdgeType_0                .req d6
uiEdgeType_1                .req d7

pSAO_m_iUpBuff1_val         .req q4
pSAO_m_iOffsetEo_value      .req q5
pSAO_m_iOffsetEo_value_0    .req d10
pSAO_m_iOffsetEo_value_1    .req d11

pRec_val_new                .req q6
pRec_val_new_0              .req d12
pRec_val_new_1              .req d13
q_reg_temp                  .req q8
    
case_SAO_EO_3_yLoop_prepare:  
      
        sub                 yCnt, iEndY, iStartY   
        
        ldr                 pRec, [sp,#36]
        add                 pRec, pRec, iStartX
        mov                 pRecBak, pRec
        ldr                 uiStride, [sp,#40]
     
        
        ldr                 pTmpUpBuff, [sp,#44]
        add                 pTmpUpBuff, pTmpUpBuff, iStartX
        mov                 pTmpUpBuffBak, pTmpUpBuff
        add                 pSAO_m_iUpBuff1iEndXSub1, pTmpUpBuff, iEndX   @ pTmpUpBuff = pTmpUpBuff[iStartX]    
        sub                 pSAO_m_iUpBuff1iEndXSub1, pSAO_m_iUpBuff1iEndXSub1, iStartX @ no need -1 now
        
        ldr                 pTmpL, [sp,#48]
        add                 pTmpL, pTmpL, iStartY
        add                 pTmpL, pTmpL, #1
       
        
        sub                 xCnt, iEndX, iStartX
        sub                 xCnt, xCnt, #1   
        mov                 xCntBak, xCnt        
        
        @load table, pSAO->m_iOffsetEo       
        ldr                 processSaoCuOrg_asm_temp, [sp, #52]   
        vld1.32             pSAO_m_iOffsetEo_val[0], [processSaoCuOrg_asm_temp]! 
        vld1.8              pSAO_m_iOffsetEo_val[4], [processSaoCuOrg_asm_temp]!  
        
        mov                 neg_one, #-1
        
case_SAO_EO_3_yLoop: 
        mov                 xCnt, xCntBak
        
        vld1.u8             {pRec_val}, [pRec]                           @load 16 bytes
        vld1.u8             {pRecNextRowSubOne_val_0[0]}, [pTmpL]!         @load 1 byte
        xSign               iSignDown1,pRec_val,pRecNextRowSubOne_val,q8,q9     @iSignDown1      =  xSign(pRec[x] - pTmpL[y+1]) @
        vld1.u8             {pSAO_m_iUpBuff1_val}, [pTmpUpBuff]
      
        vadd.u8             uiEdgeType, iSignDown1, pSAO_m_iUpBuff1_val  @uiEdgeType      =  iSignDown1 + pSAO->m_iUpBuff1[x] @
        
        vtbl.8              pSAO_m_iOffsetEo_value_0,{pSAO_m_iOffsetEo_val},uiEdgeType_0
        
        qAddUS              pRec_val_new_0, pRec_val_new_1, pRec_val_0, pRec_val_1, pSAO_m_iOffsetEo_value_0, pSAO_m_iOffsetEo_value_1, q8,q9        
       
        vst1.u8             pRec_val_new_0[0], [pRec]!                     @end, pRec[x] = pSAO->m_pClipTable[pRec[x] + pSAO->m_iOffsetEo[uiEdgeType]]@ 
        
        
        add                 pRecNextRowSubOne, pRec, uiStride
        sub                 pRecNextRowSubOne, pRecNextRowSubOne, #1
        
case_SAO_EO_3_xLoop: 

        vld1.u8             {pRec_val}, [pRec]                          @load 16 bytes
        vld1.u8             {pRecNextRowSubOne_val}, [pRecNextRowSubOne]!
        add                 pTmpUpBuff, pTmpUpBuff, #1
        xSign               iSignDown1,pRec_val,pRecNextRowSubOne_val,q8,q9
        vld1.u8             {pSAO_m_iUpBuff1_val}, [pTmpUpBuff], neg_one
        vsub.u8             q8,CONST_2,iSignDown1
        vadd.u8             uiEdgeType, iSignDown1, pSAO_m_iUpBuff1_val                  
        vst1.u8             {q8},[pTmpUpBuff]!                 @save pSAO->m_iUpBuff1
        
        
        vtbl.8              pSAO_m_iOffsetEo_value_0,{pSAO_m_iOffsetEo_val},uiEdgeType_0
        vtbl.8              pSAO_m_iOffsetEo_value_1,{pSAO_m_iOffsetEo_val},uiEdgeType_1
        
        qAddUS              pRec_val_new_0, pRec_val_new_1, pRec_val_0, pRec_val_1, pSAO_m_iOffsetEo_value_0, pSAO_m_iOffsetEo_value_1, q8,q9
        
        subs                xCnt, xCnt, #16
        bge                 case_SAO_EO_3_16values
        add                 xCnt, xCnt, #16
        
        writeQReg           EO_3, pRec_val_new, q_reg_temp, pRec, xCnt, processSaoCuOrg_asm_temp, case_SAO_EO_3_post_xLoop     
       
case_SAO_EO_3_16values:        
        vst1.u8             {pRec_val_new},[pRec]!                 @save pRec_val  
        bgt                 case_SAO_EO_3_xLoop           
       
case_SAO_EO_3_post_xLoop:       
        
        @pSAO->m_iUpBuff1[iEndX-1] = xSign(pRec[iEndX-1 + uiStride] - pRec[iEndX])@
        ldrb                pRec_iEndX, [pRec], uiStride
        ldrb                processSaoCuOrg_asm_temp, [pRec,#-1]!  @ 
        xSignSimple         processSaoCuOrg_asm_temp, processSaoCuOrg_asm_temp,pRec_iEndX
       
        strb                processSaoCuOrg_asm_temp, [pSAO_m_iUpBuff1iEndXSub1, #-1 ]  
        mov                 pTmpUpBuff, pTmpUpBuffBak
        
        @pRec += uiStride@
        add                 pRec, pRecBak, uiStride                  @pRec[iEndX-1 + uiStride]
        mov                 pRecBak, pRec
        
        subs                yCnt, yCnt, #1
        bgt                 case_SAO_EO_3_yLoop

        ldmfd               sp!, {r4 - r11, PC}                      @processSaoEO3_asm  
        @endp                                                        @@endp of processSaoEO3_asm
     
@/************************************************************************
@   VO_VOID processSaoBo(VO_S32 iLcuWidth, VO_S32 uiCurLCURowStartY, 
@                 VO_S32 uiCurLCURowEndY, VO_U8* pRec, VO_U32 uiStride, 
@                 VO_U8 *iOffsetBo)
@************************************************************************/
processSaoBo_asm:      @function
        stmfd               sp!, {r4 - r11, lr}
        
@input REGs
iLcuWidth                   .req r0
uiCurLCURowStartY           .req r1
uiCurLCURowEndYBo           .req r2
pRecBo                      .req r3

@global REGs
uiStride                    .req r4       
xCntBo                      .req r7
yCntBo                      .req r8

pSAO_m_iOffsetBo            .req r5
pRec_val_simple             .req r6
processSaoCuOrg_asm_temp    .req r14

        ldr                 uiStride, [sp,#36]
      
        ldr                 pSAO_m_iOffsetBo, [sp,#40]     
              
        sub                 yCntBo, uiCurLCURowEndYBo, uiCurLCURowStartY     

case_SAO_BO_yLoop: 
        mov                 xCntBo, iLcuWidth

case_SAO_BO_xLoop: 

        ldrb                pRec_val_simple, [pRecBo]           
        ldrb                processSaoCuOrg_asm_temp, [pSAO_m_iOffsetBo,pRec_val_simple]        
        strb                processSaoCuOrg_asm_temp, [pRecBo], #1

        subs                xCntBo, xCntBo, #1
        bgt                 case_SAO_BO_xLoop
        
        add                 pRecBo, pRecBo, uiStride
        sub                 pRecBo, pRecBo, iLcuWidth
        
        subs                yCntBo, yCntBo, #1
        bgt                 case_SAO_BO_yLoop

        ldmfd               sp!, {r4 - r11, PC}                     @processSaoBo_asm  
        @endp                                                       @endp of processSaoEO3_asm
                                                                 
        .endif                                                      @end of if SAO_ASM_ENABLED==1 
        .end                                                        @ Mark end of file
    
