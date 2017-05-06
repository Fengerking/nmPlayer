;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;void Residu (
;    Word16 a[], /* (i)     : prediction coefficients                      */
;    Word16 x[], /* (i)     : speech signal                                */
;    Word16 y[], /* (o)     : residual signal                              */
;    Word16 lg   /* (i)     : size of filtering                            */
;)

	AREA	|.text|, CODE, READONLY
        EXPORT Vo_Residu_asm 

Vo_Residu_asm     FUNCTION

        STMFD          r13!, {r4 - r12, r14} 
        SUB            r7, r3, #4                       ;i = lg - 4
        
        VLD1.S16       {D0, D1, D2}, [r0]!              ;get all a[]  
        VMOV.S32       Q8,  #0x8000
        
        VDUP.S16       D4, D0[0]                       ;load a[0]
        VDUP.S16       D5, D0[1]                       ;load a[1]
        VDUP.S16       D6, D0[2]                       ;load a[2]
        VDUP.S16       D7, D0[3]                       ;load a[3]
        VDUP.S16       D8, D1[0]                       ;load a[4]
        VDUP.S16       D9, D1[1]                       ;load a[5]
        VDUP.S16       D10, D1[2]                      ;load a[6]
        VDUP.S16       D11, D1[3]                      ;load a[7]
        VDUP.S16       D12, D2[0]                      ;load a[8]
        VDUP.S16       D13, D2[1]                      ;load a[9]
        VDUP.S16       D14, D2[2]                      ;load a[10]
                               
LOOP1
        ADD            r9, r1, r7, LSL #1               ;copy the address
        ADD            r10, r2, r7, LSL #1
        MOV            r8, r9
        VLD1.S16       D3, [r8]!                       ;get x[i], x[i+1], x[i+2], x[i+3]
        VQDMULL.S16    Q10, D3, D4                    ;finish the first L_mult

        SUB            r8, r9, #2                       ;get the x[i-1] address
        VLD1.S16       D3, [r8]! 
        VQDMLAL.S16    Q10, D3, D5

        SUB            r8, r9, #4                       ;load the x[i-2] address
        VLD1.S16       D3, [r8]!
        VQDMLAL.S16    Q10, D3, D6

        SUB            r8, r9, #6                       ;load the x[i-3] address
        VLD1.S16       D3, [r8]!     
        VQDMLAL.S16    Q10, D3, D7                    

        SUB            r8, r9, #8                       ;load the x[i-4] address
        VLD1.S16       D3, [r8]!     
        VQDMLAL.S16    Q10, D3, D8  

        SUB            r8, r9, #10                      ;load the x[i-5] address
        VLD1.S16       D3, [r8]!     
        VQDMLAL.S16    Q10, D3, D9 

        SUB            r8, r9, #12                      ;load the x[i-6] address
        VLD1.S16            D3, [r8]!     
        VQDMLAL.S16    Q10, D3, D10  

        SUB            r8, r9, #14                      ;load the x[i-7] address
        VLD1.S16            D3, [r8]!     
        VQDMLAL.S16    Q10, D3, D11  

        SUB            r8, r9, #16                      ;load the x[i-8] address
        VLD1.S16            D3, [r8]!     
        VQDMLAL.S16    Q10, D3, D12  

        SUB            r8, r9, #18                      ;load the x[i-9] address
        VLD1.S16       D3, [r8]!     
        VQDMLAL.S16    Q10, D3, D13         
           
        SUB            r8, r9, #20                      ;load the x[i-10] address
        VLD1.S16       D3, [r8]!     
        VQDMLAL.S16    Q10, D3, D14  

        SUB            r7, r7, #4                       ;i-=4
        VQSHL.S32      Q10, Q10, #3
        VQADD.S32      Q10, Q10, Q8
        VSHRN.S32      D3, Q10, #16
        VST1.S16       D3, [r10]!
        CMP            r7,  #0

        BGE            LOOP1

Residu_asm_end 
 
        LDMFD      r13!, {r4 - r12, r15}
    
        ENDFUNC
        END
