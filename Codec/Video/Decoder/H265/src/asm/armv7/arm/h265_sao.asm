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
    include		h265dec_ASM_config.h

    area |.text|, CODE, READONLY, ALIGN=4
    
    if SAO_ASM_ENABLED==1
    
    export processSaoEO0_asm
    export processSaoEO1_asm    
    export processSaoEO2_asm   
    export processSaoEO3_asm   
    export processSaoBo_asm
   
    
;/************************************************************************
; MACRO:            qAddUS
; function:         $output = sat($input1+$input2), output!=input1!=input2
; args:             $output,unsigned   $input1,signed   $input2,unsigned
; regs:             $temp1, $temp2, $temp3  
;************************************************************************/
        macro
$label  qAddUS               $output_0, $output_1, $input1_0, $input1_1, $input2_0, $input2_1, $temp1, $temp2  

        vmovl.u8            $temp1, $input1_0
        vmovl.u8            $temp2, $input1_1
        vaddw.s8            $temp1, $temp1, $input2_0
        vaddw.s8            $temp2, $temp2, $input2_1
      
        vqmovun.s16         $output_0,  $temp1
        vqmovun.s16         $output_1,  $temp2
        
        mend
        
;/************************************************************************
; MACRO:            xSign
; function:         $input1 < $input2, return 0; =, return 1; >, return 2
; args:             $
; regs:             $temp1, $temp2, 
;************************************************************************/       
        macro
$label  xSign               $output, $input1, $input2, $temp1, $temp2 
        
        vcgt.u8             $temp1,$input1,$input2
        vand.u8             $temp1,$temp1,CONST_1
        vcge.u8             $temp2,$input1,$input2 
        vand.u8             $temp2,$temp2,CONST_1      
        vadd.u8             $output,$temp1,$temp2    
        
        mend
        
;/************************************************************************
; MACRO:            xSignSimple
; function:         $input1 < $input2, return 0; =, return 1; >, return 2
; args:             $
; regs:             $temp1, $temp2, 
;************************************************************************/      
        macro
$label  xSignSimple               $output, $input1, $input2
        
        subs                $input1,$input2
        movlt               $output,#0
        moveq               $output,#1      
        movgt               $output,#2
                
        mend
        
;/************************************************************************
; MACRO:            writeQReg
; function:         cnt is between 1 to 15, including 1 and 15
; args:             $
; regs:             $temp1, $temp2, 
;************************************************************************/
        macro 
$label  writeQReg          $reg0, $reg1, $addr, $cnt, $tmp, $go_label

        add                 $addr, $addr, $cnt
        vld1.8              {$reg1}, [$addr]        

     	subs				$tmp, $cnt, #15
        beq					$label.write15bytes
        
        subs				$tmp, $cnt, #14
        beq					$label.write14bytes
        
        subs				$tmp, $cnt, #13
        beq					$label.write13bytes
        
        subs				$tmp, $cnt, #12
        beq					$label.write12bytes
        
        subs				$tmp, $cnt, #11
        beq					$label.write11bytes
        
        subs				$tmp, $cnt, #10
        beq					$label.write10bytes
        
        subs				$tmp, $cnt, #9
        beq					$label.write9bytes
        
        subs				$tmp, $cnt, #8
        beq					$label.write8bytes
        
        subs				$tmp, $cnt, #7
        beq					$label.write7bytes
        
        subs				$tmp, $cnt, #6
        beq					$label.write6bytes
        
        subs				$tmp, $cnt, #5
        beq					$label.write5bytes
        
        subs				$tmp, $cnt, #4
        beq					$label.write4bytes
        
        subs				$tmp, $cnt, #3
        beq					$label.write3bytes
        
        subs				$tmp, $cnt, #2
        beq					$label.write2bytes
        
        subs				$tmp, $cnt, #1
        beq					$label.write1bytes  
        
$label.paddingOffset
        b                   $label.writeData 
        
$label.write1bytes 
        vext.8              $reg0, $reg1, $reg0, #1
        vext.8              $reg0, $reg0, $reg1, #15  
        b                   $label.writeData 
        
$label.write2bytes 
        vext.8              $reg0, $reg1, $reg0, #2
        vext.8              $reg0, $reg0, $reg1, #14
        b                   $label.writeData   
        
$label.write3bytes
        vext.8              $reg0, $reg1, $reg0, #3 
        vext.8              $reg0, $reg0, $reg1, #13
        b                   $label.writeData  
        
$label.write4bytes 
        vext.8              $reg0, $reg1, $reg0, #4
        vext.8              $reg0, $reg0, $reg1, #12 
        b                   $label.writeData  
        
$label.write5bytes 
        vext.8              $reg0, $reg1, $reg0, #5
        vext.8              $reg0, $reg0, $reg1, #11
        b                   $label.writeData   
        
$label.write6bytes 
        vext.8              $reg0, $reg1, $reg0, #6
        vext.8              $reg0, $reg0, $reg1, #10
        b                   $label.writeData    
        
$label.write7bytes 
        vext.8              $reg0, $reg1, $reg0, #7
        vext.8              $reg0, $reg0, $reg1, #9
        b                   $label.writeData    
        
$label.write8bytes 
        vext.8              $reg0, $reg1, $reg0, #8
        vext.8              $reg0, $reg0, $reg1, #8 
        b                   $label.writeData   
        
$label.write9bytes 
        vext.8              $reg0, $reg1, $reg0, #9
        vext.8              $reg0, $reg0, $reg1, #7 
        b                   $label.writeData   
        
$label.write10bytes 
        vext.8              $reg0, $reg1, $reg0, #10
        vext.8              $reg0, $reg0, $reg1, #6 
        b                   $label.writeData  
        
$label.write11bytes 
        vext.8              $reg0, $reg1, $reg0, #11
        vext.8              $reg0, $reg0, $reg1, #5
        b                   $label.writeData    
        
$label.write12bytes 
        vext.8              $reg0, $reg1, $reg0, #12
        vext.8              $reg0, $reg0, $reg1, #4 
        b                   $label.writeData       
        
$label.write13bytes
        vext.8              $reg0, $reg1, $reg0, #13
        vext.8              $reg0, $reg0, $reg1, #3
        b                   $label.writeData      
        
$label.write14bytes
        vext.8              $reg0, $reg1, $reg0, #14
        vext.8              $reg0, $reg0, $reg1, #2
        b                   $label.writeData  
        
$label.write15bytes
        vext.8              $reg0, $reg1, $reg0, #15
        vext.8              $reg0, $reg0, $reg1, #1
        
$label.writeData  
        sub                 $addr, $addr, $cnt 
        vst1.u8             {$reg0}, [$addr], $cnt 
        
        b                   $go_label  
        
        mend 
        
;/************************************************************************
;   Registers for processSaoEo, global registers
;************************************************************************/
CONST_1                     qn 15
CONST_2                     qn 14

pSAO_m_iOffsetEo_val        dn  26
        
;/************************************************************************
;   VO_VOID processSaoEO0(VO_S32 iStartX, VO_S32 uiCurLCURowStartY, 
;                 VO_S32 iEndX, VO_S32 uiCurLCURowEndY, VO_U8* pRec, 
;                 VO_U32 uiStride, VO_U8 *pTmpL, VO_S8 *iOffsetEo)
;************************************************************************/
processSaoEO0_asm      PROC
        stmfd               sp!, {r4 - r11, lr}
        vmov.u8             CONST_1, #0x1
        vmov.u8             CONST_2, #0x2  

;input REGs
iStartX                     rn 0
uiCurLCURowStartY           rn 1
iEndX                       rn 2
uiCurLCURowEndY             rn 3
       
;global REGs
uiStride                    rn 4
pRec                        rn 11
pRecBak                     rn 9


xCntEO0                     rn 8
yCnt                        rn 3 

pTmpL                       rn 5

pRecLeft                    rn 6
pRecRight                   rn 7

processSaoCuOrg_asm_temp    rn 14
       
;Q REGs
pRec_val                    qn 0
pRec_val_0                  dn 0
pRec_val_1                  dn 1
pRecLeft_val                qn 1
pRecLeft_val_0              dn 2
pRecLeft_val_1              dn 3
pRecRight_val               qn 4
iSignLeft                   qn 2
uiEdgeType                  qn 3
uiEdgeType_0                dn 6
uiEdgeType_1                dn 7


pSAO_m_iOffsetEo_value      qn 5
pSAO_m_iOffsetEo_value_0    dn 10
pSAO_m_iOffsetEo_value_1    dn 11

pRec_val_new                qn 6
pRec_val_new_0              dn 12
pRec_val_new_1              dn 13
iSignRight                  qn 7
q_reg_temp                  qn 8


case_SAO_EO_0_yLoop_prepare  

        sub                 yCnt, uiCurLCURowEndY, uiCurLCURowStartY  
        
        ldr                 pRec, [sp,#36]
        add                 pRec, pRec, iStartX
        mov                 pRecBak, pRec
        ldr                 uiStride, [sp,#40]     
    
        ldr                 pTmpL, [sp,#44]
        add                 pTmpL, pTmpL, uiCurLCURowStartY
        mov                 pRecBak, pRec
        
         ;load table, pSAO->m_iOffsetEo       
        ldr                 processSaoCuOrg_asm_temp, [sp, #48]   
        vld1.32             pSAO_m_iOffsetEo_val[0], [processSaoCuOrg_asm_temp]! 
        vld1.8              pSAO_m_iOffsetEo_val[4], [processSaoCuOrg_asm_temp]!  
        
case_SAO_EO_0_yLoop 
        sub                 xCntEO0, iEndX, iStartX            
     
        
        sub                 pRecLeft, pRec, #1 
        vld1.u8             {pRecLeft_val}, [pRecLeft]!
        vld1.u8             pRecLeft_val_0[0], [pTmpL]!
        
        add                 pRecRight, pRec, #1
        
case_SAO_EO_0_xLoop 

        vld1.u8             {pRec_val}, [pRec]                          ;load 16 bytes
        
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
      
EO_0    writeQReg           pRec_val_new, q_reg_temp, pRec, xCntEO0, processSaoCuOrg_asm_temp, case_SAO_EO_0_post_xLoop   
       
case_SAO_EO_0_16values  
        vld1.u8             {pRecLeft_val}, [pRecLeft]!      
        vst1.u8             {pRec_val_new},[pRec]!                 ;save pRec_val 
        
        bgt                 case_SAO_EO_0_xLoop           
       
case_SAO_EO_0_post_xLoop         
      
        ;pRec += uiStride;     
        add                 pRec, pRecBak, uiStride                  ;pRec[iEndX-1 + uiStride]
        mov                 pRecBak, pRec
        
        subs                yCnt, yCnt, #1
        bgt                 case_SAO_EO_0_yLoop
        
        ldmfd               sp!, {r4 - r11, PC}                     ;processSaoEO0_asm  
        endp                                                        ;endp of processSaoEO0_asm

;/************************************************************************
;   VO_VOID processSaoEO1(VO_S32 iLcuWidth, VO_S32 iStartY, 
;                 VO_S32 iEndY, VO_U8* pRec, VO_U32 uiStride, 
;                 VO_U8 *pTmpBuff1, VO_S8 *iOffsetEo)
;************************************************************************/
processSaoEO1_asm      PROC
        stmfd               sp!, {r4 - r11, lr}
        
        vmov.u8             CONST_1, #0x1
        vmov.u8             CONST_2, #0x2 

;input REGs
iLcuWidth                   rn 0
iStartY                     rn 1
iEndYEO1                    rn 2
pRecEO1                     rn 3
 
;global REGs
uiStride                    rn 4
pRecNextRow                 rn 5

xCnt                        rn 2
yCntEO1                     rn 7 

pTmpUpBuff                  rn 6
pTmpUpBuffBak               rn 8

processSaoCuOrg_asm_temp    rn 14
 
;Q REGs       
pRec_val                    qn 0
pRec_val_0                  dn 0
pRec_val_1                  dn 1
pRecNextRow_val             qn 1
iSignDown                   qn 2
uiEdgeType                  qn 3
uiEdgeType_0                dn 6
uiEdgeType_1                dn 7

pSAO_m_iUpBuff1_val         qn 4
pSAO_m_iOffsetEo_value      qn 5
pSAO_m_iOffsetEo_value_0    dn 10
pSAO_m_iOffsetEo_value_1    dn 11

pRec_val_new                qn 6
pRec_val_new_0              dn 12
pRec_val_new_1              dn 13
q_reg_temp                  qn 8
     
case_SAO_EO_1_yLoop_prepare   
      
        sub                 yCntEO1, iEndYEO1, iStartY         
        ldr                 uiStride, [sp,#36]     
    
        ldr                 pTmpUpBuff, [sp,#40]  
        mov                 pTmpUpBuffBak, pTmpUpBuff
        
        ;load table, pSAO->m_iOffsetEo       
        ldr                 processSaoCuOrg_asm_temp, [sp, #44]   
        vld1.32             pSAO_m_iOffsetEo_val[0], [processSaoCuOrg_asm_temp]! 
        vld1.8              pSAO_m_iOffsetEo_val[4], [processSaoCuOrg_asm_temp]!  
        
case_SAO_EO_1_yLoop 
        mov                 xCnt, iLcuWidth        
        add                 pRecNextRow, pRecEO1, uiStride
        
case_SAO_EO_1_xLoop 

        vld1.u8             {pRec_val}, [pRecEO1]                          ;load 16 bytes
        vld1.u8             {pRecNextRow_val}, [pRecNextRow]!
     
        xSign               iSignDown, pRec_val, pRecNextRow_val, q8, q9
        vld1.u8             {pSAO_m_iUpBuff1_val}, [pTmpUpBuff]
        vsub.u8             q8, CONST_2, iSignDown
        vadd.u8             uiEdgeType, iSignDown, pSAO_m_iUpBuff1_val
                
        vst1.u8             {q8},[pTmpUpBuff]!                     ;save pSAO->m_iUpBuff1
        
        
        vtbl.8              pSAO_m_iOffsetEo_value_0,{pSAO_m_iOffsetEo_val},uiEdgeType_0
        vtbl.8              pSAO_m_iOffsetEo_value_1,{pSAO_m_iOffsetEo_val},uiEdgeType_1
        
        qAddUS              pRec_val_new_0, pRec_val_new_1, pRec_val_0, pRec_val_1, pSAO_m_iOffsetEo_value_0, pSAO_m_iOffsetEo_value_1, q8,q9
        
        subs                xCnt, xCnt, #16
        bge                 case_SAO_EO_1_16values
        add                 xCnt, xCnt, #16        
      
EO_1    writeQReg           pRec_val_new, q_reg_temp, pRecEO1, xCnt, processSaoCuOrg_asm_temp, case_SAO_EO_1_post_xLoop   
       
case_SAO_EO_1_16values      
        vst1.u8             {pRec_val_new},[pRecEO1]!                      ;save 16 bytes  
        bgt                 case_SAO_EO_1_xLoop           
       
case_SAO_EO_1_post_xLoop  
        ;pRecEO1 += uiStride;
        add                 pRecEO1, pRecEO1, uiStride
        sub                 pRecEO1, pRecEO1, iLcuWidth
        
        mov                 pTmpUpBuff, pTmpUpBuffBak;
        
        subs                yCntEO1, yCntEO1, #1
        bgt                 case_SAO_EO_1_yLoop        
      
        ldmfd               sp!, {r4 - r11, PC}                     ;processSaoEO1_asm  
        endp                                                        ;endp of processSaoEO1_asm
 
;/************************************************************************
;   VO_VOID processSaoEO2(VO_S32 iStartX, VO_S32 iStartY, 
;                 VO_S32 iEndX, VO_S32 iEndY, VO_U8* pRec, VO_U32 uiStride, 
;                 VO_U8 *pTmpBuff1, VO_U8 *pTmpL, VO_S8 *iOffsetEo)
;************************************************************************/
processSaoEO2_asm      PROC
        stmfd               sp!, {r4 - r11, lr}
        vmov.u8             CONST_1, #0x1
        vmov.u8             CONST_2, #0x2  
        
;input REGs
iStartX                     rn 0
iStartY                     rn 1
iEndX                       rn 2
iEndY                       rn 3

;global REGs
uiStride                    rn 4
pRec                        rn 11
pRecBak                     rn 9
pRecNextRowAddOne           rn 10

xCnt                        rn 2
yCnt                        rn 3
xCntBak                     rn 0   

pTmpL                       rn 5

pTmpUpBuff                  rn 6
pSAO_m_iUpBuff1Add1         rn 7
pTmpUpBuffBak               rn 8

processSaoCuOrg_asm_temp    rn 14

;reused REGs
EO2TmpLValue                rn 2
       
;Q REGs
pRec_val                    qn 0
pRec_val_0                  dn 0
pRec_val_1                  dn 1
pRecNextRowAddOne_val       qn 1
iSignDown1                  qn 2
uiEdgeType                  qn 3
uiEdgeType_0                dn 6
uiEdgeType_1                dn 7

pSAO_m_iUpBuff1_val         qn 4
pSAO_m_iOffsetEo_value      qn 5
pSAO_m_iOffsetEo_value_0    dn 10
pSAO_m_iOffsetEo_value_1    dn 11

pRec_val_new                qn 6
pRec_val_new_0              dn 12
pRec_val_new_1              dn 13
q_reg_temp                  qn 8
     
case_SAO_EO_2_yLoop_prepare   

      
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
        
        ;load table, pSAO->m_iOffsetEo       
        ldr                 processSaoCuOrg_asm_temp, [sp, #52]   
        vld1.32             pSAO_m_iOffsetEo_val[0], [processSaoCuOrg_asm_temp]! 
        vld1.8              pSAO_m_iOffsetEo_val[4], [processSaoCuOrg_asm_temp]!    
        
case_SAO_EO_2_yLoop 
       
        add                 pSAO_m_iUpBuff1Add1, pTmpUpBuff, #1
        vld1.u8             {pSAO_m_iUpBuff1_val}, [pTmpUpBuff]!            ;load upbuffer 16 bytes
        
        add                 pRecNextRowAddOne, pRec, uiStride
        ldrb                processSaoCuOrg_asm_temp, [pRecNextRowAddOne],#1 
     
        ldrb                EO2TmpLValue, [pTmpL], #1       
        xSignSimple         processSaoCuOrg_asm_temp, processSaoCuOrg_asm_temp,EO2TmpLValue
        
        strb                processSaoCuOrg_asm_temp, [pTmpUpBuffBak]
        
        mov                 xCnt, xCntBak
        
case_SAO_EO_2_xLoop 

        vld1.u8             {pRec_val}, [pRec]                          ;load 16 bytes
        vld1.u8             {pRecNextRowAddOne_val}, [pRecNextRowAddOne]!
     
        xSign               iSignDown1,pRec_val,pRecNextRowAddOne_val,q8,q9
       
        vsub.u8             q8,CONST_2,iSignDown1
        vadd.u8             uiEdgeType, iSignDown1, pSAO_m_iUpBuff1_val
          
        vld1.u8             {pSAO_m_iUpBuff1_val}, [pTmpUpBuff]!        ;load upbuffer 16 bytes
        vst1.u8             {q8},[pSAO_m_iUpBuff1Add1]!                 ;save pSAO->m_iUpBuff
        
        vtbl.8              pSAO_m_iOffsetEo_value_0,{pSAO_m_iOffsetEo_val},uiEdgeType_0
        vtbl.8              pSAO_m_iOffsetEo_value_1,{pSAO_m_iOffsetEo_val},uiEdgeType_1
        
        qAddUS              pRec_val_new_0, pRec_val_new_1, pRec_val_0, pRec_val_1, pSAO_m_iOffsetEo_value_0, pSAO_m_iOffsetEo_value_1, q8,q9
        
        subs                xCnt, xCnt, #16
        bge                 case_SAO_EO_2_16values
        add                 xCnt, xCnt, #16
        
EO_2    writeQReg           pRec_val_new, q_reg_temp, pRec, xCnt, processSaoCuOrg_asm_temp, case_SAO_EO_2_post_xLoop
     
       
case_SAO_EO_2_16values        
        vst1.u8             {pRec_val_new},[pRec]!                 ;save pRec_val  
        bgt                 case_SAO_EO_2_xLoop   
        
       
case_SAO_EO_2_post_xLoop     

        mov                 pTmpUpBuff, pTmpUpBuffBak
        
        ;pRec += uiStride;
        add                 pRec, pRecBak, uiStride                  ;pRec[iEndX-1 + uiStride]
        mov                 pRecBak, pRec   
        
        subs                yCnt, yCnt, #1
        bgt                 case_SAO_EO_2_yLoop
        
        ldmfd               sp!, {r4 - r11, PC}                     ;processSaoEO2_asm  
        endp                                                        ;endp of processSaoEO2_asm
                   
;/************************************************************************
;   VO_VOID processSaoEO3(VO_S32 iStartX, VO_S32 iStartY, 
;                 VO_S32 iEndX, VO_S32 iEndY, VO_U8* pRec, VO_U32 uiStride, 
;                 VO_U8 *pTmpBuff1, VO_U8 *pTmpL, VO_S8 *iOffsetEo)
;************************************************************************/
processSaoEO3_asm      PROC
        stmfd               sp!, {r4 - r11, lr}
        vmov.u8             CONST_1, #0x1
        vmov.u8             CONST_2, #0x2   

;input REGs
iStartX                     rn 0
iStartY                     rn 1
iEndX                       rn 2
iEndY                       rn 3

;global REGs
uiStride                    rn 4
pRec                        rn 11
pRecBak                     rn 9
pRecNextRowSubOne           rn 10
pRec_iEndX                  rn 12

xCnt                        rn 2
yCnt                        rn 3
xCntBak                     rn 0   

pTmpL                       rn 5

pTmpUpBuff                  rn 6
pSAO_m_iUpBuff1iEndXSub1    rn 7
pTmpUpBuffBak               rn 8

neg_one                     rn 1

processSaoCuOrg_asm_temp    rn 14
 
;Q REGs      
pRec_val                    qn 0
pRec_val_0                  dn 0
pRec_val_1                  dn 1
pRecNextRowSubOne_val       qn 1
pRecNextRowSubOne_val_0     dn 2
pRecNextRowSubOne_val_1     dn 3
iSignDown1                  qn 2
uiEdgeType                  qn 3
uiEdgeType_0                dn 6
uiEdgeType_1                dn 7

pSAO_m_iUpBuff1_val         qn 4
pSAO_m_iOffsetEo_value      qn 5
pSAO_m_iOffsetEo_value_0    dn 10
pSAO_m_iOffsetEo_value_1    dn 11

pRec_val_new                qn 6
pRec_val_new_0              dn 12
pRec_val_new_1              dn 13
q_reg_temp                  qn 8
    
case_SAO_EO_3_yLoop_prepare  
      
        sub                 yCnt, iEndY, iStartY   
        
        ldr                 pRec, [sp,#36]
        add                 pRec, pRec, iStartX
        mov                 pRecBak, pRec
        ldr                 uiStride, [sp,#40]
     
        
        ldr                 pTmpUpBuff, [sp,#44]
        add                 pTmpUpBuff, pTmpUpBuff, iStartX
        mov                 pTmpUpBuffBak, pTmpUpBuff
        add                 pSAO_m_iUpBuff1iEndXSub1, pTmpUpBuff, iEndX   ; pTmpUpBuff = pTmpUpBuff[iStartX]    
        sub                 pSAO_m_iUpBuff1iEndXSub1, pSAO_m_iUpBuff1iEndXSub1, iStartX ; no need -1 now
        
        ldr                 pTmpL, [sp,#48]
        add                 pTmpL, pTmpL, iStartY
        add                 pTmpL, pTmpL, #1
       
        
        sub                 xCnt, iEndX, iStartX
        sub                 xCnt, xCnt, #1   
        mov                 xCntBak, xCnt        
        
         ;load table, pSAO->m_iOffsetEo       
        ldr                 processSaoCuOrg_asm_temp, [sp, #52]   
        vld1.32             pSAO_m_iOffsetEo_val[0], [processSaoCuOrg_asm_temp]! 
        vld1.8              pSAO_m_iOffsetEo_val[4], [processSaoCuOrg_asm_temp]!  
        
        mov                 neg_one, #-1
        
case_SAO_EO_3_yLoop 
        mov                 xCnt, xCntBak
        
        vld1.u8             {pRec_val}, [pRec]                           ;load 16 bytes
        vld1.u8             {pRecNextRowSubOne_val_0[0]}, [pTmpL]!         ;load 1 byte
        xSign               iSignDown1,pRec_val,pRecNextRowSubOne_val,q8,q9     ;iSignDown1      =  xSign(pRec[x] - pTmpL[y+1]) ;
        vld1.u8             {pSAO_m_iUpBuff1_val}, [pTmpUpBuff]
      
        vadd.u8             uiEdgeType, iSignDown1, pSAO_m_iUpBuff1_val  ;uiEdgeType      =  iSignDown1 + pSAO->m_iUpBuff1[x] ;
        
        vtbl.8              pSAO_m_iOffsetEo_value_0,{pSAO_m_iOffsetEo_val},uiEdgeType_0
        
        qAddUS              pRec_val_new_0, pRec_val_new_1, pRec_val_0, pRec_val_1, pSAO_m_iOffsetEo_value_0, pSAO_m_iOffsetEo_value_1, q8,q9        
       
        vst1.u8             pRec_val_new_0[0], [pRec]!                     ;end, pRec[x] = pSAO->m_pClipTable[pRec[x] + pSAO->m_iOffsetEo[uiEdgeType]]; 
        
        
        add                 pRecNextRowSubOne, pRec, uiStride
        sub                 pRecNextRowSubOne, pRecNextRowSubOne, #1
        
case_SAO_EO_3_xLoop 

        vld1.u8             {pRec_val}, [pRec]                          ;load 16 bytes
        vld1.u8             {pRecNextRowSubOne_val}, [pRecNextRowSubOne]!
        add                 pTmpUpBuff, pTmpUpBuff, #1
        xSign               iSignDown1,pRec_val,pRecNextRowSubOne_val,q8,q9
        vld1.u8             {pSAO_m_iUpBuff1_val}, [pTmpUpBuff], neg_one
        vsub.u8             q8,CONST_2,iSignDown1
        vadd.u8             uiEdgeType, iSignDown1, pSAO_m_iUpBuff1_val                  
        vst1.u8             {q8},[pTmpUpBuff]!                 ;save pSAO->m_iUpBuff1
        
        
        vtbl.8              pSAO_m_iOffsetEo_value_0,{pSAO_m_iOffsetEo_val},uiEdgeType_0
        vtbl.8              pSAO_m_iOffsetEo_value_1,{pSAO_m_iOffsetEo_val},uiEdgeType_1
        
        qAddUS              pRec_val_new_0, pRec_val_new_1, pRec_val_0, pRec_val_1, pSAO_m_iOffsetEo_value_0, pSAO_m_iOffsetEo_value_1, q8,q9
        
        subs                xCnt, xCnt, #16
        bge                 case_SAO_EO_3_16values
        add                 xCnt, xCnt, #16
        
EO_3    writeQReg           pRec_val_new, q_reg_temp, pRec, xCnt, processSaoCuOrg_asm_temp, case_SAO_EO_3_post_xLoop     
       
case_SAO_EO_3_16values        
        vst1.u8             {pRec_val_new},[pRec]!                 ;save pRec_val  
        bgt                 case_SAO_EO_3_xLoop           
       
case_SAO_EO_3_post_xLoop       
        
        ;pSAO->m_iUpBuff1[iEndX-1] = xSign(pRec[iEndX-1 + uiStride] - pRec[iEndX]);
        ldrb                pRec_iEndX, [pRec], uiStride
        ldrb                processSaoCuOrg_asm_temp, [pRec,#-1]!  ; 
        xSignSimple         processSaoCuOrg_asm_temp, processSaoCuOrg_asm_temp,pRec_iEndX
       
        strb                processSaoCuOrg_asm_temp, [pSAO_m_iUpBuff1iEndXSub1, #-1 ]  
        mov                 pTmpUpBuff, pTmpUpBuffBak
        
        ;pRec += uiStride;
        add                 pRec, pRecBak, uiStride                  ;pRec[iEndX-1 + uiStride]
        mov                 pRecBak, pRec
        
        subs                yCnt, yCnt, #1
        bgt                 case_SAO_EO_3_yLoop

        ldmfd               sp!, {r4 - r11, PC}                     ;processSaoEO3_asm  
        endp                                                        ;endp of processSaoEO3_asm
            
;/************************************************************************
;   VO_VOID processSaoBo(VO_S32 iLcuWidth, VO_S32 uiCurLCURowStartY, 
;                 VO_S32 uiCurLCURowEndY, VO_U8* pRec, VO_U32 uiStride, 
;                 VO_U8 *iOffsetBo)
;************************************************************************/
processSaoBo_asm      PROC
        stmfd               sp!, {r4 - r11, lr}
        
;input REGs
iLcuWidth                   rn 0
uiCurLCURowStartY           rn 1
uiCurLCURowEndYBo           rn 2
pRecBo                      rn 3

;global REGs
uiStride                    rn 4       
xCntBo                      rn 7
yCntBo                      rn 8

pSAO_m_iOffsetBo            rn 5
pRec_val_simple             rn 6
processSaoCuOrg_asm_temp    rn 14

        ldr                 uiStride, [sp,#36]
      
        ldr                 pSAO_m_iOffsetBo, [sp,#40]     
              
        sub                 yCntBo, uiCurLCURowEndYBo, uiCurLCURowStartY     

case_SAO_BO_yLoop 
        mov                 xCntBo, iLcuWidth

case_SAO_BO_xLoop 

        ldrb                pRec_val_simple, [pRecBo]           
        ldrb                processSaoCuOrg_asm_temp, [pSAO_m_iOffsetBo,pRec_val_simple]        
        strb                processSaoCuOrg_asm_temp, [pRecBo], #1

        subs                xCntBo, xCntBo, #1
        bgt                 case_SAO_BO_xLoop
        
        add                 pRecBo, pRecBo, uiStride
        sub                 pRecBo, pRecBo, iLcuWidth
        
        subs                yCntBo, yCntBo, #1
        bgt                 case_SAO_BO_yLoop

        ldmfd               sp!, {r4 - r11, PC}                     ;processSaoBo_asm  
        endp                                                        ;endp of processSaoBo_asm 
        
        endif														; end of SAO_ASM_ENABLED==1
        end                                                         ; Mark end of file
    