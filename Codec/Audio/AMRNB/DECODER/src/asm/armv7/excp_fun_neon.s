;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;void excp_fun(Word16 *ptr, 
;	       Word16 excp[],
;	       Word16 pit_sharp,
;	       Word16 gain_pit)


	AREA	|.text|, CODE, READONLY
        EXPORT excp_fun 
        
excp_fun    PROC
        STMFD           r13!, {r4 - r12, r14}  
        MOV             r4, r0
        MOV             r5, r1
        VMOV.S16        D0[0], r2
        VMOV.S16        D0[1], r3 
        VLD1.S16        {D2, D3, D4, D5}, [r4]!
        VLD1.S16        {D6, D7, D8, D9}, [r4]! 
        VLD1.S16        {D10, D11}, [r4]!
        VMOV.S32        Q11, #0x8000

        VMULL.S16       Q10, D2, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D2, Q10, #16

        VMULL.S16       Q10, D3, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D3, Q10, #16

        VMULL.S16       Q10, D4, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D4, Q10, #16

        VMULL.S16       Q10, D5, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D5, Q10, #16

        VMULL.S16       Q10, D6, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D6, Q10, #16

        VMULL.S16       Q10, D7, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D7, Q10, #16

        VMULL.S16       Q10, D8, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D8, Q10, #16

        VMULL.S16       Q10, D9, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D9, Q10, #16

        VMULL.S16       Q10, D10, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D10, Q10, #16

        VMULL.S16       Q10, D11, D0[0]
        VSHRN.S32       D1, Q10, #15
        VQDMULL.S16     Q10, D1, D0[1]
        VSHR.S32        Q10, Q10, #1
        VQADD.S32       Q10, Q10, Q11
        VSHRN.S32       D11, Q10, #16          

        VST1.S16       {D2, D3, D4, D5}, [r1]!
        VST1.S16       {D6, D7, D8, D9}, [r1]!
        VST1.S16       {D10, D11}, [r1]!         
       
excp_fun_end 
 
        LDMFD      r13!, {r4 - r12, r15}
    
        ENDP
        END
