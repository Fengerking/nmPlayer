;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;* File Name: 
;*            amrnb_vq_subvec_neon.s
;* Description: 
;*            This module implements the Vq_subvec().
;* Functions Included:
;*            1. Word16 Vq_subvec()
;*
;***************************** Change History**************************
;* 
;*    DD/MMM/YYYY     Code Ver     Description             Author
;*    -----------     --------     -----------             ------
;*    08-12-2008        1.0        File imported from      Huaping Liu
;*                                             
;**********************************************************************
;**************************
; constant
;**************************
LSF_R1              EQU             0
LSF_R2              EQU             LSF_R1+4
DICO                EQU             LSF_R2+4
STACK_USED          EQU             DICO+4
WF2                 EQU             STACK_USED+4*9
DICO_SIZE           EQU             WF2+4

;**************************
;ARM Register
;**************************
; *lsf_r1                RN               r0
; *lsf_r2                RN               r1
; *dico                  RN               r2
; *wf1                   RN               r3
; *f2                    RN               r4
; *dico_size             RN               r5
     
        AREA    |.text|, CODE, READONLY
        EXPORT  Vq_subvec_asm  

Vq_subvec_asm FUNCTION

        STMFD       sp!, {r4 - r11, r14}
        SUB         r13, r13, #STACK_USED
        LDR         r10, [r13,#WF2]                                ; get wf2 address 
        STR         r0,  [r13, #LSF_R1]                            ; push lsf_r1 
        STR         r1,  [r13, #LSF_R2]                            ; push lsf_r2  

        VLD1.S16    D4, [r0]!                                      ; lsf_r1[0] ~ lsf_r1[3]
        VLD1.S16    D6, [r1]!                                      ; lsf_r2[0] ~ lsf_r2[3]
        VZIP.32     D4, D6                                         ; generate D4: lsf_r1[0], lsf_r1[1]; lsf_r2[0], lsf_r2[1]

        VLD1.S16    D5, [r3]!                                      ; wf1[0] ~ wf1[3]
        VLD1.S16    D6, [r10]!                                     ; wf2[0] ~ wf2[3]
        VZIP.32     D5, D6                                        ; generate D5: wf1[0], wf1[1]; wf2[0], wf2[1]
          
        MOV         r12, r2                                        ;p_dico = dico                  
        STR         r12, [r13, #DICO]                              ;push dico 

        LDR         r3,  [r13, #DICO_SIZE]                         ;r3-->dico_size 

        LDR         r6, =0x3fffffff  
        VDUP.S32    Q8 , r6                                        ;dist_min = MAX_32    
        
        ;ADR         r7, start_index
        ;VLD1.S32    Q6, [r7]
	MOV         r7, #0
	VMOV.S32    D12[0], r7
	MOV         r7, #1
	VMOV.S32    D12[1], r7
	MOV         r7, #2
	VMOV.S32    D13[0], r7
	MOV         r7, #3
	VMOV.S32    D13[1], r7

        LDR         r8, =0x4
        VDUP.S32    Q7, r8
                                                              
LOOP
        VLD1.S16    {D0, D1, D2, D3}, [r12]!                   ;load 16 p_dico[], p_dico[0] ~ p_dico[15]

        VQSUB.S16   D6,  D4, D0                                ; sub(lsf_r1[0], *p_dico++) ~ sub(lsf_r2[1], *p_dico++)
        VMULL.S16   Q13, D5, D6                              
        VSHRN.S32   D6, Q13, #15                               ; mult(wf1[0], temp) ~ mult(wf2[1], temp)
        VMULL.S16   Q9, D6, D6

        VQSUB.S16   D6,  D4, D1
        VMULL.S16   Q13, D5, D6
        VSHRN.S32   D6, Q13, #15
        VMULL.S16   Q10, D6, D6

        VQSUB.S16   D6,  D4, D2
        VMULL.S16   Q13, D5, D6
        VSHRN.S32   D6, Q13, #15
        VMULL.S16   Q11, D6, D6

        VQSUB.S16   D6,  D4, D3
        VMULL.S16   Q13, D5, D6
        VSHRN.S32   D6, Q13, #15
        VMULL.S16   Q12, D6, D6                                      ; get 4 dist
   
        VADD.S32    D18, D18, D19
        VPADD.S32   D18, D18, D18                              ; D18[0] is the sum of Q9

        VADD.S32    D20, D20, D21
        VPADD.S32   D20, D20, D20                           ; D20[0] is the sum of Q9 
        VMOV.S32    r4, D20[0]        
      
        VADD.S32    D22, D22, D23
        VPADD.S32   D22, D22, D22                           ; D22[0] is the sum of Q9 
        VMOV.S32    r5, D22[0] 

	VADD.S32    D24, D24, D25
        VPADD.S32   D24, D24, D24                           ; D24[0] is the sum of Q9
        VMOV.S32    r6, D24[0]

        VMOV.S32    D18[1], r4
        VMOV.S32    D19[0], r5
        VMOV.S32    D19[1], r6                                         ; Q9 store 4 sum result

        VCLT        Q5.U32, Q9.S32, Q8.S32
        VMIN.S32    Q8, Q8, Q9                             ; update Q8
        VBIT        Q15.S32, Q6.S32, Q5.U32                        ; store min index
        VADD.S32    Q6, Q6, Q7                    ; update Q6
        SUBS        r3, r3, #4
        BGT         LOOP
        
Vq_subvec_asm_end
        VEXT.S32    Q9, Q8, Q8, #2
        VEXT.S32    Q6, Q15, Q15, #2
        VCLT        Q5.U32, Q9.S32, Q8.S32
        VMIN.S32    Q8, Q8, Q9
        VBIT        Q15.S32, Q6.S32, Q5.U32
    
        VMOV.S32    r4, D16[0]
        VMOV.S32    r5, D16[1]
        VMOV.S32    r6, D30[0]
        VMOV.S32    r7, D30[1]      
 
        CMP         r4, r5
        MOVLT       r0, r6
        MOVGT       r0, r7
                 
        LDR         r7,[r13,#LSF_R1]                      ;r7--> lsf_r1  store lsf_r1 
        LDR         r8,[r13,#LSF_R2]                      ;r8--> lsf_r2 ;pop lsf_r2
        ADD         r10, r2, r0, LSL #3                   ;&dico[shl2(index, 2)]
        
        LDRSH       r6,[r10], #2                          ;r6--> lsf_r1[0]  
        LDRSH       r5,[r10], #2                          ;r5--> lsf_r1[1] 
        LDRSH       r4,[r10], #2                          ;r4--> lsf_r2[0]  
        LDRSH       r2,[r10], #2                          ;r2--> lsf_r2[1]       
        
        STRH        r6,[r7],#2
        STRH        r5,[r7]
        STRH        r4,[r8],#2
        STRH        r2,[r8]
               
        ADD         r13, r13, #STACK_USED  
        LDMFD       sp!, {r4 - r11, r15} 
 
;start_index
;        DCQ         0x0000000100000000,0x0000000300000002                        ;start_index[j] = j    
                         
        ENDFUNC      
        END





